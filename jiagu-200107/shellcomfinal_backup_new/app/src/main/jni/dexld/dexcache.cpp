#include <string.h>
#include <jni.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/system_properties.h>
#include <dirent.h>
#include <pthread.h>
#include <unistd.h>
#include <zlib.h>
#include <zconf.h>
#include <stdint.h>
#include <stdarg.h>
#include <vector>
#include <iostream>
#include <sys/mman.h>
#include <sys/socket.h>
#include <android/log.h>
#include <linux/ashmem.h>
#include "../sec.h"

#if defined(_TEST_DEBUG_ANDROID)

#define LOGTEST(...) __android_log_print(3,"dexCache", __VA_ARGS__)
#else
#define LOGTEST(...)
#endif


#define DEX_MAGIC       "dex\n"
#define DEX_MAGIC_VERS  "036\0"
#define DEX_MAGIC_VERS_API_13  "035\0"

/* auxillary data section chunk codes */
enum {
    kDexChunkClassLookup1            = 0x434c4b50,   /* CLKP */
    kDexChunkRegisterMaps1           = 0x524d4150,   /* RMAP */

    kDexChunkEnd1                    = 0x41454e44,   /* AEND */
};

enum { kSHA1DigestLen1 = 20, kSHA1DigestOutputLen1 = kSHA1DigestLen1*2 +1 };

//static DvmGlobals_14*	gDvm14;
static DvmGlobals_19*   gDvm19;
static DvmGlobals_15*   gDvm15;
static DvmGlobals_16*   gDvm16;
static DvmGlobals_17*   gDvm17;
static DvmGlobals_18*   gDvm18;

static bool rewriteDex(u1* addr, int len, bool doVerify, bool doOpt,
    DexClassLookup** ppClassLookup, DvmDex** ppDvmDex, int sdkType);
static bool loadAllClasses(DvmDex* pDvmDex, int sdkType);
static void verifyAndOptimizeClasses(DexFile* pDexFile, bool doVerify,
    bool doOpt);
static void verifyAndOptimizeClass(DexFile* pDexFile, ClassObject* clazz,
    const DexClassDef* pClassDef, bool doVerify, bool doOpt);
static void updateChecksum(u1* addr, int len, DexHeader* pHeader);

static bool writeOptData(u1** fdBuf, const DexClassLookup* pClassLookup,\
    const RegisterMapBuilder* pRegMapBuilder, ssize_t &posTemp);
static int writeDependencies(const char *packageLocation, u1** fdBuf, u4 modWhen, u4 crc, ssize_t &pos, int sdkType);
static int sysWriteFully(void* fdBuf, const void* buf, size_t count, const char* logMsg);
static bool computeFileChecksum(void* fdBuf, off_t start, size_t length, u4* pSum);


/*
 * Do the actual optimization.  This is executed in the dexopt process.
 *
 * For best use of disk/memory, we want to extract once and perform
 * optimizations in place.  If the file has to expand or contract
 * to match local structure padding/alignment expectations, we want
 * to do the rewrite as part of the extract, rather than extracting
 * into a temp file and slurping it back out.  (The structure alignment
 * is currently correct for all platforms, and this isn't expected to
 * change, so we should be okay with having it already extracted.)
 *
 * Returns "true" on success.
 */
 
 u4 dexRoundUpPower2(u4 val)
{
    val--;
    val |= val >> 1;
    val |= val >> 2;
    val |= val >> 4;
    val |= val >> 8;
    val |= val >> 16;
    val++;

    return val;
}
static u4 classDescriptorHash(const char* str)
{
    u4 hash = 1;

    while (*str != '\0')
        hash = hash * 31 + *str++;

    return hash;
}
 static void classLookupAdd(DexFile* pDexFile, DexClassLookup* pLookup,
    int stringOff, int classDefOff, int* pNumProbes)
{
    const char* classDescriptor =
        (const char*) (pDexFile->baseAddr + stringOff);
    const DexClassDef* pClassDef =
        (const DexClassDef*) (pDexFile->baseAddr + classDefOff);
    u4 hash = classDescriptorHash(classDescriptor);
    int mask = pLookup->numEntries-1;
    int idx = hash & mask;

    /*
     * Find the first empty slot.  We oversized the table, so this is
     * guaranteed to finish.
     */
    int probes = 0;
    while (pLookup->table[idx].classDescriptorOffset != 0) {
        idx = (idx + 1) & mask;
        probes++;
    }
    //if (probes > 1)
    //    ALOGW("classLookupAdd: probes=%d", probes);

    pLookup->table[idx].classDescriptorHash = hash;
    pLookup->table[idx].classDescriptorOffset = stringOff;
    pLookup->table[idx].classDefOffset = classDefOff;
    *pNumProbes = probes;
}

static void dexFileSetupBasicPointers(DexFile* pDexFile, const u1* data) {
    DexHeader *pHeader = (DexHeader*) data;

    pDexFile->baseAddr = (const char*)data;
    pDexFile->pHeader = pHeader;
    pDexFile->pStringIds = (const DexStringId*) (data + pHeader->stringIdsOff);
    pDexFile->pTypeIds = (const DexTypeId*) (data + pHeader->typeIdsOff);
    pDexFile->pFieldIds = (const void*)(data + pHeader->fieldIdsOff);
    pDexFile->pMethodIds = (const void*) (data + pHeader->methodIdsOff);
    pDexFile->pProtoIds = (const void*) (data + pHeader->protoIdsOff);
    pDexFile->pClassDefs = (const DexClassDef*) (data + pHeader->classDefsOff);
    pDexFile->pLinkData = (const void *) (data + pHeader->linkOff);
}

static bool isValidPointer(const void* ptr, const void* start, const void* end)
{
    return (ptr >= start) && (ptr < end) && (((u4) ptr & 7) == 0);
}
 
 DexClassLookup* dexCreateClassLookup(DexFile* pDexFile)
{
    DexClassLookup* pLookup;
    int allocSize;
    int i, numEntries;
    int numProbes, totalProbes, maxProbes;

    numProbes = totalProbes = maxProbes = 0;

    assert(pDexFile != NULL);

    /*
     * Using a factor of 3 results in far less probing than a factor of 2,
     * but almost doubles the flash storage requirements for the bootstrap
     * DEX files.  The overall impact on class loading performance seems
     * to be minor.  We could probably get some performance improvement by
     * using a secondary hash.
     */
    numEntries = dexRoundUpPower2(pDexFile->pHeader->classDefsSize * 2);
    allocSize = offsetof(DexClassLookup, table)
                    + numEntries * sizeof(pLookup->table[0]);

    pLookup = (DexClassLookup*) calloc(1, allocSize);
    if (pLookup == NULL)
        return NULL;
    pLookup->size = allocSize;
    pLookup->numEntries = numEntries;

    for (i = 0; i < (int)pDexFile->pHeader->classDefsSize; i++) {
        const DexClassDef* pClassDef;
        const char* pString;

        pClassDef = dexGetClassDef(pDexFile, i);
        pString = dexStringByTypeIdx(pDexFile, pClassDef->classIdx);

        classLookupAdd(pDexFile, pLookup,
            (u1*)pString - (u1 *)pDexFile->baseAddr,
            (u1*)pClassDef - (u1 *)pDexFile->baseAddr, &numProbes);

        if (numProbes > maxProbes)
            maxProbes = numProbes;
        totalProbes += numProbes;
    }

    LOGTEST("Class lookup: classes=%d slots=%d (%d%% occ) alloc=%d"
         " total=%d max=%d",
        pDexFile->pHeader->classDefsSize, numEntries,
        (100 * pDexFile->pHeader->classDefsSize) / numEntries,
        allocSize, totalProbes, maxProbes);

    return pLookup;
}

static bool dexHasValidMagic(const DexHeader* pHeader)
{
    const u1* magic = pHeader->magic;
    const u1* version = &magic[4];

    if (memcmp(magic, DEX_MAGIC, 4) != 0) {

        return false;
    }

    if ((memcmp(version, DEX_MAGIC_VERS, 4) != 0) &&
            (memcmp(version, DEX_MAGIC_VERS_API_13, 4) != 0)) {
        /*
         * Magic was correct, but this is an unsupported older or
         * newer format variant.
         */

        return false;
    }

    return true;
}
static bool dexParseOptData(const u1* data, size_t length, DexFile* pDexFile)
{
    const void* pOptStart = data + pDexFile->pOptHeader->optOffset;
    const void* pOptEnd = data + length;
    const u4* pOpt = (const u4*) pOptStart;
    u4 optLength = (const u1*) pOptEnd - (const u1*) pOptStart;

    /*
     * Make sure the opt data start is in range and aligned. This may
     * seem like a superfluous check, but (a) if the file got
     * truncated, it might turn out that pOpt >= pOptEnd; and (b)
     * if the opt data header got corrupted, pOpt might not be
     * properly aligned. This test will catch both of these cases.
     */
    if (!isValidPointer(pOpt, pOptStart, pOptEnd)) {
        printf("Bogus opt data start pointer");
        return false;
    }

    /* Make sure that the opt data length is a whole number of words. */
    if ((optLength & 3) != 0) {
        printf("Unaligned opt data area end");
        return false;
    }

    /*
     * Make sure that the opt data area is large enough to have at least
     * one chunk header.
     */
    if (optLength < 8) {
        printf("Undersized opt data area (%u)", optLength);
        return false;
    }

    /* Process chunks until we see the end marker. */
    while (*pOpt != kDexChunkEnd) {
        if (!isValidPointer(pOpt + 2, pOptStart, pOptEnd)) {
            printf("Bogus opt data content pointer at offset %u",
                    ((const u1*) pOpt) - data);
            return false;
        }

        u4 size = *(pOpt + 1);
        const u1* pOptData = (const u1*) (pOpt + 2);

        /*
         * The rounded size is 64-bit aligned and includes +8 for the
         * type/size header (which was extracted immediately above).
         */
        u4 roundedSize = (size + 8 + 7) & ~7;
        const u4* pNextOpt = pOpt + (roundedSize / sizeof(u4));

        if (!isValidPointer(pNextOpt, pOptStart, pOptEnd)) {
            printf("Opt data area problem for chunk of size %u at offset %u",
                    size, ((const u1*) pOpt) - data);
            return false;
        }

        switch (*pOpt) {
        case kDexChunkClassLookup:
            pDexFile->pClassLookup = (const DexClassLookup*) pOptData;
            break;
        case kDexChunkRegisterMaps:
            printf("+++ found register maps, size=%u", size);
            pDexFile->pRegisterMapPool = pOptData;
            break;
        default:
            printf("Unknown chunk 0x%08x (%c%c%c%c), size=%d in opt data area",
                *pOpt,
                (char) ((*pOpt) >> 24), (char) ((*pOpt) >> 16),
                (char) ((*pOpt) >> 8),  (char)  (*pOpt),
                size);
            break;
        }

        pOpt = pNextOpt;
    }

    return true;
}

 void dexFileFree(DexFile* pDexFile)
{
    if (pDexFile == NULL)
        return;

    free(pDexFile);
}

static DexFile* dexFileParse(const u1* data, size_t length, int flags)
{
    DexFile* pDexFile = NULL;
    const DexHeader* pHeader;
    const u1* magic;
    int result = -1;

    if (length < sizeof(DexHeader)) {
		LOGTEST("dex file parse enter...");
        goto bail;      /* bad file format */
    }

    pDexFile = (DexFile*) malloc(sizeof(DexFile));
    if (pDexFile == NULL)
    {
    	LOGTEST("dex file is null");
        goto bail;      /* alloc failure */
    }
    memset(pDexFile, 0, sizeof(DexFile));

    /*
     * Peel off the optimized header.
     */
    if (memcmp(data, DEX_OPT_MAGIC, 4) == 0) {
        //magic = data;
        //if (memcmp(magic+4, DEX_OPT_MAGIC_VERS, 4) != 0) {
		//	LOGI("cmp magic failed");
        //    goto bail;
        //}

        pDexFile->pOptHeader = (DexOptHeader*) data;

        /* parse the optimized dex file tables */
        if (!dexParseOptData(data, length, pDexFile))
        {
        	LOGTEST("parse opt data failed");
            goto bail;
        }

        /* ignore the opt header and appended data from here on out */
        data += pDexFile->pOptHeader->dexOffset;
        length -= pDexFile->pOptHeader->dexOffset;
        if (pDexFile->pOptHeader->dexLength > length) {
			LOGTEST("optheader len is too long");
            goto bail;
        }
        length = pDexFile->pOptHeader->dexLength;
    }

    dexFileSetupBasicPointers(pDexFile, data);
    pHeader = pDexFile->pHeader;

    if (!dexHasValidMagic(pHeader)) {
		LOGTEST("dex has valid magic failed");
        goto bail;
    }

    if (pHeader->classDefsSize == 0) {
		LOGTEST("class def size failed");
        goto bail;
    }

    /*
     * Success!
     */
    result = 0;

bail:
    if (result != 0 && pDexFile != NULL) {
        dexFileFree(pDexFile);
        pDexFile = NULL;
    }
    return pDexFile;
}

#define SYSTEM_PAGE_SIZE 4096
 #define ALIGN_UP(x, n) (((size_t)(x) + (n) - 1) & ~((n) - 1))

#define ALIGN_UP_TO_PAGE_SIZE(p) ALIGN_UP(p, SYSTEM_PAGE_SIZE)

int ashmem_create_region(const char *name, size_t len)
{
    return open("/dev/zero", O_RDWR);
}
void *dvmAllocRegion(unsigned int  byteCount, int prot, const char *name) {
    void *base;
    int fd, ret;

    byteCount = ALIGN_UP_TO_PAGE_SIZE(byteCount);
    fd = ashmem_create_region(name, byteCount);
    if (fd == -1) {
        return NULL;
    }
    base = mmap(NULL, byteCount, prot, MAP_PRIVATE, fd, 0);
    ret = close(fd);
    if (base == MAP_FAILED) {
        return NULL;
    }
    if (ret == -1) {
        munmap(base, byteCount);
        return NULL;
    }
    return base;
}



static DvmDex* allocateAuxStructures(DexFile* pDexFile) {
   	DvmDex* pDvmDex;
    const DexHeader* pHeader;
    u4 stringSize, classSize, methodSize, fieldSize;

    pHeader = pDexFile->pHeader;

    stringSize = pHeader->stringIdsSize * sizeof(struct StringObject*);
    classSize  = pHeader->typeIdsSize * sizeof(struct ClassObject*);
    methodSize = pHeader->methodIdsSize * sizeof(struct Method*);
    fieldSize  = pHeader->fieldIdsSize * sizeof(struct Field*);

    u4 totalSize = sizeof(DvmDex) +
                   stringSize + classSize + methodSize + fieldSize;

    u1 *blob = (u1 *)dvmAllocRegion(totalSize,
                              PROT_READ | PROT_WRITE, "dalvik-aux-structure");
    if ((void *)blob == MAP_FAILED)
        return NULL;

    pDvmDex = (DvmDex*)blob;
    blob += sizeof(DvmDex);

   pDvmDex->pDexFile = pDexFile;
    pDvmDex->pHeader = pHeader;

    pDvmDex->pResStrings = (struct StringObject**)blob;
    blob += stringSize;
    pDvmDex->pResClasses = (struct ClassObject**)blob;
    blob += classSize;
    pDvmDex->pResMethods = (struct Method**)blob;
    blob += methodSize;
    pDvmDex->pResFields = (struct Field**)blob;

   LOGTEST("+++ DEX %p: allocateAux (%d+%d+%d+%d)*4 = %d bytes",
        pDvmDex, stringSize/4, classSize/4, methodSize/4, fieldSize/4,
       stringSize + classSize + methodSize + fieldSize);

    pDvmDex->pInterfaceCache = NULL;

    //dvmInitMutex(&pDvmDex->modLock);

    return pDvmDex;
}


void dvmDexFileFree(DvmDex* pDvmDex)
{
    u4 totalSize;

    if (pDvmDex == NULL)
        return;

    //dvmDestroyMutex(&pDvmDex->modLock);

    totalSize  = pDvmDex->pHeader->stringIdsSize * sizeof(struct StringObject*);
    totalSize += pDvmDex->pHeader->typeIdsSize * sizeof(struct ClassObject*);
    totalSize += pDvmDex->pHeader->methodIdsSize * sizeof(struct Method*);
    totalSize += pDvmDex->pHeader->fieldIdsSize * sizeof(struct Field*);
    totalSize += sizeof(DvmDex);

    dexFileFree(pDvmDex->pDexFile);

    LOGTEST("+++ DEX %p: freeing aux structs", pDvmDex);
    //dvmFreeAtomicCache(pDvmDex->pInterfaceCache);
    //sysReleaseShmem(&pDvmDex->memMap);
    munmap(pDvmDex, totalSize);
}

enum {
    kDexParseDefault            = 0,
    kDexParseVerifyChecksum     = 1,
   kDexParseContinueOnError    = (1 << 1),
};

int dvmDexFileOpenPartial(const void* addr, int len, DvmDex** ppDvmDex)
{
    DvmDex* pDvmDex;
    DexFile* pDexFile;
    int parseFlags = kDexParseDefault;
    int result = -1;

    /* -- file is incomplete, new checksum has not yet been calculated
    if (gDvm.verifyDexChecksum)
        parseFlags |= kDexParseVerifyChecksum;
    */

    pDexFile = dexFileParse((u1*)addr, len, parseFlags);
    if (pDexFile == NULL) {
        LOGTEST("DEX parse failed");
        goto bail;
    }
    pDvmDex = allocateAuxStructures(pDexFile);
    if (pDvmDex == NULL) {
        dexFileFree(pDexFile);
        goto bail;
    }

    pDvmDex->isMappedReadOnly = false;
    *ppDvmDex = pDvmDex;
    result = 0;

bail:
    return result;
}


enum RegisterMapFormat {
    kRegMapFormatUnknown = 0,
    kRegMapFormatNone,          
    kRegMapFormatCompact8,      
    kRegMapFormatCompact16,     
    kRegMapFormatDifferential,  

    kRegMapFormatOnHeap = 0x80, 
};

enum {
    ACC_PUBLIC       = 0x00000001,       // class, field, method, ic
    ACC_PRIVATE      = 0x00000002,       // field, method, ic
    ACC_PROTECTED    = 0x00000004,       // field, method, ic
    ACC_STATIC       = 0x00000008,       // field, method, ic
    ACC_FINAL        = 0x00000010,       // class, field, method, ic
    ACC_SYNCHRONIZED = 0x00000020,       // method (only allowed on natives)
    ACC_SUPER        = 0x00000020,       // class (not used in Dalvik)
    ACC_VOLATILE     = 0x00000040,       // field
    ACC_BRIDGE       = 0x00000040,       // method (1.5)
    ACC_TRANSIENT    = 0x00000080,       // field
    ACC_VARARGS      = 0x00000080,       // method (1.5)
    ACC_NATIVE       = 0x00000100,       // method
    ACC_INTERFACE    = 0x00000200,       // class, ic
    ACC_ABSTRACT     = 0x00000400,       // class, method, ic
    ACC_STRICT       = 0x00000800,       // method
    ACC_SYNTHETIC    = 0x00001000,       // field, method, ic
    ACC_ANNOTATION   = 0x00002000,       // class, ic (1.5)
    ACC_ENUM         = 0x00004000,       // class, field, ic (1.5)
    ACC_CONSTRUCTOR  = 0x00010000,       // method (Dalvik only)
    ACC_DECLARED_SYNCHRONIZED =
                       0x00020000,       // method (Dalvik only)
};

enum AccessFlags {
    ACC_MIRANDA         = 0x8000,       // method (internal to VM)
    JAVA_FLAGS_MASK     = 0xffff,       // bits set from Java sources (low 16)
};

 bool dvmIsNativeMethod(const Method* method) {
    return (method->accessFlags & ACC_NATIVE) != 0;
}
 bool dvmIsAbstractMethod(const Method* method) {
    return (method->accessFlags & ACC_ABSTRACT) != 0;
}

RegisterMapFormat dvmRegisterMapGetFormat(const RegisterMap* pMap) {
    return (RegisterMapFormat)(pMap->format & ~(kRegMapFormatOnHeap));
}

 u2 dvmRegisterMapGetNumEntries(const RegisterMap* pMap) {
    return pMap->numEntries[0] | (pMap->numEntries[1] << 8);
}

int readUnsignedLeb128(const u1** pStream) {
    const u1* ptr = *pStream;
    int result = *(ptr++);

    if (result > 0x7f) {
        int cur = *(ptr++);
        result = (result & 0x7f) | ((cur & 0x7f) << 7);
        if (cur > 0x7f) {
            cur = *(ptr++);
            result |= (cur & 0x7f) << 14;
            if (cur > 0x7f) {
               cur = *(ptr++);
                result |= (cur & 0x7f) << 21;
                if (cur > 0x7f) {
                 
                    cur = *(ptr++);
                    result |= cur << 28;
                }
            }
        }
    }

    *pStream = ptr;
    return result;
}


static size_t computeRegisterMapSize(const RegisterMap* pMap)
{
    static const int kHeaderSize = offsetof(RegisterMap, data);
    u1 format = dvmRegisterMapGetFormat(pMap);
    u2 numEntries = dvmRegisterMapGetNumEntries(pMap);

    assert(pMap != NULL);

    switch (format) {
    case kRegMapFormatNone:
        return 1;
    case kRegMapFormatCompact8:
        return kHeaderSize + (1 + pMap->regWidth) * numEntries;
    case kRegMapFormatCompact16:
        return kHeaderSize + (2 + pMap->regWidth) * numEntries;
    case kRegMapFormatDifferential:
        {
            
            const u1* ptr = pMap->data;
            int len = readUnsignedLeb128(&ptr);
            return len + (ptr - (u1*) pMap);
        }
    default:
        LOGTEST("Bad register map format %d", format);
        dvmAbort();
        return 0;
    }
}





static bool writeMapForMethod(const Method* meth, u1** pPtr)
{
    if (meth->registerMap == NULL) {
        if (!dvmIsAbstractMethod(meth) && !dvmIsNativeMethod(meth)) {
            LOGTEST("Warning: no map available for %s.%s",
                meth->clazz->descriptor, meth->name);
            
        }
        *(*pPtr)++ = kRegMapFormatNone;
        return true;
    }

    
    size_t mapSize = computeRegisterMapSize(meth->registerMap);
    memcpy(*pPtr, meth->registerMap, mapSize);

    
    assert(**pPtr == meth->registerMap->format);
    **pPtr &= ~(kRegMapFormatOnHeap);

    *pPtr += mapSize;

    return true;
}

bool dvmIsMirandaMethod(const Method* method) {
    return (method->accessFlags & ACC_MIRANDA) != 0;
}

static inline u1* align32(u1* ptr)
{
    return (u1*) (((int) ptr + 3) & ~0x03);
}


static bool writeMapsAllMethods(DvmDex* pDvmDex, const ClassObject* clazz,
    u1** pPtr, size_t length)
{
    RegisterMapMethodPool* pMethodPool;
    u1* ptr = *pPtr;
    int i, methodCount;

  
    if (clazz->virtualMethodCount + clazz->directMethodCount >= 65536) {
        LOGTEST("Too many methods in %s", clazz->descriptor);
        return false;
    }

    pMethodPool = (RegisterMapMethodPool*) ptr;
    ptr += offsetof(RegisterMapMethodPool, methodData);
    methodCount = 0;


    for (i = 0; i < clazz->directMethodCount; i++) {
        const Method* meth = &clazz->directMethods[i];
        if (dvmIsMirandaMethod(meth))
            continue;
        if (!writeMapForMethod(&clazz->directMethods[i], &ptr)) {
            return false;
        }
        methodCount++;
        //ptr = align32(ptr);
    }

    for (i = 0; i < clazz->virtualMethodCount; i++) {
        const Method* meth = &clazz->virtualMethods[i];
        if (dvmIsMirandaMethod(meth))
            continue;
        if (!writeMapForMethod(&clazz->virtualMethods[i], &ptr)) {
            return false;
        }
        methodCount++;
        //ptr = align32(ptr);
    }

    pMethodPool->methodCount = methodCount;

    *pPtr = ptr;
    return true;
}


static size_t writeMapsAllClasses(DvmDex* pDvmDex, u1* basePtr, size_t length)
{
    DexFile* pDexFile = pDvmDex->pDexFile;
    u4 count = pDexFile->pHeader->classDefsSize;
    RegisterMapClassPool* pClassPool;
    u4* offsetTable;
    u1* ptr = basePtr;
    u4 idx;

    assert(gDvm.optimizing);

    pClassPool = (RegisterMapClassPool*) ptr;
    ptr += offsetof(RegisterMapClassPool, classDataOffset);
    offsetTable = (u4*) ptr;
    ptr += count * sizeof(u4);

    pClassPool->numClasses = count;


    for (idx = 0; idx < count; idx++) {
        const DexClassDef* pClassDef;
        const char* classDescriptor;
        ClassObject* clazz;

        pClassDef = dexGetClassDef(pDexFile, idx);
        classDescriptor = dexStringByTypeIdx(pDexFile, pClassDef->classIdx);


        clazz = NULL;
        if ((pClassDef->accessFlags & CLASS_ISPREVERIFIED) != 0)
        	{
        		LOGTEST("dvmLookupClass in\n");
        			void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );
              u4 count = pDexFile->pHeader->classDefsSize;
              u4 idx;
              typedef ClassObject* (*dvmLookupClass_t)(const char*, Object* , bool);
              dvmLookupClass_t dvmLookupClass = (dvmLookupClass_t)dlsym(ldvm, "_Z14dvmLookupClassPKcP6Objectb");
              if(dvmLookupClass == NULL)
              {
               LOGTEST("find dvmLookupClass fail\n");
               return ptr - basePtr;
              }
            clazz = dvmLookupClass(classDescriptor, NULL, false);
          }
        if (clazz != NULL) {
            offsetTable[idx] = ptr - basePtr;
            LOGTEST("%d -> offset %d (%p-%p)",
                idx, offsetTable[idx], ptr, basePtr);

            if (!writeMapsAllMethods(pDvmDex, clazz, &ptr,
                    length - (ptr - basePtr)))
            {
                return 0;
            }

            ptr = align32(ptr);
            LOGTEST("Size %s (%d+%d methods): %d", clazz->descriptor,
                clazz->directMethodCount, clazz->virtualMethodCount,
                (ptr - basePtr) - offsetTable[idx]);
        } else {
            //LOGTEST("%4d NOT mapadding '%s'", idx, classDescriptor);
            assert(offsetTable[idx] == 0);
        }
    }

    if (ptr - basePtr >= (int)length) {
 
        LOGTEST("Buffer overrun");
        exit(1);
    }

    return ptr - basePtr;
}

#define HAVE_POSIX_FILEMAP

static void* sysCreateAnonShmem(size_t length)
{
#ifdef HAVE_POSIX_FILEMAP
    void* ptr;

    ptr = mmap(NULL, length, PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANON, -1, 0);
    if (ptr == MAP_FAILED) {
        LOGTEST("mmap(%d, RW, SHARED|ANON)", (int) length);
        return NULL;
    }

    return ptr;
#else
    LOGTEST("sysCreateAnonShmem not implemented.");
    return NULL;
#endif
}


int sysCreatePrivateMap(size_t length, MemMapping* pMap)
{
    void* memPtr;

    memPtr = sysCreateAnonShmem(length);
    if (memPtr == NULL)
        return -1;

    pMap->addr = pMap->baseAddr = (char *)memPtr;
    pMap->length = pMap->baseLength = length;
    return 0;
}

void sysReleaseShmem(MemMapping* pMap)
{
    if (pMap->baseAddr == NULL && pMap->baseLength == 0)
        return;

   if (munmap(pMap->baseAddr, pMap->baseLength) < 0) {
        LOGTEST("munmap(%p, %d)\n",
            pMap->baseAddr, (int)pMap->baseLength);
    } else {
        LOGTEST("munmap(%p, %d) succeeded\n", pMap->baseAddr, pMap->baseLength);
        pMap->baseAddr = NULL;
        pMap->baseLength = 0;
    }
}

void dvmFreeRegisterMapBuilder(RegisterMapBuilder* pBuilder)
{
    if (pBuilder == NULL)
        return;

    sysReleaseShmem(&pBuilder->memMap);
    free(pBuilder);
}

RegisterMapBuilder* dvmGenerateRegisterMaps(DvmDex* pDvmDex)
{
    RegisterMapBuilder* pBuilder;

    pBuilder = (RegisterMapBuilder*) calloc(1, sizeof(RegisterMapBuilder));
    if (pBuilder == NULL)
        return NULL;


    if (sysCreatePrivateMap(4 * 1024 * 1024, &pBuilder->memMap) != 0) {
        free(pBuilder);
        return NULL;
    }


    size_t actual = writeMapsAllClasses(pDvmDex, (u1*)pBuilder->memMap.addr,
                                        pBuilder->memMap.length);
    if (actual == 0) {
        dvmFreeRegisterMapBuilder(pBuilder);
        return NULL;
    }

    LOGTEST("TOTAL size of register maps: %d", actual);

    pBuilder->data = pBuilder->memMap.addr;
    pBuilder->size = actual;
    return pBuilder;
}





bool dvmContinueOptimization(const char *packageLocation, void* fdBuf, u1** OutBuf, off_t dexOffset, u4 crc, u4 modWhen,long dexLengthAll,
     int sdkType, long* OdexLength)
{
    //u4 modWhen(0);
    //u4 crc(0);
    bool isBootstrap(true);
	int adjDexPos(0);
	int adjDependPos(0);
	int adjAllPos(0);
	int OutBufPos(0);
	int DependSize(0);
	int OptSize(0);
    long dexLength(0);
	u1* DependBuf = 0;
	u1* OptBuf = 0;
	
	const char* bcp = getenv("BOOTCLASSPATH");
	
	
	u4 tempDexLength(0);
	memcpy(&tempDexLength, fdBuf+32, 4);
	dexLength = tempDexLength;

	void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );
	if (ldvm == NULL){
	LOGTEST("open libdvm fail\n");
	return JNI_ERR;}
	ssize_t posTemp(0);
	ssize_t offpos(0);
	typedef void (*dvmFreeRegisterMapBuilder_t)(RegisterMapBuilder*);
    DexClassLookup* pClassLookup = NULL;
    RegisterMapBuilder* pRegMapBuilder = NULL;

    /* quick test so we don't blow up on empty file */
    if (dexLength < (int) sizeof(DexHeader)) {
//        LOGE("too small to be DEX");
        return false;
    }
    if (dexOffset < (int) sizeof(DexOptHeader)) {
//        LOGE("not enough room for opt header");
        return false;
    }

    bool result = false;

    /*
     * Drop this into a global so we don't have to pass it around.  We could
     * also add a field to DexFile, but since it only pertains to DEX
     * creation that probably doesn't make sense.
     */
    

    {
        /*
         * Map the entire file (so we don't have to worry about page
         * alignment).  The expectation is that the output file contains
         * our DEX data plus room for a small header.
         */
        bool success;

        bool doVerify(false), doOpt(false);

        /*
         * Rewrite the file.  Byte reordering, structure realigning,
         * class verification, and bytecode optimization are all performed
         * here.
         *
         * In theory the file could change size and bits could shift around.
         * In practice this would be annoying to deal with, so the file
         * layout is designed so that it can always be rewritten in place.
         *
         * This creates the class lookup table as part of doing the processing.
         */
        // typedef bool (*rewriteDex_t)(u1*, int, bool , bool ,DexClassLookup** , DvmDex** );
        //typedef int (*dvmDexFileOpenPartial_t)(const void* , int , DvmDex** );
        typedef RegisterMapBuilder* (*dvmGenerateRegisterMaps_t)(DvmDex*);
        // typedef void (*updateChecksum_t)(u1* , int , DexHeader* );
        typedef void (*dvmDexFileFree_t)(DvmDex*);


//´Ë´¦Ïàµ±ÓÚµ÷ÓÃÔ­À´µÄº¯Êý£¬½«dexÔ­ÎÄ¼þÖÐµÄÄÚÈÝÓÅ»¯ÖØÐ´µ½pClassLookup£¬pRegMapBuilderÖÐ

        success = rewriteDex(((u1*) fdBuf), dexLength, doVerify, doOpt, &pClassLookup, NULL, sdkType);
        if (success) {
            DvmDex* pDvmDex = NULL;
            u1* dexAddr = ((u1*) fdBuf);
            if (dvmDexFileOpenPartial(dexAddr, dexLength, &pDvmDex) != 0) {
                LOGTEST("Unable to create DexFile");
                success = false;
            } else {
                /*
                 * If configured to do so, generate register map output
                 * for all verified classes.  The register maps were
                 * generated during verification, and will now be serialized.
                 */
                if (true) {
                	/*
                	dvmGenerateRegisterMaps_t dvmGenerateRegisterMaps = (dvmGenerateRegisterMaps_t)dlsym(ldvm, "_Z23dvmGenerateRegisterMapsP6DvmDex");
                if(dvmGenerateRegisterMaps == NULL)
            	{
            		LOGTEST("find dvmGenerateRegisterMaps fail\n");
            		success = false;;
            	}
            	*/
                    pRegMapBuilder = dvmGenerateRegisterMaps(pDvmDex);
                    if (pRegMapBuilder == NULL) {
//                        LOGE("Failed generating register maps");
                        success = false;
                    }
                }

                DexHeader* pHeader = (DexHeader*)pDvmDex->pHeader;
//                updateChecksum_t updateChecksum = (updateChecksum_t)dlsym(ldvm,"updateChecksum");
                updateChecksum(dexAddr, dexLength, pHeader);

/*
                dvmDexFileFree_t dvmDexFileFree = (dvmDexFileFree_t)dlsym(ldvm,"_Z14dvmDexFileFreeP6DvmDex");
              if(dvmDexFileFree == NULL)
            	{
            		LOGTEST("find dvmDexFileFree fail\n");
            		return false;
            	}*/
                dvmDexFileFree(pDvmDex);
            }
        }

        /* unmap the read-write version, forcing writes to disk */
//        if (msync(mapAddr, dexOffset + dexLength, MS_SYNC) != 0) {
////            LOGW("msync failed: %s", strerror(errno));
//            // weird, but keep going
//        }
#if 1
        /*
         * This causes clean shutdown to fail, because we have loaded classes
         * that point into it.  For the optimizer this isn't a problem,
         * because it's more efficient for the process to simply exit.
         * Exclude this code when doing clean shutdown for valgrind.
         */
//        if (munmap(mapAddr, dexOffset + dexLength) != 0) {
//            LOGE("munmap failed: %s", strerror(errno));
//            goto bail;
//        }
#endif

        if (!success)
            goto bail;
    }

    /* get start offset, and adjust deps start for 64-bit alignment */
    off_t depsOffset, optOffset, endOffset, adjOffset;
    int depsLength, optLength;
    u4 optChecksum;

//    depsOffset = lseek(fd, 0, SEEK_END);
    depsOffset = dexLength + sizeof(DexOptHeader);
    if (depsOffset < 0) {
//        LOGE("lseek to EOF failed: %s", strerror(errno));
        goto bail;
    }
    adjOffset = (depsOffset + 7) & ~(0x07);
    if (adjOffset != depsOffset) {
//        LOGV("Adjusting deps start from %d to %d",
//            (int) depsOffset, (int) adjOffset);
        depsOffset = adjOffset;
//        lseek(fd, depsOffset, SEEK_SET);
    }
    offpos += depsOffset;

    /*
     * Append the dependency list.
     */


    if (writeDependencies(packageLocation, &DependBuf, modWhen, crc, posTemp, sdkType) != 0) {
       LOGTEST("Failed writing dependencies");
        goto bail;
    }
//    LOGTEST("dvmContinueOptimization# after writeDependencies!!!!!!!!!!!!!!!!!!");
    DependSize = posTemp;
    offpos += posTemp;
    depsLength = posTemp;


    /* compute deps length, then adjust opt start for 64-bit alignment */
//    optOffset = lseek(fd, 0, SEEK_END);
    optOffset = sizeof(DexOptHeader) + dexLength + DependSize;
    

    adjOffset = (optOffset + 7) & ~(0x07);
    if (adjOffset != optOffset) {
//        LOGV("Adjusting opt start from %d to %d",
//            (int) optOffset, (int) adjOffset);
        optOffset = adjOffset;
//        lseek(fd, optOffset, SEEK_SET);
        offpos = optOffset;
    }

    /*
     * Append any optimized pre-computed data structures.
     */
    posTemp = 0;
//TODO:
    if (!writeOptData(&OptBuf, pClassLookup, NULL, posTemp)) {
       LOGTEST("Failed writing opt data");
        goto bail;
    }
//    LOGTEST("Successfully wrote OptBuf");

    OptSize = posTemp;
    offpos += posTemp;
    endOffset = offpos;
//    endOffset = lseek(fd, 0, SEEK_END);
    optLength = OptSize;


    adjDexPos = sizeof(DexOptHeader) + dexLength;
    adjDexPos = (adjDexPos + 7) / 8 * 8;
    adjDependPos = adjDexPos + DependSize;
    adjDependPos = (adjDependPos + 7) / 8 * 8;
    adjAllPos = adjDependPos + OptSize;
    adjAllPos = (adjAllPos + 7) / 8 * 8;

//    LOGTEST("DEX header# optHdr:%d dexLength:%d DependSize:%d OptSize:%d", sizeof(DexOptHeader),dexLength, DependSize, OptSize);
    *OutBuf = (u1*)malloc(adjAllPos);
    memset(*OutBuf, 0, adjAllPos);
    // if (sysWriteFully(*OutBuf, &optHdr, sizeof(optHdr), "DexOpt opt header") != 0)
    //     goto bail;
    OutBufPos += sizeof(DexOptHeader);
//·Ö±ð½«fdBuf¡¢writeDependenciesºÍwriteOptDataÐ´ÈëOutBuf£¬OutBuf¼´ÎªÄÚ´æÖÐµÄodexÎÄ¼þ
    if (sysWriteFully(*OutBuf + OutBufPos, fdBuf, dexLength, "DexOpt opt OutPut") != 0)
        goto bail;
    OutBufPos += dexLength;
    if (sysWriteFully(*OutBuf + adjDexPos, DependBuf, DependSize, "DexOpt opt OutPut") != 0)
        goto bail;
    OutBufPos += DependSize;
    if (sysWriteFully(*OutBuf + adjDependPos, OptBuf, OptSize, "DexOpt opt OutPut") != 0)
        goto bail;

    free(DependBuf);
    free(OptBuf);

    /* compute checksum from start of deps to end of opt area */
    if (!computeFileChecksum(*OutBuf, adjDexPos,
            adjAllPos - adjDexPos ,&optChecksum))
    {
        goto bail;
    }

    /*
     * Output the "opt" header with all values filled in and a correct
     * magic number.
     */
    DexOptHeader optHdr;
    memset(&optHdr, 0xff, sizeof(optHdr));
    memcpy(optHdr.magic, DEX_OPT_MAGIC, 4);
    memcpy(optHdr.magic+4, DEX_OPT_MAGIC_VERS, 4);
    optHdr.dexOffset = (u4) dexOffset;
    optHdr.dexLength = (u4) dexLength;
    optHdr.depsOffset = (u4) depsOffset;
    optHdr.depsLength = (u4) depsLength;
    optHdr.optOffset = (u4) optOffset;
    optHdr.optLength = (u4) optLength;
#if __BYTE_ORDER != __LITTLE_ENDIAN
    optHdr.flags = DEX_OPT_FLAG_BIG;
#else
    optHdr.flags = 0;
#endif
    optHdr.checksum = optChecksum;

//    fsync(fd);      /* ensure previous writes go before header is written */

//    lseek(fd, 0, SEEK_SET);
//½«Í·²¿·ÖÐ´ÈëÊä³öµÄÄÚ´æÖÐ

    if (sysWriteFully(*OutBuf, &optHdr, sizeof(optHdr), "DexOpt opt header") != 0)
        goto bail;
    OutBufPos += OptSize;
//    LOGTEST("Successfully wrote DEX header OdexLength:%d", adjAllPos);
    *OdexLength = adjAllPos;
    result = true;

    //dvmRegisterMapDumpStats();

bail:
	/*
	dvmFreeRegisterMapBuilder_t dvmFreeRegisterMapBuilder = (dvmFreeRegisterMapBuilder_t)dlsym(ldvm,"_Z25dvmFreeRegisterMapBuilderP18RegisterMapBuilder");
  if(dvmFreeRegisterMapBuilder == NULL)
    {
    LOGTEST("find dvmFreeRegisterMapBuilder fail\n");
    return true;
     }
	*/
    dvmFreeRegisterMapBuilder(pRegMapBuilder);
    free(pClassLookup);
    return result;
}

/*
 * Get the cache file name from a ClassPathEntry.
 */
static const char* getCacheFileName(const ClassPathEntry* cpe)
{
	void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );
	typedef const char* (*dvmGetJarFileCacheFileName_t)(JarFile* );
	typedef const char* (*dvmGetRawDexFileCacheFileName_t)(RawDexFile* );
	dvmGetJarFileCacheFileName_t dvmGetJarFileCacheFileName = (dvmGetJarFileCacheFileName_t)dlsym(ldvm, "_Z26dvmGetJarFileCacheFileNameP7JarFile");
	  if(dvmGetJarFileCacheFileName == NULL)
    {
    LOGTEST("find dvmGetJarFileCacheFileName fail\n");
    return false;
     }
	dvmGetRawDexFileCacheFileName_t dvmGetRawDexFileCacheFileName = (dvmGetRawDexFileCacheFileName_t)dlsym(ldvm, "_Z29dvmGetRawDexFileCacheFileNameP10RawDexFile");
  	if(dvmGetRawDexFileCacheFileName == NULL)
    {
    LOGTEST("find dvmGetRawDexFileCacheFileName fail\n");
    return false;
     }
     
    switch (cpe->kind) {
    case kCpeJar:
        return dvmGetJarFileCacheFileName((JarFile*) cpe->ptr);
    case kCpeDex:
        return dvmGetRawDexFileCacheFileName((RawDexFile*) cpe->ptr);
    default:
        LOGTEST("DexOpt: unexpected cpe kind %d", cpe->kind);
        dvmAbort();
        return NULL;
    }
}

int getCacheDexFileName(const char *target, char * dexFilename)
{
	//LOGTEST("get cache file name in\n");


    FILE *pf;
    char *strPtr;
    char command[256+1];
    char data[256+1];
    const char *pattern = target;
    int found = 0;

    // execute a memory-mapping listing 
    sprintf(command, "/proc/%d/maps", getpid());

    // setup our pipe for reading and execute the command 
    pf = fopen(command, "r");

    if (!pf)
    {
        return 102;
    }


    while(fgets(data, sizeof(data), pf))
    {
         
        strPtr = strstr(data, pattern);
        if (strPtr)
        {
        	char delims[] = "classes.dex";
         char *temp = NULL;
         
         temp = strstr( data, delims );
         if(temp){
         	char delims2[] = "/";
         	char *cacheFileName = NULL;
         	cacheFileName = strstr( data, delims2 );
         	if(cacheFileName)
         		{
         memcpy(dexFilename, cacheFileName,strlen(cacheFileName)-1);
      break;}
    }
      
        }
    }
    //strcpy(dexFilename, data);
    //LOGTEST("cacheDexname is:%s\n",dexFilename);
    fclose(pf);
    return (found)? 0 : 104;
}

static int getTokenLength(const char *string, const char *sep)
{
    const char *strpos = strstr(string, sep);
    if (!strpos || (strpos - string) < strlen(sep))
    {
        return -1;
    }
    return (strpos - string);
}

static int splitString(const char *string, int strLen, const char *sep,
        char *stringA, char *stringB)
{
    const int sepSize = 1;
    const char *target = string;
    char *strpos;

    strpos = strstr(target, sep);
    if (!strpos) return 103;
    const int stringALen = strpos - target;
    strncpy(stringA, target, stringALen);

    target = strpos + sepSize;
    const int stringBLen = strLen - stringALen - sepSize;
    strncpy(stringB, target, stringBLen);

    return 0;
}

static int parseMemoryAddressFromMaps(const char *mapsline,
        unsigned int *startAddr, unsigned int *endAddr)
{
    /* Warning: Here we assume the code is running on a 32-bit GNU platform! */
    /* we are only able to handle the memory address string like: 43ee8000-43ef5000 */

    const char *target = mapsline;
    const char sep[] = "-";
    char stringA[9] = {0};
    char stringB[9] = {0};

    const int memAddrTokenLen = getTokenLength(target, " ");
    splitString(target, memAddrTokenLen, sep, stringA, stringB);

    *startAddr = (unsigned int)strtoul((const char *)stringA, 0, 16);
    *endAddr = (unsigned int)strtoul((const char *)stringB, 0, 16);

    return 0;
}

int getMemoryAddress(const char *target, unsigned int *startAddr, unsigned int *endAddr)
{


    FILE *pf;
    char *strPtr;
    char command[256+1];
    char data[256+1];
    const char *pattern = target;
    int found = 0;

    // execute a memory-mapping listing 
    sprintf(command, "/proc/%d/maps", getpid());

    // setup our pipe for reading and execute the command 
    pf = fopen(command, "r");

    if (!pf)
    {
        return 102;
    }

    // grab data from process execution 
    *startAddr = 0;
    *endAddr = 0;
    while(fgets(data, sizeof(data), pf))
    {
        // look for the target memory address 
        strPtr = strstr(data, pattern);
        if (strPtr)
        {
            // read the first token divided by "space" to get the start/end address
            parseMemoryAddressFromMaps(data, startAddr, endAddr);
            found = 1;
            break;
        }
    }

    fclose(pf);
    return (found)? 0 : 104;
}

/*
 * Get the SHA-1 signature.
 */
static  u1* getSignature(const char * cpe)
{
    LOGTEST("getSignature!!!!!!!!!!!!!!!!!!!!!");
    //DvmDex* pDvmDex;
    // void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );
    // _Z14dvmDefineClassP6DvmDexPKcP6Object
    // _Z19dvmGetRawDexFileDexP10RawDexFile
/*
    switch (cpe->kind) {
    case kCpeJar:
        pDvmDex = dvmGetJarFileDex((JarFile*) cpe->ptr);
        break;
    case kCpeDex:
        pDvmDex = dvmGetRawDexFileDex((RawDexFile*) cpe->ptr);
        break;
    default:
        LOGTEST("unexpected cpe kind %d", cpe->kind);
        dvmAbort();
        pDvmDex = NULL;         // make gcc happy
    }
*/
    //pDvmDex = dvmGetRawDexFileDex((RawDexFile*) cpe);
    //assert(pDvmDex != NULL);
    //return pDvmDex->pDexFile->pHeader->signature;

  //sprintf(OdexPath, "/data/data/%s/files/classes.dex", pk_name);
  int fd = open(cpe, O_RDONLY, (mode_t)0400);

  if(fd < 0)
  	{
  		LOGTEST("open file failed\n");
  	}
  char * dataodex = (char *)malloc(120);
  read(fd, dataodex,120);
  close(fd);
  u1 *sigbuf = (u1 *)malloc(kSHA1DigestLen);
  memcpy(sigbuf, dataodex+40+12, kSHA1DigestLen);
  return sigbuf;
}

/*
 * Write the dependency info to "fd" at the current file position.
 */
int writeDependencies(const char *packageLocation, u1** fdBuf, u4 modWhen, u4 crc, ssize_t &pos, int sdkType)
{
//	LOGTEST("write dependencies!!!!!!!!!!!!!!!!!!!!!");
    u1* buf = NULL;
    int result = 0;
    ssize_t bufLen;
    ClassPathEntry* cpe;
    int numDeps;
    numDeps = 0;
    bufLen = 0;

/*
//    typedef const char* (*getCacheFileName_t)(const ClassPathEntry*);
    typedef const char* (*dvmPathToAbsolutePortion_t)(const char* );
    // typedef const u1* (*getSignature_t)(const ClassPathEntry*);
    void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );

//    LOGTEST("writeDependencies# ldvm:%d", ldvm);
//    getCacheFileName_t getCacheFileName = (getCacheFileName_t)dlsym(ldvm, "getCacheFileName");
    dvmPathToAbsolutePortion_t dvmPathToAbsolutePortion = (dvmPathToAbsolutePortion_t)dlsym(ldvm, "_Z24dvmPathToAbsolutePortionPKc");
      	if(dvmPathToAbsolutePortion == NULL)
    {
    LOGTEST("find dvmPathToAbsolutePortion fail\n");
    return false;
     }
    // getSignature_t getSignature = (getSignature_t)dlsym(ldvm, "_Z18dvmDbgGetSignaturey");
//    LOGTEST("writeDependencies# getSignature:%d", getSignature);




    switch (sdkType)
    {
        case 19:
            cpe = gDvm19->bootClassPath;
            break;
        case 18:
            cpe = gDvm18->bootClassPath;
            break;
        case 17:
            cpe = gDvm17->bootClassPath;
            break;
        case 16:
            cpe = gDvm16->bootClassPath;
            break;
        case 15:
            cpe = gDvm15->bootClassPath;
            break;
        default:
            cpe = gDvm15->bootClassPath;
            break;
    }

    for ( ; cpe->ptr != NULL; cpe++) {
        const char* cacheFileName =
            dvmPathToAbsolutePortion(getCacheFileName(cpe));
        assert(cacheFileName != NULL); // guaranteed by Class.c 

        LOGTEST("+++ DexOpt: found dep '%s'", cacheFileName);

        numDeps++;
        bufLen += strlen(cacheFileName) +1;
    }

    
    // get cacheFileName from "BOOTCLASSPATH", like "/system/framework/core.jar" which should be "/data/dalvik-cache/system@framework@core.jar@classes.dex" got from gDvm->bootClassPath
       char delims[] = ":";
       char *cacheFileName = NULL;
       char* bootpath = getenv("BOOTCLASSPATH");
       char* bcp = (char *)malloc(strlen(bootpath) + 1);
       memset(bcp, 0, strlen(bootpath) + 1);
       memcpy(bcp, bootpath, strlen(bootpath));
       char* bcp2 = (char *)malloc(strlen(bootpath) + 1);
       memset(bcp2, 0, strlen(bootpath) + 1);
       memcpy(bcp2, bootpath, strlen(bootpath));
       
       cacheFileName = strtok( bcp, delims );
       while( cacheFileName != NULL ) 
       {
       LOGTEST( "result is %s\n", cacheFileName );
       
       char cacheDexNameTemp[256]= {0};
       memcpy(cacheDexNameTemp, cacheFileName + 1, strlen(cacheFileName) - 1);
       for(int i = 0; i < strlen(cacheDexNameTemp); i++)
       {
       	if(cacheDexNameTemp[i] == '/')cacheDexNameTemp[i]='@';
       	}
       	
       	char cacheDexName[256]={0};
       	
       //sprintf(cacheDexName, "/data/dalvik-cache/%s%s", cacheDexNameTemp,"@classes.dex");
       getCacheDexFileName(cacheDexNameTemp, cacheDexName);
       assert(strlen(cacheFileName) != 0); // guaranteed by Class.c
       numDeps++;
       bufLen += strlen(cacheDexName) +1;
       cacheFileName = strtok( NULL, delims );
      }
    bufLen += 4*4 + numDeps * (4+kSHA1DigestLen1);

//    LOGTEST("writeDependencies# bufLen:%d", bufLen);
    buf = (u1*)malloc(bufLen);

    set4LE(buf+0, modWhen);
    set4LE(buf+4, crc);
    set4LE(buf+8, DALVIK_VM_BUILD);
    set4LE(buf+12, numDeps);
//    LOGTEST("writeDependencies# buf:%d", buf);

    // TODO: do we want to add dvmGetInlineOpsTableLength() here?  Won't
    // help us if somebody replaces an existing entry, but it'd catch
    // additions/removals.
  
  

    u1* ptr = buf + 4*4;
    // LOGTEST("writeDependencies# before the for gDvm->bootClassPath:%d", gDvm15->bootClassPath);
    /*
    switch (sdkType)
    {
        case 19:
            cpe = gDvm19->bootClassPath;
            break;
        case 18:
            cpe = gDvm18->bootClassPath;
            break;
        case 17:
            cpe = gDvm17->bootClassPath;
            break;
        case 16:
            cpe = gDvm16->bootClassPath;
            break;
        case 15:
            cpe = gDvm15->bootClassPath;
            break;
        default:
            cpe = gDvm15->bootClassPath;
            break;
    }
    
    for ( ; cpe->ptr != NULL; cpe++) {
//        LOGTEST("writeDependencies# in the for cpe:%x", cpe);
        const char* cacheFileName =
            dvmPathToAbsolutePortion(getCacheFileName(cpe));
        assert(cacheFileName != NULL); // guaranteed by Class.c 
//        LOGTEST("writeDependencies# in the for cacheFileName:%s", cacheFileName);

        const u1* signature = getSignature(cpe);
        int len = strlen(cacheFileName) +1;

        if (ptr + 4 + len + kSHA1DigestLen1 > buf + bufLen) {
//            LOGE("DexOpt: overran buffer");
            dvmAbort();
        }
//        LOGTEST("writeDependencies# in the for len:%d ptr:%d", len, ptr);



        set4LE(ptr, len);
        ptr += 4;
//        LOGTEST("writeDependencies# in the for after set4LE ptr:%d", ptr);
        memcpy(ptr, cacheFileName, len);
        ptr += len;
//        LOGTEST("writeDependencies# in the for after cacheFileName signature:%x, kSHA1DigestLen1:%d", signature, kSHA1DigestLen1);
        memcpy(ptr, signature, kSHA1DigestLen1);
        ptr += kSHA1DigestLen1;
//        LOGTEST("writeDependencies# in the for last ptr:%d", ptr);
    }
//    LOGTEST("writeDependencies# after for ptr:%d bufLen:%d", ptr, bufLen);

       char delims2[] = ":";
       char *cacheFileName2 = NULL;
       //char* bcp2 = getenv("BOOTCLASSPATH");
       LOGTEST("boot path is %s\n",bcp2);
       cacheFileName2 = strtok( bcp2, delims2);
       while( cacheFileName2 != NULL ) 
       {
       LOGTEST( "result is %s\n", cacheFileName2 );
       char cacheDexNameTemp[256]= {0};
       memcpy(cacheDexNameTemp, cacheFileName2 + 1, strlen(cacheFileName2) - 1);
       for(int i = 0; i < strlen(cacheDexNameTemp); i++)
       {
       	if(cacheDexNameTemp[i] == '/')cacheDexNameTemp[i]='@';
       	}
       	
       	char cacheDexName[256]={0};
       	
       //sprintf(cacheDexName, "/data/dalvik-cache/%s%s", cacheDexNameTemp,"@classes.dex");
       getCacheDexFileName(cacheDexNameTemp, cacheDexName);
       assert(strlen(cacheFileName) != 0);
       LOGTEST("cache dex name is %s\n", cacheDexName);
        sprintf(cacheDexName, "/data/dalvik-cache/%s%s", cacheDexNameTemp,"@classes.dex");
        u1* signature = getSignature(cacheDexName);
        int len = strlen(cacheDexName) +1;

        if (ptr + 4 + len + kSHA1DigestLen1 > buf + bufLen) {
            LOGTEST("DexOpt: overran buffer");
            dvmAbort();
        }
       LOGTEST("get signature finished\n");

        set4LE(ptr, len);
        ptr += 4;
//        LOGTEST("writeDependencies# in the for after set4LE ptr:%d", ptr);
        memcpy(ptr, cacheDexName, len);
        ptr += len;
//        LOGTEST("writeDependencies# in the for after cacheFileName signature:%x, kSHA1DigestLen1:%d", signature, kSHA1DigestLen1);
        memcpy(ptr, signature, kSHA1DigestLen1);
        free(signature);
        ptr += kSHA1DigestLen1;
        
       cacheFileName2 = strtok( NULL, delims2);
      }
    assert(ptr == buf + bufLen);
*/

// the dependence of odex is the same on the same device, so we can get the dependence from the shell apk odex.
       char cacheDexNameTemp[256]= {0};
 //odex path is like data@app@packagename-1.apk@classes.dex ,so we should get it from packageLocation as /data/app/packagename-1.apk      
       memcpy(cacheDexNameTemp, packageLocation + 1, strlen(packageLocation) - 1);
       for(int i = 0; i < strlen(cacheDexNameTemp); i++)
       {
       	if(cacheDexNameTemp[i] == '/')cacheDexNameTemp[i]='@';
       	}
        char cacheDexName[256]={0};
        // get the full odex path name from proc/pid/maps
       getCacheDexFileName(cacheDexNameTemp, cacheDexName);
       LOGTEST("apk odex path is %s\n", cacheDexName);
       unsigned int startAddr = 0;
		  unsigned int endAddr = 0;
		  // get the dependence from odex in memory
		 if (getMemoryAddress(cacheDexName, &startAddr, &endAddr) == 0)
			{
				LOGTEST("find apk odex memory \n");
				 unsigned char* pStartAddr = NULL;
				 char var[128]={0};
		     memset(var, 0, 128);
				 sprintf(var, "%x", startAddr);
				 sscanf(var, "%p", (unsigned char **)&pStartAddr);
				 int depoffset=0;
         int deplen = 0;
         memcpy(&depoffset, pStartAddr +8 + 8, 4);
         memcpy(&deplen, pStartAddr +8 + 8 + 4, 4);
         bufLen = deplen;
         buf = (u1*)malloc(bufLen);
        *fdBuf = (u1*)malloc(bufLen);
        //set the right crc and modWhen 
         set4LE(buf+0, modWhen);
         set4LE(buf+4, crc);
         // copy the dependence from shell odex except crc and modWhen
         memcpy(buf+8, pStartAddr+depoffset+8, deplen-8);
         LOGTEST("test!!!!");
         memcpy(*fdBuf, buf, bufLen);
//    LOGTEST("writeDependencies# before out *fdBuf:%x",*fdBuf);
    // result = sysWriteFully(*fdBuf, buf, bufLen, "DexOpt dep info");
    pos += bufLen;
  

  free(buf);
  LOGTEST("bufLen is %d\n",bufLen);
  }
    return result;
}



/*
 * Write a block of data in "chunk" format.
 *
 * The chunk header fields are always in "native" byte order.  If "size"
 * is not a multiple of 8 bytes, the data area is padded out.
 */
static bool writeChunk(void* fdBuf, u4 type, const void* data, size_t size)
{
	size_t pos(0);
    union {             /* save a syscall by grouping these together */
        char raw[8];
        struct {
            u4 type;
            u4 size;
        } ts;
    } header;

    assert(sizeof(header) == 8);

//    LOGV("Writing chunk, type=%.4s size=%d", (char*) &type, size);

    header.ts.type = type;
    header.ts.size = (u4) size;
    if (sysWriteFully(fdBuf, &header, sizeof(header),
            "DexOpt opt chunk header write") != 0)
    {
        return false;
    }
    pos += sizeof(header);

    if (size > 0) {
//        LOGTEST("writeChunk# data:%x", data);
        if (sysWriteFully(fdBuf + pos, data, size, "DexOpt opt chunk write") != 0)
            return false;
    }

    /* if necessary, pad to 64-bit alignment */
    if ((size & 7) != 0) {
        int padSize = 8 - (size & 7);
//        LOGV("size was %d, inserting %d pad bytes", size, padSize);
        pos += padSize;
//        lseek(fd, padSize, SEEK_CUR);
    }

    assert( ((int)(fdBuf + pos) & 7) == 0);

    return true;
}



/*
 * Write opt data.
 *
 * We have different pieces, some of which may be optional.  To make the
 * most effective use of space, we use a "chunk" format, with a 4-byte
 * type and a 4-byte length.  We guarantee 64-bit alignment for the data,
 * so it can be used directly when the file is mapped for reading.
 */
static bool writeOptData(u1** fdBuf, const DexClassLookup* pClassLookup,
    const RegisterMapBuilder* pRegMapBuilder, ssize_t &posTemp)
{
//    LOGTEST("writeOptData!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	size_t pos(0);
    /* pre-computed class lookup hash table */
	*fdBuf = (u1*)malloc(8 + pClassLookup->size);
    if (!writeChunk(*fdBuf, (u4) kDexChunkClassLookup1,
            pClassLookup, pClassLookup->size))
    {
        return false;
    }
    pos += pClassLookup->size;
    pos += 8;

    /* register maps (optional) */
    if (pRegMapBuilder != NULL) {
    	*fdBuf = (u1*)realloc(*fdBuf, pos + 8 + pRegMapBuilder->size);
        if (!writeChunk(*fdBuf + pos, (u4) kDexChunkRegisterMaps1,
                pRegMapBuilder->data, pRegMapBuilder->size))
        {
            return false;
        }
        pos += 8;
        pos += pRegMapBuilder->size;
//        LOGTEST("writeOptData# pRegMapBuilder->size:%d", pRegMapBuilder->size);
    }

    /* write the end marker */
    *fdBuf = (u1*)realloc(*fdBuf, pos + 8);
    if (!writeChunk(*fdBuf + pos, (u4) kDexChunkEnd1, NULL, 0)) {
        return false;
    }
    pos += 8;
    pos += 0;
//    LOGTEST("writeOptData# posTemp:%d", pos);

    posTemp = pos;
    return true;
}

/*
 * Write until all bytes have been written.
 *
 * Returns 0 on success, or an errno value on failure.
 */
static int sysWriteFully(void* fdBuf, const void* buf, size_t count, const char* logMsg)
{
//    while (count != 0) {
        memcpy(fdBuf, buf, count);
//        if (actual < 0) {
//            int err = errno;
//            LOGE("%s: write failed: %s", logMsg, strerror(err));
//            return err;
//        } else if (actual != (ssize_t) count) {
//            LOGD("%s: partial write (will retry): (%d of %zd)",
//                logMsg, (int) actual, count);
//            buf = (const void*) (((const u1*) buf) + actual);
//        }
//        count -= actual;
//    }

    return 0;
}

/*
 * Compute a checksum on a piece of an open file.
 *
 * File will be positioned at end of checksummed area.
 *
 * Returns "true" on success.
 */
static bool computeFileChecksum(void* fdBuf, off_t start, size_t length, u4* pSum)
{
	ssize_t pos = start;
    unsigned char readBuf[8192];
    ssize_t actual;

    uLong adler;

//    if (lseek(fd, start, SEEK_SET) != start) {
////        LOGE("Unable to seek to start of checksum area (%ld): %s",
////            (long) start, strerror(errno));
//        return false;
//    }

//    LOGTEST("computeFileChecksum# before adler32!!!!!");
    adler = adler32(0L, Z_NULL, 0); 
//    LOGTEST("computeFileChecksum# after adler32!!!!!");

    while (length > 0) {
        size_t wanted = (length < sizeof(readBuf)) ? length : sizeof(readBuf);
        memcpy(readBuf,fdBuf+pos, wanted);
        pos += wanted;
//        actual = read(fd, readBuf, wanted);
        // if (actual <= 0) {
        //    LOGTEST("Read failed (%d) while computing checksum (len=%zu): ",
        //        (int) actual, length);
        //     return false;
        // }         

       adler = adler32(adler, readBuf, wanted);

    length -= wanted;
    }

//    LOGTEST("write checksum successfully!!!!!!!!!!!!!!");
    *pSum = adler;
    return true;
}


/*
 * Try to load all classes in the specified DEX.  If they have some sort
 * of broken dependency, e.g. their superclass lives in a different DEX
 * that wasn't previously loaded into the bootstrap class path, loading
 * will fail.  This is the desired behavior.
 *
 * We have no notion of class loader at this point, so we load all of
 * the classes with the bootstrap class loader.  It turns out this has
 * exactly the behavior we want, and has no ill side effects because we're
 * running in a separate process and anything we load here will be forgotten.
 *
 * We set the CLASS_MULTIPLE_DEFS flag here if we see multiple definitions.
 * This works because we only call here as part of optimization / pre-verify,
 * not during verification as part of loading a class into a running VM.
 *
 * This returns "false" if the world is too screwed up to do anything
 * useful at all.
 */
static bool loadAllClasses(DvmDex* pDvmDex, int sdkType)
{
	void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );
    u4 count = pDvmDex->pDexFile->pHeader->classDefsSize;
    u4 idx;
    int loaded = 0;

    typedef int (*dvmSetBootPathExtraDex_t)(DvmDex*);
    typedef bool (*dvmFindRequiredClassesAndMembers_t)(void);
    typedef bool (*dvmInitClass_t)(ClassObject*);
    typedef ClassObject* (*dvmFindSystemClassNoInit_t)(const char*);
    typedef void (*dvmClearOptException_t)(void);


//    LOGTEST("DexOpt: enter the loadAllClasses!");
//    LOGTEST("DexOpt: +++ trying to load %d classes", count);

    dvmSetBootPathExtraDex_t dvmSetBootPathExtraDex = (dvmSetBootPathExtraDex_t)dlsym(ldvm, "_Z22dvmSetBootPathExtraDexP6DvmDex");
    dvmSetBootPathExtraDex(pDvmDex);
//    LOGTEST("dvmSetBootPathExtraDex succeed!");

    /*
     * At this point, it is safe -- and necessary! -- to look up the
     * VM's required classes and members, even when what we are in the
     * process of processing is the core library that defines these
     * classes itself. (The reason it is necessary is that in the act
     * of initializing the class Class, below, the system will end up
     * referring to many of the class references that got set up by
     * this call.)
     */
    //Õâ¸öÃ²ËÆ»¹ÊÇ²»ÔÚlibdvm.soµÄÊä³öÖÐ£¬ÏÖÔÚ²»È·¶¨£¬µÈÈ«²¿¸ÄÍêºóÔÙ¿´
    // TODO: do we want to add dvmGetInlineOpsTableLength() here?  Won't
    dvmFindRequiredClassesAndMembers_t dvmFindRequiredClassesAndMembers = (dvmFindRequiredClassesAndMembers_t)dlsym(ldvm, "_Z32dvmFindRequiredClassesAndMembersv");
    if (!dvmFindRequiredClassesAndMembers()) {
        return false;
    }
    // LOGTEST("dvmFindRequiredClassesAndMembers succeed!%d", gDvm15->classJavaLangClass);

    /*
     * We have some circularity issues with Class and Object that are
     * most easily avoided by ensuring that Object is never the first
     * thing we try to find-and-initialize. The call to
     * dvmFindSystemClass() here takes care of that situation. (We
     * only need to do this when loading classes from the DEX file
     * that contains Object, and only when Object comes first in the
     * list, but it costs very little to do it in all cases.)
     */
    dvmInitClass_t dvmInitClass = (dvmInitClass_t)dlsym(ldvm, "dvmInitClass");
//    LOGTEST("dvmFindRequiredClassesAndMembers succeed!%d", dvmInitClass);
    bool tempInit;

    switch (sdkType)
    {
        case 19:
            tempInit = gDvm19->classJavaLangClass;
            break;
        case 18:
            tempInit = gDvm18->classJavaLangClass;
            break;
        case 17:
            tempInit = gDvm17->classJavaLangClass;
            break;
        case 16:
            tempInit = gDvm16->classJavaLangClass;
            break;
        case 15:
            tempInit = gDvm15->classJavaLangClass;
            break;
        default:
            tempInit = gDvm15->classJavaLangClass;
            break;
    }

    if (!tempInit) {
        LOGTEST("ERROR: failed to initialize the class Class!");
        return false;
    }
//    LOGTEST("dvmInitClass succeed!");

    for (idx = 0; idx < count; idx++) {
        const DexClassDef* pClassDef;
        const char* classDescriptor;
        ClassObject* newClass;

        pClassDef = dexGetClassDef(pDvmDex->pDexFile, idx);
        classDescriptor =
            dexStringByTypeIdx(pDvmDex->pDexFile, pClassDef->classIdx);

//        LOGTEST("+++  loading '%s'", classDescriptor);
        //newClass = dvmDefineClass(pDexFile, classDescriptor,
        //        NULL);
        dvmFindSystemClassNoInit_t dvmFindSystemClassNoInit = (dvmFindSystemClassNoInit_t)dlsym(ldvm, "_Z24dvmFindSystemClassNoInitPKc");
        newClass = dvmFindSystemClassNoInit(classDescriptor);
        if (newClass == NULL) {
//            LOGTEST("DexOpt: failed loading '%s'", classDescriptor);
//            TODO£º£º²»ÖªµÀÊÇÀ´×öÊ²Ã´µÄ£¬µ«ÒòÎªÀàÐÍÉæ¼°Thread£¬ÓÖÐèÒªÒýÈëÆäËûº¯ÊýºÍÀà£¬¹ýÓÚÂé·³£¬ÔÝÇÒºöÂÔ
//            dvmClearOptException(dvmThreadSelf());
        } else if (newClass->pDvmDex != pDvmDex) {
            /*
             * We don't load the new one, and we tag the first one found
             * with the "multiple def" flag so the resolver doesn't try
             * to make it available.
             */
//            LOGTEST("DexOpt: '%s' has an earlier definition; blocking out",
//                classDescriptor);
            SET_CLASS_FLAG(newClass, CLASS_MULTIPLE_DEFS);
        } else {
            loaded++;
        }
    }
//    LOGTEST("DexOpt: +++ successfully loaded %d classes", loaded);


    dvmSetBootPathExtraDex(NULL);
    return true;
}

/*
 * Perform in-place rewrites on a memory-mapped DEX file.
 *
 * If this is called from a short-lived child process (dexopt), we can
 * go nutty with loading classes and allocating memory.  When it's
 * called to prepare classes provided in a byte array, we may want to
 * be more conservative.
 *
 * If "ppClassLookup" is non-NULL, a pointer to a newly-allocated
 * DexClassLookup will be returned on success.
 *
 * If "ppDvmDex" is non-NULL, a newly-allocated DvmDex struct will be
 * returned on success.
 */
static bool rewriteDex(u1* addr, int len, bool doVerify, bool doOpt,
    DexClassLookup** ppClassLookup, DvmDex** ppDvmDex, int sdkType)
{
	void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );
    DexClassLookup* pClassLookup = NULL;
    u8 prepWhen, loadWhen, verifyOptWhen;
    DvmDex* pDvmDex = NULL;
    bool result = false;
    const char* msgStr = "???";


    typedef int (*dexSwapAndVerify_t)(u1*, int);
    //typedef int (*dvmDexFileOpenPartial_t)(u1*, int, DvmDex**);
    //typedef DexClassLookup* (*dexCreateClassLookup_t)(DexFile*);
    typedef u8 (*dvmGetRelativeTimeUsec_t)(void);
    typedef bool (*dvmCreateInlineSubsTable_t)(void);
    typedef void (*dvmDexFileFree_t)(DvmDex*);

/*
    dvmDexFileOpenPartial_t dvmDexFileOpenPartial = (dvmDexFileOpenPartial_t)dlsym(ldvm, "_Z21dvmDexFileOpenPartialPKviPP6DvmDex");
    if(dvmDexFileOpenPartial == NULL)
    {
    LOGTEST("find dvmDexFileOpenPartial fail\n");
    result = true;
    goto bail;
     }
     
    dexCreateClassLookup_t dexCreateClassLookup = (dexCreateClassLookup_t)dlsym(ldvm, "_Z20dexCreateClassLookupP7DexFile");
    if(dexCreateClassLookup == NULL)
    {
    LOGTEST("find dexCreateClassLookup fail\n");
    result = true;
    goto bail;
     }
*/


    /* if the DEX is in the wrong byte order, swap it now */
//    LOGTEST("enter the rewrite, do before dexSwapAndVerify ");

    dexSwapAndVerify_t dexSwapAndVerify = (dexSwapAndVerify_t)dlsym(ldvm, "_Z16dexSwapAndVerifyPhi");
//    LOGTEST("enter the rewrite, do after dexSwapAndVerify ldvm:%d dexSwapAndVerify:%d", ldvm, dexSwapAndVerify);

    	dvmGetRelativeTimeUsec_t dvmGetRelativeTimeUsec = (dvmGetRelativeTimeUsec_t)dlsym(ldvm, "dvmGetRelativeTimeUsec");
    	dvmCreateInlineSubsTable_t dvmCreateInlineSubsTable = (dvmCreateInlineSubsTable_t)dlsym(ldvm, "_Z24dvmCreateInlineSubsTablev");
//    LOGTEST("enter the rewrite, before dexSwapAndVerify addr: %ld len:%d", addr, len);
/*
    if (dexSwapAndVerify(addr, len) != 0)
       goto bail;
*/
//    LOGTEST("dexSwapAndVerify succeed!!!!!!!!");
    /*
     * Now that the DEX file can be read directly, create a DexFile struct
     * for it.
     */

    if (dvmDexFileOpenPartial(addr, len, &pDvmDex) != 0) {
//        LOGTEST("Unable to create DexFile");
        goto bail;
    }
//    LOGTEST("dvmDexFileOpenPartial succeed!!!!!!!!");

    /*
     * Create the class lookup table.  This will eventually be appended
     * to the end of the .odex.
     *
     * We create a temporary link from the DexFile for the benefit of
     * class loading, below.
     */
//    LOGTEST("pClassLookup!!!!!!!!pDvmDex:%d, dexCreateClassLookup:%d", pDvmDex,dexCreateClassLookup);
    pClassLookup = dexCreateClassLookup(pDvmDex->pDexFile);
    if (pClassLookup == NULL)
        goto bail;
//    LOGTEST("dexCreateClassLookup succeed!!!!!!!!");
    pDvmDex->pDexFile->pClassLookup = pClassLookup;

    /*
     * If we're not going to attempt to verify or optimize the classes,
     * there's no value in loading them, so bail out early.
     */
    if (!doVerify && !doOpt) {
        result = true;
        goto bail;
    }

//    LOGTEST("doVerify doOpt succeed!!!!!!!!");

//    prepWhen = dvmGetRelativeTimeUsec();

//    LOGTEST("dvmGetRelativeTimeUsec succeed!!!!!!!!prepWhen:%s",prepWhen);
    /*
     * Load all classes found in this DEX file.  If they fail to load for
     * some reason, they won't get verified (which is as it should be).
     */
    if (!loadAllClasses(pDvmDex, sdkType))
        goto bail;
//    LOGTEST("loadAllClasses dvmGetRelativeTimeUsec succeed!!!!!!!!");
//    loadWhen = dvmGetRelativeTimeUsec();

    /*
     * Create a data structure for use by the bytecode optimizer.
     * We need to look up methods in a few classes, so this may cause
     * a bit of class loading.  We usually do this during VM init, but
     * for dexopt on core.jar the order of operations gets a bit tricky,
     * so we defer it to here.
     */

//    LOGTEST("RewriteDex: before dvmCreateInlineSubsTable!!!!!!!! dvmCreateInlineSubsTable: %d", dvmCreateInlineSubsTable);
    
    /*
    if(dvmCreateInlineSubsTable == NULL)
    {
    LOGTEST("find dvmCreateInlineSubsTable fail\n");
    return false;
     }
     */
    if (!dvmCreateInlineSubsTable())
        goto bail;

    /*
     * Verify and optimize all classes in the DEX file (command-line
     * options permitting).
     *
     * This is best-effort, so there's really no way for dexopt to
     * fail at this point.
     */
//    LOGTEST("RewriteDex: before verifyAndOptimizeClasses!!!!!!!! verifyAndOptimizeClasses: %d,pDvmDex:%d", verifyAndOptimizeClasses,pDvmDex);
    verifyAndOptimizeClasses(pDvmDex->pDexFile, doVerify, doOpt);
//    LOGTEST("RewriteDex: before dvmGetRelativeTimeUsec!!!!!!!! dvmGetRelativeTimeUsec: %d", dvmGetRelativeTimeUsec);
    
    /*
    if(dvmGetRelativeTimeUsec == NULL)
    {
    LOGTEST("find dvmGetRelativeTimeUsec fail\n");
    return false;
     }
     */
    verifyOptWhen = dvmGetRelativeTimeUsec();

    if (doVerify && doOpt)
        msgStr = "verify+opt";
    else if (doVerify)
        msgStr = "verify";
    else if (doOpt)
        msgStr = "opt";
    LOGTEST("DexOpt: load %dms, %s %dms",
        (int) (loadWhen - prepWhen) / 1000,
        msgStr,
        (int) (verifyOptWhen - loadWhen) / 1000);

    result = true;

bail:
    /*
     * On success, return the pieces that the caller asked for.
     */

    if (pDvmDex != NULL) {
        /* break link between the two */
        pDvmDex->pDexFile->pClassLookup = NULL;
    }

    if (ppDvmDex == NULL || !result) {
    	/*
    	dvmDexFileFree_t dvmDexFileFree = (dvmDexFileFree_t)dlsym(ldvm, "_Z14dvmDexFileFreeP6DvmDex");
    if(dvmDexFileFree == NULL)
    {
    LOGTEST("find dvmDexFileFree fail\n");
    return true;
     }
     */
        dvmDexFileFree(pDvmDex);
    } else {
        *ppDvmDex = pDvmDex;
    }

    if (ppClassLookup == NULL || !result) {
        free(pClassLookup);
    } else {
        *ppClassLookup = pClassLookup;
    }

    return result;
}


/*
 * Verify and/or optimize all classes that were successfully loaded from
 * this DEX file.
 */
static void verifyAndOptimizeClasses(DexFile* pDexFile, bool doVerify,
    bool doOpt)
{
	void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );
    u4 count = pDexFile->pHeader->classDefsSize;
    u4 idx;
    typedef ClassObject* (*dvmLookupClass_t)(const char*, Object* , bool);
    dvmLookupClass_t dvmLookupClass = (dvmLookupClass_t)dlsym(ldvm, "_Z14dvmLookupClassPKcP6Objectb");
    if(dvmLookupClass == NULL)
    {
    LOGTEST("find dvmLookupClass fail\n");
    return ;
     }

//    LOGTEST("~~~~~~~~~~~~~~~~~~~~verifyAndOptimizeClasses~~~~~~~~~~~~~~~~~~~");
    for (idx = 0; idx < count; idx++) {
        const DexClassDef* pClassDef;
        const char* classDescriptor;
        ClassObject* clazz;

//        LOGTEST("verifyAndOptimizeClasses: inline dexStringByTypeIdx");
        pClassDef = dexGetClassDef(pDexFile, idx);
        classDescriptor = dexStringByTypeIdx(pDexFile, pClassDef->classIdx);//ÎªÁË»ñÈ¡Õâ¸öinlineº¯Êý£¬Òª°üº¬5¸öÆäËûµÄinlineº¯Êý£¡£¡£¡£¡£¡

        /* all classes are loaded into the bootstrap class loader */
//        LOGTEST("verifyAndOptimizeClasses: dvmLookupClass:%d", dvmLookupClass);
        clazz = dvmLookupClass(classDescriptor, NULL, false);
        if (clazz != NULL) {

            verifyAndOptimizeClass(pDexFile, clazz, pClassDef, doVerify, doOpt);

        } else {
            // TODO: log when in verbose mode
            LOGTEST("DexOpt: not optimizing unavailable class '%s'",
                classDescriptor);
        }
    }

#ifdef VERIFIER_STATS
/*
    LOGTEST("Verifier stats:");
    LOGTEST(" methods examined        : %u", gDvm15.verifierStats.methodsExamined);
    LOGTEST(" monitor-enter methods   : %u", gDvm15.verifierStats.monEnterMethods);
    LOGTEST(" instructions examined   : %u", gDvm15.verifierStats.instrsExamined);
    LOGTEST(" instructions re-examined: %u", gDvm15.verifierStats.instrsReexamined);
    LOGTEST(" copying of register sets: %u", gDvm15.verifierStats.copyRegCount);
    LOGTEST(" merging of register sets: %u", gDvm15.verifierStats.mergeRegCount);
    LOGTEST(" ...that caused changes  : %u", gDvm15.verifierStats.mergeRegChanged);
    LOGTEST(" uninit searches         : %u", gDvm15.verifierStats.uninitSearches);
    LOGTEST(" max memory required     : %u", gDvm15.verifierStats.biggestAlloc);
    */
#endif
}

/*
 * Verify and/or optimize a specific class.
 */
static void verifyAndOptimizeClass(DexFile* pDexFile, ClassObject* clazz,
    const DexClassDef* pClassDef, bool doVerify, bool doOpt)
{
	void* ldvm = (void*)dlopen("libdvm.so", RTLD_LAZY );
    const char* classDescriptor;
    bool verified = false;
    typedef bool (*dvmVerifyClass_t)(ClassObject*);
    typedef void (*dvmOptimizeClass_t)(ClassObject*, bool);

//    LOGTEST("~~~~~~~~~~~~~Enter verifyAndOptimizeClass~~~~~~~~~~~~~~~~~~~");
//    LOGTEST("verifyAndOptimizeClass : clazz: %d",clazz);
//    LOGTEST("verifyAndOptimizeClass : clazz->pDvmDex: %d",clazz->pDvmDex);
//    LOGTEST("verifyAndOptimizeClass : clazz->pDvmDex->pDexFile: %d",clazz->pDvmDex->pDexFile);
    if (clazz->pDvmDex->pDexFile != pDexFile) {
        /*
         * The current DEX file defined a class that is also present in the
         * bootstrap class path.  The class loader favored the bootstrap
         * version, which means that we have a pointer to a class that is
         * (a) not the one we want to examine, and (b) mapped read-only,
         * so we will seg fault if we try to rewrite instructions inside it.
         */
//        LOGTEST("DexOpt: not verifying/optimizing '%s': multiple definitions",
//            clazz->descriptor);
        return;
    }

//    LOGTEST("verifyAndOptimizeClass: dexStringByTypeIdx:%d", dexStringByTypeIdx);
    classDescriptor = dexStringByTypeIdx(pDexFile, pClassDef->classIdx);

    /*
     * First, try to verify it.
     */
    if (doVerify) {
    	dvmVerifyClass_t dvmVerifyClass = (dvmVerifyClass_t)dlsym(ldvm, "_Z14dvmVerifyClassP11ClassObject");
        if (dvmVerifyClass(clazz)) {
            /*
             * Set the "is preverified" flag in the DexClassDef.  We
             * do it here, rather than in the ClassObject structure,
             * because the DexClassDef is part of the odex file.
             */
            assert((clazz->accessFlags & JAVA_FLAGS_MASK) ==
                pClassDef->accessFlags);
            ((DexClassDef*)pClassDef)->accessFlags |= CLASS_ISPREVERIFIED;
            verified = true;
        } else {
            // TODO: log when in verbose mode
            LOGTEST("DexOpt: '%s' failed verification", classDescriptor);
        }
    }

    if (doOpt) {
        bool needVerify = (gDvm15->dexOptMode == OPTIMIZE_MODE_VERIFIED ||
                           gDvm15->dexOptMode == OPTIMIZE_MODE_FULL);
        if (!verified && needVerify) {
            LOGTEST("DexOpt: not optimizing '%s': not verified",
                classDescriptor);
        } else {
        	dvmOptimizeClass_t dvmOptimizeClass = (dvmOptimizeClass_t)dlsym(ldvm, "_Z16dvmOptimizeClassP11ClassObjectb");
            dvmOptimizeClass(clazz, false);

            /* set the flag whether or not we actually changed anything */
            ((DexClassDef*)pClassDef)->accessFlags |= CLASS_ISOPTIMIZED;
        }
    }
}

/*
 * Update the Adler-32 checksum stored in the DEX file.  This covers the
 * swapped and optimized DEX data, but does not include the opt header
 * or optimized data.
 */
static void updateChecksum(u1* addr, int len, DexHeader* pHeader)
{
    /*
     * Rewrite the checksum.  We leave the SHA-1 signature alone.
     */
    uLong adler = adler32(0L, Z_NULL, 0);
    const int nonSum = sizeof(pHeader->magic) + sizeof(pHeader->checksum);

    adler = adler32(adler, addr + nonSum, len - nonSum);
    pHeader->checksum = adler;
}

