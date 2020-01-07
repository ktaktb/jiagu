#ifndef DEX_LOAD_H_
#define DEX_LOAD_H_

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include<vector>
#include<string>
#include <iostream>
#include <sys/socket.h>
extern "C"
{
#include "./seclib/aes.h"
#include "./seclib/sha1.h"
#include "./seclib/amac.h"
#include "./seclib/aes_mod.h"
#include "./seclib/AES_wb_mod.h"
#include "./javaadb/jdebug.h"
#include "./nativeadb/ndebug.h"
#include "./dexdec/dexdec.h"
#include "./fileint/fileint.h"

extern void protect_ndebug(const char *pkname);
extern void protect_jdebug();
extern void analysis_bin_file(const char *binfilename, BIN_FILE_TYPE *binfiletype);
extern void AES_Encrypt_128_CBC(const unsigned char *userk,const unsigned char *iv,const unsigned char *p,unsigned char *c,int length);
extern void AES_Decrypt_128_CBC(const unsigned char *userk,const unsigned char *iv,const unsigned char *c,unsigned char *p,int length);
extern void file_integrity(const char *binfilename,const unsigned char *addr,int length);
extern void amac_init(amac_ctx* amac,const unsigned char* key,const unsigned char* key_last);
extern int amac_calc(amac_ctx* amac,const unsigned char *data,unsigned long length);
extern void amac_result(amac_ctx* amac,unsigned char *result);
extern void AES_wb_Decrypt_ecb(unsigned char *M,unsigned int *table,const unsigned char *c,unsigned char *p,int length);
}



#if defined(_TEST_DEBUG_ANDROID)
#define LOG_TAG "JNI_info"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#else
#define LOGI(...)
#endif

using namespace std;

struct ClassObject;
struct Method;
struct HashTable;
struct InstField;
struct StringObject; 

typedef uint8_t             u1;
typedef uint16_t            u2;
typedef uint32_t            u4;
typedef uint64_t            u8;
typedef int8_t              s1;
typedef int16_t             s2;
typedef int32_t             s4;
typedef int64_t             s8;

typedef struct{
u4 lock;
}Object;

typedef struct{
u4              length;
u8              contents[1];
}ArrayObject;

union JValue{
u1      z;
s1      b;
u2      c;
s2      s;
s4      i;
s8      j;
float   f;
double  d;
Object* l;
};

typedef char boolean;

typedef struct _DexClassDef
{
    u4  classIdx;           /* index into typeIds for this class */
    u4  accessFlags;
    u4  superclassIdx;      /* index into typeIds for superclass */
    u4  interfacesOff;      /* file offset to DexTypeList */
    u4  sourceFileIdx;      /* index into stringIds for source file name */
    u4  annotationsOff;     /* file offset to annotations_directory_item */
    u4  classDataOff;       /* file offset to class_data_item */
    u4  staticValuesOff;    /* file offset to DexEncodedArray */

}DexClassDef;

typedef struct _DexClassLookup {
    int     size;                       // total size, including "size"
    int     numEntries;                 // size of table[]; always power of 2
            struct {
                u4      classDescriptorHash;    // class descriptor hash code
                int     classDescriptorOffset;  // in bytes, from start of DEX
                int     classDefOffset;         // in bytes, from start of DEX
         } table[1];
} DexClassLookup;

typedef struct _DexTypeId {
     u4  descriptorIdx;      /* index into stringIds list for type descriptor */
}DexTypeId;

typedef struct _DexStringId {
     u4 stringDataOff;      /* file offset to string_data_item */
}DexStringId;

typedef struct MemMapping
{
	char* addr; /* start of data */
	int length; /* length of data */

	char* baseAddr; /* page-aligned base address */
	int baseLength; /* length of mapping */
} MemMapping;

enum PrimitiveType {
    PRIM_NOT        = 0,       /* value is a reference type, not a primitive type */
    PRIM_VOID       = 1,
    PRIM_BOOLEAN    = 2,
    PRIM_BYTE       = 3,
    PRIM_SHORT      = 4,
    PRIM_CHAR       = 5,
    PRIM_INT        = 6,
    PRIM_LONG       = 7,
    PRIM_FLOAT      = 8,
    PRIM_DOUBLE     = 9,
};

typedef void (*DalvikBridgeFunc)(const u4* args, JValue* pResult,
    const Method* method, struct Thread* self);
/* auxillary data section chunk codes */
enum {
    kDexChunkClassLookup            = 0x434c4b50,   /* CLKP */
    kDexChunkRegisterMaps           = 0x524d4150,   /* RMAP */

    kDexChunkEnd                    = 0x41454e44,   /* AEND */
};

enum { kSHA1DigestLen = 20, kSHA1DigestOutputLen = kSHA1DigestLen*2 +1 };


typedef struct _DexOptHeader {
    char  magic[8];           /* includes version number */

    int  dexOffset;          /* file offset of DEX header */
    int  dexLength;
    int  depsOffset;         /* offset of optimized DEX dependency table */
    int  depsLength;
    int  optOffset;          /* file offset of optimized data tables */
    int  optLength;

    int  flags;              /* some info flags */
    int  checksum;           /* adler32 checksum covering deps/opt */

    /* pad for 64-bit alignment if necessary */
} DexOptHeader;

#define DEX_INDEX_MAP_DECLARATION

typedef struct _DexHeader {
    u1  magic[8];           /* includes version number */
    u4  checksum;           /* adler32 checksum */
    u1  signature[kSHA1DigestLen]; /* SHA-1 hash */
    u4  fileSize;           /* length of entire file */
    u4  headerSize;         /* offset to start of next section */
    u4  endianTag;
    u4  linkSize;
    u4  linkOff;
    u4  mapOff;
    u4  stringIdsSize;
    u4  stringIdsOff;
    u4  typeIdsSize;
    u4  typeIdsOff;
    u4  protoIdsSize;
    u4  protoIdsOff;
    u4  fieldIdsSize;
    u4  fieldIdsOff;
    u4  methodIdsSize;
    u4  methodIdsOff;
    u4  classDefsSize;
    u4  classDefsOff;
    u4  dataSize;
    u4  dataOff;
} DexHeader;

typedef struct DexFile
{
	/* directly-mapped "opt" header */
	//const DexOptHeader* pOptHeader;
	DexOptHeader* pOptHeader;

	/* pointers to directly-mapped structs and arrays in base DEX */
	const DexHeader* pHeader;
	//const void* pHeader;
	const DexStringId* pStringIds;
	//const void* pStringIds;
	const DexTypeId* pTypeIds;
	//const void* pTypeIds;
	//const DexFieldId* pFieldIds;
	const void* pFieldIds;
	//const DexMethodId* pMethodIds;
	const void* pMethodIds;
	//const DexProtoId* pProtoIds;
	const void* pProtoIds;

	const DexClassDef* pClassDefs;
	//const void* pClassDefs;
	//const DexLink* pLinkData;
	const void* pLinkData;

	/* mapped in "auxillary" section */
	//const DexClassLookup* pClassLookup;
	const void* pClassLookup;

	/* mapped in "auxillary" section */
	DEX_INDEX_MAP_DECLARATION// this one will be empty on API level >= 9
	const void* pRegisterMapPool;

	/* points to start of DEX file data */
	const char* baseAddr;

	/* track memory overhead for auxillary structures */
	int         overhead;

/* additional app-specific data structures associated with the DEX */
//void*               auxData;
} DexFile;




struct DvmDex {
  /* pointer to the DexFile we're associated with */
    DexFile*            pDexFile;

    /* clone of pDexFile->pHeader (it's used frequently enough) */
    const DexHeader*    pHeader;

    /* interned strings; parallel to "stringIds" */
    struct StringObject** pResStrings;

    /* resolved classes; parallel to "typeIds" */
    struct ClassObject** pResClasses;

    /* resolved methods; parallel to "methodIds" */
   struct Method**     pResMethods;

    /* resolved instance fields; parallel to "fieldIds" */
    /* (this holds both InstField and StaticField) */
    struct Field**      pResFields;

    /* interface method lookup cache */
    struct AtomicCache* pInterfaceCache;

    /* shared memory region with file contents */
    bool                isMappedReadOnly;
   MemMapping          memMap;

    jobject dex_object;

    /* lock ensuring mutual exclusion during updates */
   pthread_mutex_t     modLock;
};


typedef struct RawDexFile {
	char*       cacheFileName;
	DvmDex*     pDvmDex;
} RawDexFile;

typedef struct ZipArchive
{
        /* open Zip archive */
        int mFd;

        long mDirectoryOffset;

        /* mapped file */
        MemMapping mDirectoryMap;

        /* number of entries in the Zip archive */
        int mNumEntries;

        /*
         * We know how many entries are in the Zip archive, so we can have a
         * fixed-size hash table.  We probe on collisions.
         */
        int mHashTableSize;
        //ZipHashEntry* mHashTable;
        void* mHashTable;
} ZipArchive;

typedef struct JarFile
{
	ZipArchive archive;
	//MemMapping  map;
	char* cacheFileName;
	DvmDex* pDvmDex;
} JarFile;

typedef struct DexOrJar
{
	 char*       fileName;
	 boolean     isDex;
	 boolean     okayToFree;
	 char        dummy1,dummy2;
	 //RawDexFile* pRawDexFile;
	 void*       pRawDexFile;
	 JarFile*    pJarFile;
	 //void*       pJarFile;
	 //DEX_DEXMEMORY_POINTER
	 unsigned char* pDexMemory;
} DexOrJar;

struct StringObject : Object {
   /* variable #of u4 slots; u8 uses 2 slots */
   u4              instanceData[1];

    /** Returns this string's length in characters. */
   int length() const;

    /**
    * Returns this string's length in bytes when encoded as modified UTF-8.
     * Does not include a terminating NUL byte.
     */
    int utfLength() const;

    /** Returns this string's char[] as an ArrayObject. */
    ArrayObject* array() const;

   /** Returns this string's char[] as a u2*. */
    const u2* chars() const;
};

struct InitiatingLoaderList {
   /* a list of initiating loader Objects; grown and initialized on demand */
    Object**  initiatingLoaders;
   /* count of loaders in the above list */
    int       initiatingLoaderCount;
};

struct InterfaceEntry {
 /* pointer to interface class */
   ClassObject*    clazz;
   /*
     * Index into array of vtable offsets.  This points into the ifviPool,
     * which holds the vtables for all interfaces declared by this class.
     */
    int*            methodIndexArray;
};

struct Field {
   ClassObject*    clazz;          /* class in which the field is declared */
    const char*     name;
    const char*     signature;      /* e.g. "I", "[C", "Landroid/os/Debug;" */
    u4              accessFlags;
};

struct RegisterMap {
    /* header */
    u1      format;         /* enum RegisterMapFormat; MUST be first entry */
   u1      regWidth;       /* bytes per register line, 1+ */
    u1      numEntries[2];  /* number of entries */

    /* raw data starts here; need not be aligned */
    u1      data[1];
};

struct RegisterMapMethodPool {
    u2      methodCount;            /* chiefly used as a sanity check */

    /* stream of per-method data starts here */
    u4      methodData[1];
};


struct DexProto {
    const DexFile* dexFile;     /* file the idx refers to */
    u4 protoIdx;                /* index into proto_ids table of dexFile */
};

enum ClassStatus {
    CLASS_ERROR         = -1,

    CLASS_NOTREADY      = 0,
   CLASS_IDX           = 1,    /* loaded, DEX idx in super or ifaces */
    CLASS_LOADED        = 2,    /* DEX idx values resolved */
    CLASS_RESOLVED      = 3,    /* part of linking */
    CLASS_VERIFYING     = 4,    /* in the process of being verified */
    CLASS_VERIFIED      = 5,    /* logically part of linking; done pre-init */
    CLASS_INITIALIZING  = 6,    /* class init in progress */
    CLASS_INITIALIZED   = 7,    /* ready to go */
};

enum JdwpTransportType {
    kJdwpTransportUnknown = 0,
    kJdwpTransportSocket,       /* transport=dt_socket */
    kJdwpTransportAndroidAdb,   /* transport=dt_android_adb */
};

struct JdwpStartupParams {
    JdwpTransportType transport;
    bool        server;
    bool        suspend;
    char        host[64];
    short       port;
    /* more will be here someday */
};

enum DexOptimizerMode {
    OPTIMIZE_MODE_UNKNOWN = 0,
    OPTIMIZE_MODE_NONE,         /* never optimize (except "essential") */
    OPTIMIZE_MODE_VERIFIED,     /* only optimize verified classes (default) */
    OPTIMIZE_MODE_ALL,          /* optimize verified & unverified (risky) */
    OPTIMIZE_MODE_FULL          /* fully opt verified classes at load time */
};

enum DexClassVerifyMode {
    VERIFY_MODE_UNKNOWN = 0,
    VERIFY_MODE_NONE,
    VERIFY_MODE_REMOTE,
    VERIFY_MODE_ALL
};

enum ExecutionMode {
    kExecutionModeUnknown = 0,
    kExecutionModeInterpPortable,
    kExecutionModeInterpFast,
#if defined(WITH_JIT)
    kExecutionModeJit,
#endif
#if defined(WITH_JIT)  /* IA only */
    kExecutionModeNcgO0,
    kExecutionModeNcgO1,
#endif
};

enum ClassPathEntryKind {
    kCpeUnknown = 0,
    kCpeJar,
    kCpeDex,
    kCpeLastEntry       /* used as sentinel at end of array */
};

struct ClassPathEntry {
    ClassPathEntryKind kind;
    char*   fileName;
    void*   ptr;            /* JarFile* or DexFile* */
};

struct HashEntry {
   u4 hashValue;
    void* data;
};
typedef void (*HashFreeFunc)(void* ptr);

struct HashTable {
   int         tableSize;          /* must be power of 2 */
    int         numEntries;         /* current #of "live" entries */
    int         numDeadEntries;     /* current #of tombstone entries */
    HashEntry*  pEntries;           /* array on heap */
    HashFreeFunc freeFunc;
    pthread_mutex_t lock;
};

struct BitVector {
    bool    expandable;     /* expand bitmap if we run out? */
    u4      storageSize;    /* current size, in 32-bit words */
    u4*     storage;
};

typedef void* IndirectRef;
enum IndirectRefKind {
    kIndirectKindInvalid    = 0,
    kIndirectKindLocal      = 1,
    kIndirectKindGlobal     = 2,
    kIndirectKindWeakGlobal = 3
};

union IRTSegmentState {
    u4          all;
    struct {
        u4      topIndex:16;            /* index of first unused entry */
        u4      numHoles:16;            /* #of holes in entire table */
    } parts;
};

struct IndirectRefSlot {
   Object* obj;        /* object pointer itself, NULL if the slot is unused */
    u4      serial;     /* slot serial number */
};

#define kClearedJniWeakGlobal reinterpret_cast<Object*>(0xdead1234)

class iref_iterator {
public:
    explicit iref_iterator(IndirectRefSlot* table, size_t i, size_t capacity) :
            table_(table), i_(i), capacity_(capacity) {
        skipNullsAndTombstones();
    }

   iref_iterator& operator++() {
        ++i_;
        skipNullsAndTombstones();
        return *this;
    }

    Object** operator*() {
        return &table_[i_].obj;
    }

    bool equals(const iref_iterator& rhs) const {
        return (i_ == rhs.i_ && table_ == rhs.table_);
    }

private:
    void skipNullsAndTombstones() {
       // We skip NULLs and tombstones. Clients don't want to see implementation details.
        while (i_ < capacity_ && (table_[i_].obj == NULL
               || table_[i_].obj == kClearedJniWeakGlobal)) {
            ++i_;
        }
    }

    IndirectRefSlot* table_;
    size_t i_;
   size_t capacity_;
};



struct IndirectRefTable {
public:
    typedef iref_iterator iterator;

    /* semi-public - read/write by interpreter in native call handler */
    IRTSegmentState segmentState;

    /*
249     * private:
250     *
251     * TODO: we can't make these private as long as the interpreter
252     * uses offsetof, since private member data makes us non-POD.
253     */
    /* bottom of the stack */
    IndirectRefSlot* table_;
    /* bit mask, ORed into all irefs */
    IndirectRefKind kind_;
    /* #of entries we have space for */
    size_t          alloc_entries_;
    /* max #of entries allowed */
    size_t          max_entries_;

    // TODO: want hole-filling stats (#of holes filled, total entries scanned)
    //       for performance evaluation.

    /*
267     * Add a new entry.  "obj" must be a valid non-NULL object reference
268     * (though it's okay if it's not fully-formed, e.g. the result from
269     * dvmMalloc doesn't have obj->clazz set).
270     *
271     * Returns NULL if the table is full (max entries reached, or alloc
272     * failed during expansion).
273     */
    IndirectRef add(u4 cookie, Object* obj);

    /*
277     * Given an IndirectRef in the table, return the Object it refers to.
278     *
279     * Returns kInvalidIndirectRefObject if iref is invalid.
280     */
    Object* get(IndirectRef iref) const;

    /*
284     * Returns true if the table contains a reference to this object.
285     */
    bool contains(const Object* obj) const;

    /*
289     * Remove an existing entry.
290     *
291     * If the entry is not between the current top index and the bottom index
292     * specified by the cookie, we don't remove anything.  This is the behavior
293     * required by JNI's DeleteLocalRef function.
294     *
295     * Returns "false" if nothing was removed.
296     */
    bool remove(u4 cookie, IndirectRef iref);

    /*
300     * Initialize an IndirectRefTable.
301     *
302     * If "initialCount" != "maxCount", the table will expand as required.
303     *
304     * "kind" should be Local or Global.  The Global table may also hold
305     * WeakGlobal refs.
306     *
307     * Returns "false" if table allocation fails.
308     */
    bool init(size_t initialCount, size_t maxCount, IndirectRefKind kind);

   /*
312     * Clear out the contents, freeing allocated storage.
313     *
314     * You must call dvmInitReferenceTable() before you can re-use this table.
315     *
316     * TODO: this should be a destructor.
317     */
    void destroy();

    /*
321     * Dump the contents of a reference table to the log file.
322     *
323     * The caller should lock any external sync before calling.
324     *
325     * TODO: we should name the table in a constructor and remove
326     * the argument here.
327     */
    void dump(const char* descr) const;

    /*
331     * Return the #of entries in the entire table.  This includes holes, and
332     * so may be larger than the actual number of "live" entries.
333     */
    size_t capacity() const {
        return segmentState.parts.topIndex;
    }

    iterator begin() {
        return iterator(table_, 0, capacity());
    }

    iterator end() {
        return iterator(table_, capacity(), capacity());
   }

private:
    static inline u4 extractIndex(IndirectRef iref) {
        u4 uref = (u4) iref;
        return (uref >> 2) & 0xffff;
    }

    static inline u4 extractSerial(IndirectRef iref) {
        u4 uref = (u4) iref;
        return uref >> 20;
    }

    static inline u4 nextSerial(u4 serial) {
        return (serial + 1) & 0xfff;
    }

    static inline IndirectRef toIndirectRef(u4 index, u4 serial, IndirectRefKind kind) {
        assert(index < 65536);
        return reinterpret_cast<IndirectRef>((serial << 20) | (index << 2) | kind);
    }
};

struct ReferenceTable {
    Object**        nextEntry;          /* top of the list */
    Object**        table;              /* bottom of the list */

    int             allocEntries;       /* #of entries we have space for */
    int             maxEntries;         /* max #of entries allowed */
};

struct HeapBitmap {
    /* The bitmap data, which points to an mmap()ed area of zeroed
46     * anonymous memory.
47     */
    unsigned long *bits;

    /* The size of the used memory pointed to by bits, in bytes.  This
51     * value changes when the bitmap is shrunk.
52     */
    size_t bitsLen;

    /* The real size of the memory pointed to by bits.  This is the
56     * number of bytes we requested from the allocator and does not
57     * change.
58     */
    size_t allocLen;

    /* The base address, which corresponds to the first bit in
62     * the bitmap.
63     */
    uintptr_t base;

    /* The highest pointer value ever returned by an allocation
67     * from this heap.  I.e., the highest address that may correspond
68     * to a set bit.  If there are no bits set, (max < base).
69     */
    uintptr_t max;
};

struct HeapSource {

    /* The base address of backing store. */
    u1 *blockBase;

    /* Total number of blocks available for allocation. */
    size_t totalBlocks;
    size_t allocBlocks;

    /*
188     * The scavenger work queue.  Implemented as an array of index
189     * values into the queue.
190     */
    size_t *blockQueue;

    /*
194     * Base and limit blocks.  Basically the shifted start address of
195     * the block.  We convert blocks to a relative number when
196     * indexing in the block queue.  TODO: make the block queue base
197     * relative rather than the index into the block queue.
198     */
    size_t baseBlock, limitBlock;

    size_t queueHead;
    size_t queueTail;
    size_t queueSize;

    /* The space of the current block 0 (free), 1 or 2. */
    char *blockSpace;

    /* Start of free space in the current block. */
    u1 *allocPtr;
    /* Exclusive limit of free space in the current block. */
    u1 *allocLimit;

    HeapBitmap allocBits;

    /*
216     * The starting size of the heap.  This value is the same as the
217     * value provided to the -Xms flag.
218     */
    size_t minimumSize;

    /*
222     * The maximum size of the heap.  This value is the same as the
223     * -Xmx flag.
224     */
   size_t maximumSize;

    /*
228     * The current, committed size of the heap.  At present, this is
229     * equivalent to the maximumSize.
230     */
    size_t currentSize;

    size_t bytesAllocated;
};

struct GcMarkStack {
    /* Highest address (exclusive)
24     */
    const Object **limit;

    /* Current top of the stack (exclusive)
28     */
    const Object **top;

    /* Lowest address (inclusive)
32     */
    const Object **base;

    /* Maximum stack size, in bytes.
36     */
    size_t length;
};

/* This is declared publicly so that it can be included in gDvm.gcHeap.
41 */
struct GcMarkContext {
    HeapBitmap *bitmap;
    GcMarkStack stack;
    const char *immuneLimit;
    const void *finger;   // only used while scanning/recursing.
};

struct GcHeap {
    HeapSource *heapSource;

    /* Linked lists of subclass instances of java/lang/ref/Reference
30     * that we find while recursing.  The "next" pointers are hidden
31     * in the Reference objects' pendingNext fields.  These lists are
32     * cleared and rebuilt each time the GC runs.
33     */
    Object *softReferences;
    Object *weakReferences;
    Object *finalizerReferences;
    Object *phantomReferences;

    /* The list of Reference objects that need to be enqueued.
40     */
    Object *clearedReferences;

    /* The current state of the mark step.
44     * Only valid during a GC.
45     */
    GcMarkContext markContext;

    /* GC's card table */
    u1* cardTableBase;
    size_t cardTableLength;
    size_t cardTableMaxLength;
    size_t cardTableOffset;

    /* Is the GC running?  Used to avoid recursive calls to GC.
55     */
    bool gcRunning;

    /*
59     * Debug control values
60     */
    int ddmHpifWhen;
    int ddmHpsgWhen;
    int ddmHpsgWhat;
    int ddmNhsgWhen;
    int ddmNhsgWhat;
};

struct Monitor {
    Thread*     owner;          /* which thread currently owns the lock? */
    int         lockCount;      /* owner's recursive lock depth */
    Object*     obj;            /* what object are we part of [debug only] */

    Thread*     waitSet;	/* threads currently waiting on this monitor */

    pthread_mutex_t lock;

    Monitor*    next;

    /*
84     * Who last acquired this monitor, when lock sampling is enabled.
85     * Even when enabled, ownerMethod may be NULL.
86     */
    const Method* ownerMethod;
    u4 ownerPc;
};

struct AtomicCacheEntry {
    u4          key1;
    u4          key2;
    u4          value;
    volatile u4 version;    /* version and lock flag */
};

/*
42 * One cache.
43 *
44 * Thought: we might be able to save a few cycles by storing the cache
45 * struct and "entries" separately, avoiding an indirection.  (We already
46 * handle "numEntries" separately in ATOMIC_CACHE_LOOKUP.)
47 */
struct AtomicCache {
    AtomicCacheEntry*   entries;        /* array of entries */
    int         numEntries;             /* #of entries, must be power of 2 */

    void*       entryAlloc;             /* memory allocated for entries */

    /* cache stats; note we don't guarantee atomic increments for these */
   int         trivial;                /* cache access not required */
    int         fail;                   /* contention failure */
    int         hits;                   /* found entry in cache */
    int         misses;                 /* entry was for other keys */
    int         fills;                  /* entry was empty */
};

struct InlineSub {
    Method* method;
    int     inlineIdx;
};

struct LinearAllocHdr {
   int     curOffset;          /* offset where next data goes */
    pthread_mutex_t lock;       /* controls updates to this struct */

    char*   mapAddr;            /* start of mmap()ed region */
    int     mapLength;          /* length of region */
    int     firstOffset;        /* for chasing through */

    short*  writeRefCount;      /* for ENFORCE_READ_ONLY */
};




/*
97 * Holds collection of JDWP initialization parameters.
98 */


struct JdwpState;

struct ExpandBuf {
   u1*     storage;
    int     curLen;
    int     maxLen;
};

/*
49 * Transport functions.
50 */
struct JdwpTransport {
    bool (*startup)(struct JdwpState* state, const JdwpStartupParams* pParams);
    bool (*accept)(struct JdwpState* state);
    bool (*establish)(struct JdwpState* state);
    void (*close)(struct JdwpState* state);
    void (*shutdown)(struct JdwpState* state);
    void (*free)(struct JdwpState* state);
    bool (*isConnected)(struct JdwpState* state);
    bool (*awaitingHandshake)(struct JdwpState* state);
    bool (*processIncoming)(struct JdwpState* state);
    bool (*sendRequest)(struct JdwpState* state, ExpandBuf* pReq);
    bool (*sendBufferedRequest)(struct JdwpState* state,
        const struct iovec* iov, int iovcnt);
};

class JdwpNetStateBase {
public:
    int             clientSock;     /* active connection to debugger */

    JdwpNetStateBase();
    ssize_t writePacket(ExpandBuf* pReply);
    ssize_t writeBufferedPacket(const struct iovec* iov, int iovcnt);

private:
    pthread_mutex_t socketLock;     /* socket synchronization */
};

#define kInputBufferSize    8192

typedef	uint32_t	in_addr_t;
struct in_addr {
	in_addr_t s_addr;
};


struct JdwpNetState : public JdwpNetStateBase {
    short   listenPort;
    int     listenSock;         /* listen for connection from debugger */
    int     wakePipe[2];        /* break out of select */

    struct in_addr remoteAddr;
    unsigned short remotePort;

    bool    awaitingHandshake;  /* waiting for "JDWP-Handshake" */

    /* pending data from the network; would be more efficient as circular buf */
    unsigned char  inputBuffer[kInputBufferSize];
    int     inputCount;

    JdwpNetState()
    {
        listenPort  = 0;
        listenSock  = -1;
        wakePipe[0] = -1;
        wakePipe[1] = -1;

        awaitingHandshake = false;

       inputCount = 0;
    }
};

typedef u8 ObjectId;
typedef u8 RefTypeId; 
typedef u4 FieldId;
typedef u4 MethodId;

struct JdwpLocation {
    u1          typeTag;        /* class or interface? */
    RefTypeId   classId;        /* method->clazz */
    MethodId    methodId;       /* method in which "idx" resides */
    u8          idx;            /* relative index into code block */
};
union JdwpEventMod {
    u1      modKind;                /* JdwpModKind */
    struct {
        u1          modKind;
        int         count;
    } count;
    struct {
        u1          modKind;
       u4          exprId;
    } conditional;
    struct {
        u1          modKind;
        ObjectId    threadId;
    } threadOnly;
    struct {
        u1          modKind;
        RefTypeId   refTypeId;
    } classOnly;
    struct {
       u1          modKind;
        char*       classPattern;
    } classMatch;
    struct {
        u1          modKind;
        char*       classPattern;
    } classExclude;
    struct {
        u1          modKind;
        JdwpLocation loc;
    } locationOnly;
    struct {
        u1          modKind;
        u1          caught;
        u1          uncaught;
        RefTypeId   refTypeId;
    } exceptionOnly;
    struct {
        u1          modKind;
        RefTypeId   refTypeId;
        FieldId     fieldId;
    } fieldOnly;
    struct {
        u1          modKind;
        ObjectId    threadId;
        int         size;           /* JdwpStepSize */
        int         depth;          /* JdwpStepDepth */
    } step;
    struct {
        u1          modKind;
        ObjectId    objectId;
    } instanceOnly;
};

enum JdwpEventKind {
    EK_SINGLE_STEP          = 1,
    EK_BREAKPOINT           = 2,
    EK_FRAME_POP            = 3,
   EK_EXCEPTION            = 4,
    EK_USER_DEFINED         = 5,
    EK_THREAD_START         = 6,
    EK_THREAD_END           = 7,
    EK_CLASS_PREPARE        = 8,
    EK_CLASS_UNLOAD         = 9,
    EK_CLASS_LOAD           = 10,
    EK_FIELD_ACCESS         = 20,
    EK_FIELD_MODIFICATION   = 21,
    EK_EXCEPTION_CATCH      = 30,
    EK_METHOD_ENTRY         = 40,
    EK_METHOD_EXIT          = 41,
    EK_VM_INIT              = 90,
    EK_VM_DEATH             = 99,
    EK_VM_DISCONNECTED      = 100,  /* "Never sent across JDWP */
    EK_VM_START             = EK_VM_INIT,
    EK_THREAD_DEATH         = EK_THREAD_END,
};

enum JdwpSuspendPolicy {
    SP_NONE                 = 0,    /* suspend no threads */
    SP_EVENT_THREAD         = 1,    /* suspend event thread */
    SP_ALL                  = 2,    /* suspend all threads */
};

/*
82 * One of these for every registered event.
83 *
84 * We over-allocate the struct to hold the modifiers.
85 */
struct JdwpEvent {
    JdwpEvent* prev;           /* linked list */
    JdwpEvent* next;

    JdwpEventKind eventKind;      /* what kind of event is this? */
    JdwpSuspendPolicy suspendPolicy;  /* suspend all, none, or self? */
    int modCount;       /* #of entries in mods[] */
    u4 requestId;      /* serial#, reported to debugger */

   JdwpEventMod mods[1];        /* MUST be last field in struct */
};


struct JdwpState {
    JdwpStartupParams   params;

    /* wait for creation of the JDWP thread */
    pthread_mutex_t threadStartLock;
    pthread_cond_t  threadStartCond;

    int             debugThreadStarted;
    pthread_t       debugThreadHandle;
    ObjectId        debugThreadId;
    bool            run;

    const JdwpTransport*    transport;
    JdwpNetState*   netState;

    /* for wait-for-debugger */
    pthread_mutex_t attachLock;
    pthread_cond_t  attachCond;

    /* time of last debugger activity, in milliseconds */
    s8              lastActivityWhen;

    /* global counters and a mutex to protect them */
    u4              requestSerial;
    u4              eventSerial;
    pthread_mutex_t serialLock;

    /*
101     * Events requested by the debugger (breakpoints, class prep, etc).
102     */
    int             numEvents;      /* #of elements in eventList */
    JdwpEvent*      eventList;      /* linked list of events */
    pthread_mutex_t eventLock;      /* guards numEvents/eventList */

    /*
108     * Synchronize suspension of event thread (to avoid receiving "resume"
109     * events before the thread has finished suspending itself).
110     */
    pthread_mutex_t eventThreadLock;
    pthread_cond_t  eventThreadCond;
   ObjectId        eventThreadId;

    /*
116     * DDM support.
117     */
    bool            ddmActive;
};

struct Breakpoint {
    Method*     method;                 /* method we're associated with */
    u2*         addr;                   /* absolute memory address */
    u1          originalOpcode;         /* original 8-bit opcode value */
    int         setCount;               /* #of times this breakpoint was set */
};

/*
90 * Set of breakpoints.
91 */
struct BreakpointSet {
    /* grab lock before reading or writing anything else in here */
    pthread_mutex_t lock;

    /* vector of breakpoint structures */
    int         alloc;
    int         count;
    Breakpoint* breakpoints;
};

enum JdwpStepSize {
   SS_MIN                  = 0,    /* step by minimum (e.g. 1 bytecode inst) */
    SS_LINE                 = 1,    /* if possible, step to next line */
};

enum JdwpStepDepth {
    SD_INTO                 = 0,    /* step into method calls */
    SD_OVER                 = 1,    /* step over method calls */
    SD_OUT                  = 2,    /* step out of current method */
};

struct AddressSet {
   u4 setSize;
    u1 set[1];
};

struct StepControl {
    /* request */
    JdwpStepSize size;
    JdwpStepDepth depth;
    Thread* thread;         /* don't deref; for comparison only */

    /* current state */
    bool active;
    const Method* method;
    int line;           /* line #; could be -1 */
    const AddressSet* pAddressSet;    /* if non-null, address set for line */
    int frameDepth;
};

#define kMaxAllocRecordStackDepth   16

struct AllocRecord {
   ClassObject*    clazz;      /* class allocated in this block */
    u4              size;       /* total size requested */
    u2              threadId;   /* simple thread ID; could be recycled */

    /* stack trace elements; unused entries have method==NULL */
    struct {
        const Method* method;   /* which method we're executing in */
        int         pc;         /* current execution offset, in 16-bit units */
    } stackElem[kMaxAllocRecordStackDepth];
};

struct MethodTraceState {
    /* active state */
    pthread_mutex_t startStopLock;
    pthread_cond_t  threadExitCond;
    FILE*   traceFile;
    bool    directToDdms;
    int     bufferSize;
    int     flags;

    int     traceEnabled;
    u1*     buf;
    volatile int curOffset;
    u8      startWhen;
    int     overflow;

    int     traceVersion;
    size_t  recordSize;

    bool    samplingEnabled;
    pthread_t       samplingThreadHandle;
};

struct AllocProfState {
    bool    enabled;            // is allocation tracking enabled?

    int     allocCount;         // #of objects allocated
    int     allocSize;          // cumulative size of objects

    int     failedAllocCount;   // #of times an allocation failed
    int     failedAllocSize;    // cumulative size of failed allocations

    int     freeCount;          // #of objects freed
    int     freeSize;           // cumulative size of freed objects

    int     gcCount;            // #of times an allocation triggered a GC

    int     classInitCount;     // #of initialized classes
    u8      classInitTime;      // cumulative time spent in class init (nsec)
};

struct StaticField : Field {
    JValue          value;          /* initially set from DEX for primitives */
};

/*
315 * Instance field.
316 */
struct InstField : Field {
    /*
319     * This field indicates the byte offset from the beginning of the
320     * (Object *) to the actual instance data; e.g., byteOffset==0 is
321     * the same as the object pointer (bug!), and byteOffset==4 is 4
322     * bytes farther.
323     */
    int             byteOffset;
};

struct Method {
  /* the class we are a part of */
   ClassObject*    clazz;

    /* access flags; low 16 bits are defined by spec (could be u2?) */
    u4              accessFlags;

   /*
494     * For concrete virtual methods, this is the offset of the method
495     * in "vtable".
496     *
497     * For abstract methods in an interface class, this is the offset
498     * of the method in "iftable[n]->methodIndexArray".
499     */
   u2             methodIndex;

    /*
503     * Method bounds; not needed for an abstract method.
504     *
505     * For a native method, we compute the size of the argument list, and
506     * set "insSize" and "registerSize" equal to it.
507     */
   u2              registersSize;  /* ins + locals */
    u2              outsSize;
    u2              insSize;

    /* method name, e.g. "<init>" or "eatLunch" */
    const char*     name;

    /*
516     * Method prototype descriptor string (return and argument types).
517     *
518     * TODO: This currently must specify the DexFile as well as the proto_ids
519     * index, because generated Proxy classes don't have a DexFile.  We can
520     * remove the DexFile* and reduce the size of this struct if we generate
521     * a DEX for proxies.
522     */
    DexProto        prototype;

   /* short-form method descriptor string */
   const char*     shorty;

    /*
529     * The remaining items are not used for abstract or native methods.
530     * (JNI is currently hijacking "insns" as a function pointer, set
531     * after the first call.  For internal-native this stays null.)
532     */

    /* the actual code */
    const u2*       insns;          /* instructions, in memory-mapped .dex */

    /* JNI: cached argument and return-type hints */
    int             jniArgInfo;

    /*
541     * JNI: native method ptr; could be actual function or a JNI bridge.  We
542     * don't currently discriminate between DalvikBridgeFunc and
543     * DalvikNativeFunc; the former takes an argument superset (i.e. two
544     * extra args) which will be ignored.  If necessary we can use
545     * insns==NULL to detect JNI bridge vs. internal native.
546     */
    DalvikBridgeFunc nativeFunc;

    /*
550     * JNI: true if this static non-synchronized native method (that has no
551     * reference arguments) needs a JNIEnv* and jclass/jobject. Libcore
552     * uses this.
553     */
    bool fastJni;

    /*
557     * JNI: true if this method has no reference arguments. This lets the JNI
558     * bridge avoid scanning the shorty for direct pointers that need to be
559     * converted to local references.
560     *
561     * TODO: replace this with a list of indexes of the reference arguments.
562     */
    bool noRef;

    /*
566     * JNI: true if we should log entry and exit. This is the only way
567     * developers can log the local references that are passed into their code.
568     * Used for debugging JNI problems in third-party code.
569     */
    bool shouldTrace;

    /*
573     * Register map data, if available.  This will point into the DEX file
574     * if the data was computed during pre-verification, or into the
575     * linear alloc area if not.
576     */
    const RegisterMap* registerMap;

    /* set if method was called during method profiling */
    bool            inProfile;
};

struct RegisterMapClassPool {
    u4      numClasses;
    /* offset table starts here, 32-bit aligned; offset==0 means no data */
    u4      classDataOffset[1];
};

struct ClassObject : Object {
    /* leave space for instance data; we could access fields directly if we
350       freeze the definition of java/lang/Class */
    u4              instanceData[4];

    /* UTF-8 descriptor for the class; from constant pool, or on heap
354       if generated ("[C") */
    const char*     descriptor;
    char*           descriptorAlloc;

    /* access flags; low 16 bits are defined by VM spec */
    u4              accessFlags;

    /* VM-unique class serial number, nonzero, set very early */
    u4              serialNumber;

    /* DexFile from which we came; needed to resolve constant pool entries */
    /* (will be NULL for VM-generated, e.g. arrays and primitive classes) */
    DvmDex*         pDvmDex;

   /* state of class initialization */
   ClassStatus     status;

    /* if class verify fails, we must return same error on subsequent tries */
    ClassObject*    verifyErrorClass;

    /* threadId, used to check for recursive <clinit> invocation */
   u4              initThreadId;

    /*
378     * Total object size; used when allocating storage on gc heap.  (For
379     * interfaces and abstract classes this will be zero.)
380     */
    size_t          objectSize;

    /* arrays only: class object for base element, for instanceof/checkcast
384       (for String[][][], this will be String) */
    ClassObject*    elementClass;

    /* arrays only: number of dimensions, e.g. int[][] is 2 */
   int             arrayDim;

    /* primitive type index, or PRIM_NOT (-1); set for generated prim classes */
   PrimitiveType   primitiveType;

    /* superclass, or NULL if this is java.lang.Object */
    ClassObject*    super;

    /* defining class loader, or NULL for the "bootstrap" system loader */
    Object*         classLoader;

    /* initiating class loader list */
    /* NOTE: for classes with low serialNumber, these are unused, and the
401       values are kept in a table in gDvm. */
    InitiatingLoaderList initiatingLoaderList;

    /* array of interfaces this class implements directly */
    int             interfaceCount;
    ClassObject**   interfaces;

    /* static, private, and <init> methods */
   int             directMethodCount;
    Method*         directMethods;

   /* virtual methods defined in this class; invoked through vtable */
    int             virtualMethodCount;
    Method*         virtualMethods;

    /*
417     * Virtual method table (vtable), for use by "invoke-virtual".  The
418     * vtable from the superclass is copied in, and virtual methods from
419     * our class either replace those from the super or are appended.
420     */
    int             vtableCount;
    Method**        vtable;

   /*
425     * Interface table (iftable), one entry per interface supported by
426     * this class.  That means one entry for each interface we support
427     * directly, indirectly via superclass, or indirectly via
428     * superinterface.  This will be null if neither we nor our superclass
429     * implement any interfaces.
430     *
431     * Why we need this: given "class Foo implements Face", declare
432     * "Face faceObj = new Foo()".  Invoke faceObj.blah(), where "blah" is
433     * part of the Face interface.  We can't easily use a single vtable.
434     *
435     * For every interface a concrete class implements, we create a list of
436     * virtualMethod indices for the methods in the interface.
437     */
   int             iftableCount;
   InterfaceEntry* iftable;

    /*
442     * The interface vtable indices for iftable get stored here.  By placing
443     * them all in a single pool for each class that implements interfaces,
444     * we decrease the number of allocations.
445     */
    int             ifviPoolCount;
    int*            ifviPool;

    /* instance fields
     *
     * These describe the layout of the contents of a DataObject-compatible
     * Object.  Note that only the fields directly defined by this class
     * are listed in ifields;  fields defined by a superclass are listed
     * in the superclass's ClassObject.ifields.
     *
     * All instance fields that refer to objects are guaranteed to be
     * at the beginning of the field list.  ifieldRefCount specifies
     * the number of reference fields.
     */
    int             ifieldCount;
    int             ifieldRefCount; // number of fields that are object refs
   InstField*      ifields;

    /* bitmap of offsets of ifields */
    u4 refOffsets;

    /* source file name, if known */
   const char*     sourceFile;

    /* static fields */
    int             sfieldCount;
    StaticField     sfields[0]; /* MUST be last item */
};




/*
 * One of these for each -ea/-da/-esa/-dsa on the command line.
 */
struct AssertionControl {
    char*   pkgOrClass;         /* package/class string, or NULL for esa/dsa */
    int     pkgOrClassLen;      /* string length, for quick compare */
    bool    enable;             /* enable or disable */
    bool    isPackage;          /* string ended with "..."? */
};

/*
 * Register map generation mode.  Only applicable when generateRegisterMaps
 * is enabled.  (The "disabled" state is not folded into this because
 * there are callers like dexopt that want to enable/disable without
 * specifying the configuration details.)
 *
 * "TypePrecise" is slower and requires additional storage for the register
 * maps, but allows type-precise GC.  "LivePrecise" is even slower and
 * requires additional heap during processing, but allows live-precise GC.
 */
enum RegisterMapMode {
    kRegisterMapModeUnknown = 0,
    kRegisterMapModeTypePrecise,
    kRegisterMapModeLivePrecise
};

/*
 * Profiler clock source.
 */
enum ProfilerClockSource {
    kProfilerClockSourceThreadCpu,
    kProfilerClockSourceWall,
    kProfilerClockSourceDual,
};


struct DvmGlobals_15 {
    /*
     * Some options from the command line or environment.
     */
    char*       bootClassPathStr;
    char*       classPathStr;

    size_t      heapStartingSize;
    size_t      heapMaximumSize;
    size_t      heapGrowthLimit;
    size_t      stackSize;

    bool        verboseGc;
    bool        verboseJni;
    bool        verboseClass;
    bool        verboseShutdown;

    bool        jdwpAllowed;        // debugging allowed for this process?
    bool        jdwpConfigured;     // has debugging info been provided?
    JdwpTransportType jdwpTransport;
    bool        jdwpServer;
    char*       jdwpHost;
    int         jdwpPort;
    bool        jdwpSuspend;

    ProfilerClockSource profilerClockSource;

    /*
     * Lock profiling threshold value in milliseconds.  Acquires that
     * exceed threshold are logged.  Acquires within the threshold are
     * logged with a probability of $\frac{time}{threshold}$ .  If the
     * threshold is unset no additional logging occurs.
     */
    u4          lockProfThreshold;

    int         (*vfprintfHook)(FILE*, const char*, va_list);
    void        (*exitHook)(int);
    void        (*abortHook)(void);
    bool        (*isSensitiveThreadHook)(void);

    int         jniGrefLimit;       // 0 means no limit
    char*       jniTrace;
    bool        reduceSignals;
    bool        noQuitHandler;
    bool        verifyDexChecksum;
    char*       stackTraceFile;     // for SIGQUIT-inspired output

    bool        logStdio;

    DexOptimizerMode    dexOptMode;
    DexClassVerifyMode  classVerifyMode;

    bool        generateRegisterMaps;
    RegisterMapMode     registerMapMode;

    bool        monitorVerification;

    bool        dexOptForSmp;

    /*
     * GC option flags.
     */
    bool        preciseGc;
    bool        preVerify;
    bool        postVerify;
    bool        concurrentMarkSweep;
    bool        verifyCardTable;
    bool        disableExplicitGc;

    int         assertionCtrlCount;
    AssertionControl*   assertionCtrl;

    ExecutionMode   executionMode;

    /*
     * VM init management.
     */
    bool        initializing;
    bool        optimizing;

    /*
     * java.lang.System properties set from the command line with -D.
     * This is effectively a set, where later entries override earlier
     * ones.
     */
    std::vector<std::string>* properties;

    /*
     * Where the VM goes to find system classes.
     */
    ClassPathEntry* bootClassPath;
    /* used by the DEX optimizer to load classes from an unfinished DEX */
    DvmDex*     bootClassPathOptExtra;
    bool        optimizingBootstrapClass;

    /*
     * Loaded classes, hashed by class name.  Each entry is a ClassObject*,
     * allocated in GC space.
     */
    HashTable*  loadedClasses;

    /*
     * Value for the next class serial number to be assigned.  This is
     * incremented as we load classes.  Failed loads and races may result
     * in some numbers being skipped, and the serial number is not
     * guaranteed to start at 1, so the current value should not be used
     * as a count of loaded classes.
     */
    volatile int classSerialNumber;

    /*
     * Classes with a low classSerialNumber are probably in the zygote, and
     * their InitiatingLoaderList is not used, to promote sharing. The list is
     * kept here instead.
     */
    InitiatingLoaderList* initiatingLoaderList;

    /*
     * Interned strings.
     */

    /* A mutex that guards access to the interned string tables. */
    pthread_mutex_t internLock;

    /* Hash table of strings interned by the user. */
    HashTable*  internedStrings;

    /* Hash table of strings interned by the class loader. */
    HashTable*  literalStrings;

    /*
     * Classes constructed directly by the vm.
     */

    /* the class Class */
    ClassObject* classJavaLangClass;

    /* synthetic classes representing primitive types */
    ClassObject* typeVoid;
    ClassObject* typeBoolean;
    ClassObject* typeByte;
    ClassObject* typeShort;
    ClassObject* typeChar;
    ClassObject* typeInt;
    ClassObject* typeLong;
    ClassObject* typeFloat;
    ClassObject* typeDouble;

    /* synthetic classes for arrays of primitives */
    ClassObject* classArrayBoolean;
    ClassObject* classArrayByte;
    ClassObject* classArrayShort;
    ClassObject* classArrayChar;
    ClassObject* classArrayInt;
    ClassObject* classArrayLong;
    ClassObject* classArrayFloat;
    ClassObject* classArrayDouble;

    /*
     * Quick lookups for popular classes used internally.
     */
    ClassObject* classJavaLangClassArray;
    ClassObject* classJavaLangClassLoader;
    ClassObject* classJavaLangObject;
    ClassObject* classJavaLangObjectArray;
    ClassObject* classJavaLangString;
    ClassObject* classJavaLangThread;
    ClassObject* classJavaLangVMThread;
    ClassObject* classJavaLangThreadGroup;
    ClassObject* classJavaLangStackTraceElement;
    ClassObject* classJavaLangStackTraceElementArray;
    ClassObject* classJavaLangAnnotationAnnotationArray;
    ClassObject* classJavaLangAnnotationAnnotationArrayArray;
    ClassObject* classJavaLangReflectAccessibleObject;
    ClassObject* classJavaLangReflectConstructor;
    ClassObject* classJavaLangReflectConstructorArray;
    ClassObject* classJavaLangReflectField;
    ClassObject* classJavaLangReflectFieldArray;
    ClassObject* classJavaLangReflectMethod;
    ClassObject* classJavaLangReflectMethodArray;
    ClassObject* classJavaLangReflectProxy;
    ClassObject* classJavaNioReadWriteDirectByteBuffer;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationFactory;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationMember;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationMemberArray;
    ClassObject* classOrgApacheHarmonyDalvikDdmcChunk;
    ClassObject* classOrgApacheHarmonyDalvikDdmcDdmServer;
    ClassObject* classJavaLangRefFinalizerReference;

    /*
     * classes representing exception types. The names here don't include
     * packages, just to keep the use sites a bit less verbose. All are
     * in java.lang, except where noted.
     */
    ClassObject* exAbstractMethodError;
    ClassObject* exArithmeticException;
    ClassObject* exArrayIndexOutOfBoundsException;
    ClassObject* exArrayStoreException;
    ClassObject* exClassCastException;
    ClassObject* exClassCircularityError;
    ClassObject* exClassFormatError;
    ClassObject* exClassNotFoundException;
    ClassObject* exError;
    ClassObject* exExceptionInInitializerError;
    ClassObject* exFileNotFoundException; /* in java.io */
    ClassObject* exIOException;           /* in java.io */
    ClassObject* exIllegalAccessError;
    ClassObject* exIllegalAccessException;
    ClassObject* exIllegalArgumentException;
    ClassObject* exIllegalMonitorStateException;
    ClassObject* exIllegalStateException;
    ClassObject* exIllegalThreadStateException;
    ClassObject* exIncompatibleClassChangeError;
    ClassObject* exInstantiationError;
    ClassObject* exInstantiationException;
    ClassObject* exInternalError;
    ClassObject* exInterruptedException;
    ClassObject* exLinkageError;
    ClassObject* exNegativeArraySizeException;
    ClassObject* exNoClassDefFoundError;
    ClassObject* exNoSuchFieldError;
    ClassObject* exNoSuchFieldException;
    ClassObject* exNoSuchMethodError;
    ClassObject* exNullPointerException;
    ClassObject* exOutOfMemoryError;
    ClassObject* exRuntimeException;
    ClassObject* exStackOverflowError;
    ClassObject* exStaleDexCacheError;    /* in dalvik.system */
    ClassObject* exStringIndexOutOfBoundsException;
    ClassObject* exThrowable;
    ClassObject* exTypeNotPresentException;
    ClassObject* exUnsatisfiedLinkError;
    ClassObject* exUnsupportedOperationException;
    ClassObject* exVerifyError;
    ClassObject* exVirtualMachineError;

    /* method offsets - Object */
    int         voffJavaLangObject_equals;
    int         voffJavaLangObject_hashCode;
    int         voffJavaLangObject_toString;

    /* field offsets - String */
    int         offJavaLangString_value;
    int         offJavaLangString_count;
    int         offJavaLangString_offset;
    int         offJavaLangString_hashCode;

    /* field offsets - Thread */
    int         offJavaLangThread_vmThread;
    int         offJavaLangThread_group;
    int         offJavaLangThread_daemon;
    int         offJavaLangThread_name;
    int         offJavaLangThread_priority;
    int         offJavaLangThread_uncaughtHandler;
    int         offJavaLangThread_contextClassLoader;

    /* method offsets - Thread */
    int         voffJavaLangThread_run;

    /* field offsets - ThreadGroup */
    int         offJavaLangThreadGroup_name;
    int         offJavaLangThreadGroup_parent;

    /* field offsets - VMThread */
    int         offJavaLangVMThread_thread;
    int         offJavaLangVMThread_vmData;

    /* method offsets - ThreadGroup */
    int         voffJavaLangThreadGroup_removeThread;

    /* field offsets - Throwable */
    int         offJavaLangThrowable_stackState;
    int         offJavaLangThrowable_cause;

    /* method offsets - ClassLoader */
    int         voffJavaLangClassLoader_loadClass;

    /* direct method pointers - ClassLoader */
    Method*     methJavaLangClassLoader_getSystemClassLoader;

    /* field offsets - java.lang.reflect.* */
    int         offJavaLangReflectConstructor_slot;
    int         offJavaLangReflectConstructor_declClass;
    int         offJavaLangReflectField_slot;
    int         offJavaLangReflectField_declClass;
    int         offJavaLangReflectMethod_slot;
    int         offJavaLangReflectMethod_declClass;

    /* field offsets - java.lang.ref.Reference */
    int         offJavaLangRefReference_referent;
    int         offJavaLangRefReference_queue;
    int         offJavaLangRefReference_queueNext;
    int         offJavaLangRefReference_pendingNext;

    /* field offsets - java.lang.ref.FinalizerReference */
    int offJavaLangRefFinalizerReference_zombie;

    /* method pointers - java.lang.ref.ReferenceQueue */
    Method* methJavaLangRefReferenceQueueAdd;

    /* method pointers - java.lang.ref.FinalizerReference */
    Method* methJavaLangRefFinalizerReferenceAdd;

    /* constructor method pointers; no vtable involved, so use Method* */
    Method*     methJavaLangStackTraceElement_init;
    Method*     methJavaLangReflectConstructor_init;
    Method*     methJavaLangReflectField_init;
    Method*     methJavaLangReflectMethod_init;
    Method*     methOrgApacheHarmonyLangAnnotationAnnotationMember_init;

    /* static method pointers - android.lang.annotation.* */
    Method*
        methOrgApacheHarmonyLangAnnotationAnnotationFactory_createAnnotation;

    /* direct method pointers - java.lang.reflect.Proxy */
    Method*     methJavaLangReflectProxy_constructorPrototype;

    /* field offsets - java.lang.reflect.Proxy */
    int         offJavaLangReflectProxy_h;

    /* field offsets - java.io.FileDescriptor */
    int         offJavaIoFileDescriptor_descriptor;

    /* direct method pointers - dalvik.system.NativeStart */
    Method*     methDalvikSystemNativeStart_main;
    Method*     methDalvikSystemNativeStart_run;

    /* assorted direct buffer helpers */
    Method*     methJavaNioReadWriteDirectByteBuffer_init;
    int         offJavaNioBuffer_capacity;
    int         offJavaNioBuffer_effectiveDirectAddress;

    /* direct method pointers - org.apache.harmony.dalvik.ddmc.DdmServer */
    Method*     methDalvikDdmcServer_dispatch;
    Method*     methDalvikDdmcServer_broadcast;

    /* field offsets - org.apache.harmony.dalvik.ddmc.Chunk */
    int         offDalvikDdmcChunk_type;
    int         offDalvikDdmcChunk_data;
    int         offDalvikDdmcChunk_offset;
    int         offDalvikDdmcChunk_length;

    /*
     * Thread list.  This always has at least one element in it (main),
     * and main is always the first entry.
     *
     * The threadListLock is used for several things, including the thread
     * start condition variable.  Generally speaking, you must hold the
     * threadListLock when:
     *  - adding/removing items from the list
     *  - waiting on or signaling threadStartCond
     *  - examining the Thread struct for another thread (this is to avoid
     *    one thread freeing the Thread struct while another thread is
     *    perusing it)
     */
    Thread*     threadList;
    pthread_mutex_t threadListLock;

    pthread_cond_t threadStartCond;

    /*
     * The thread code grabs this before suspending all threads.  There
     * are a few things that can cause a "suspend all":
     *  (1) the GC is starting;
     *  (2) the debugger has sent a "suspend all" request;
     *  (3) a thread has hit a breakpoint or exception that the debugger
     *      has marked as a "suspend all" event;
     *  (4) the SignalCatcher caught a signal that requires suspension.
     *  (5) (if implemented) the JIT needs to perform a heavyweight
     *      rearrangement of the translation cache or JitTable.
     *
     * Because we use "safe point" self-suspension, it is never safe to
     * do a blocking "lock" call on this mutex -- if it has been acquired,
     * somebody is probably trying to put you to sleep.  The leading '_' is
     * intended as a reminder that this lock is special.
     */
    pthread_mutex_t _threadSuspendLock;

    /*
     * Guards Thread->suspendCount for all threads, and
     * provides the lock for the condition variable that all suspended threads
     * sleep on (threadSuspendCountCond).
     *
     * This has to be separate from threadListLock because of the way
     * threads put themselves to sleep.
     */
    pthread_mutex_t threadSuspendCountLock;

    /*
     * Suspended threads sleep on this.  They should sleep on the condition
     * variable until their "suspend count" is zero.
     *
     * Paired with "threadSuspendCountLock".
     */
    pthread_cond_t  threadSuspendCountCond;

    /*
     * Sum of all threads' suspendCount fields. Guarded by
     * threadSuspendCountLock.
     */
    int  sumThreadSuspendCount;

    /*
     * MUTEX ORDERING: when locking multiple mutexes, always grab them in
     * this order to avoid deadlock:
     *
     *  (1) _threadSuspendLock      (use lockThreadSuspend())
     *  (2) threadListLock          (use dvmLockThreadList())
     *  (3) threadSuspendCountLock  (use lockThreadSuspendCount())
     */


    /*
     * Thread ID bitmap.  We want threads to have small integer IDs so
     * we can use them in "thin locks".
     */
    BitVector*  threadIdMap;

    /*
     * Manage exit conditions.  The VM exits when all non-daemon threads
     * have exited.  If the main thread returns early, we need to sleep
     * on a condition variable.
     */
    int         nonDaemonThreadCount;   /* must hold threadListLock to access */
    pthread_cond_t  vmExitCond;

    /*
     * The set of DEX files loaded by custom class loaders.
     */
    HashTable*  userDexFiles;

    /*
     * JNI global reference table.
     */
    IndirectRefTable jniGlobalRefTable;
    IndirectRefTable jniWeakGlobalRefTable;
    pthread_mutex_t jniGlobalRefLock;
    pthread_mutex_t jniWeakGlobalRefLock;
    int         jniGlobalRefHiMark;
    int         jniGlobalRefLoMark;

    /*
     * JNI pinned object table (used for primitive arrays).
     */
    ReferenceTable  jniPinRefTable;
    pthread_mutex_t jniPinRefLock;

    /*
     * Native shared library table.
     */
    HashTable*  nativeLibs;

    /*
     * GC heap lock.  Functions like gcMalloc() acquire this before making
     * any changes to the heap.  It is held throughout garbage collection.
     */
    pthread_mutex_t gcHeapLock;

    /*
     * Condition variable to queue threads waiting to retry an
     * allocation.  Signaled after a concurrent GC is completed.
     */
    pthread_cond_t gcHeapCond;

    /* Opaque pointer representing the heap. */
    GcHeap*     gcHeap;

    /* The card table base, modified as needed for marking cards. */
    u1*         biasedCardTableBase;

    /*
     * Pre-allocated throwables.
     */
    Object*     outOfMemoryObj;
    Object*     internalErrorObj;
    Object*     noClassDefFoundErrorObj;

    /* Monitor list, so we can free them */
    /*volatile*/ Monitor* monitorList;

    /* Monitor for Thread.sleep() implementation */
    Monitor*    threadSleepMon;

    /* set when we create a second heap inside the zygote */
    bool        newZygoteHeapAllocated;

    /*
     * TLS keys.
     */
    pthread_key_t pthreadKeySelf;       /* Thread*, for dvmThreadSelf */

    /*
     * Cache results of "A instanceof B".
     */
    AtomicCache* instanceofCache;

    /* inline substitution table, used during optimization */
    InlineSub*          inlineSubs;

    /*
     * Bootstrap class loader linear allocator.
     */
    LinearAllocHdr* pBootLoaderAlloc;

    /*
     * Compute some stats on loaded classes.
     */
    int         numLoadedClasses;
    int         numDeclaredMethods;
    int         numDeclaredInstFields;
    int         numDeclaredStaticFields;

    /* when using a native debugger, set this to suppress watchdog timers */
    bool        nativeDebuggerActive;

    /*
     * JDWP debugger support.
     *
     * Note: Each thread will normally determine whether the debugger is active
     * for it by referring to its subMode flags.  "debuggerActive" here should be
     * seen as "debugger is making requests of 1 or more threads".
     */
    bool        debuggerConnected;      /* debugger or DDMS is connected */
    bool        debuggerActive;         /* debugger is making requests */
    JdwpState*  jdwpState;

    /*
     * Registry of objects known to the debugger.
     */
    HashTable*  dbgRegistry;

    /*
     * Debugger breakpoint table.
     */
    BreakpointSet*  breakpointSet;

    /*
     * Single-step control struct.  We currently only allow one thread to
     * be single-stepping at a time, which is all that really makes sense,
     * but it's possible we may need to expand this to be per-thread.
     */
    StepControl stepControl;

    /*
     * DDM features embedded in the VM.
     */
    bool        ddmThreadNotification;

    /*
     * Zygote (partially-started process) support
     */
    bool        zygote;

    /*
     * Used for tracking allocations that we report to DDMS.  When the feature
     * is enabled (through a DDMS request) the "allocRecords" pointer becomes
     * non-NULL.
     */
    pthread_mutex_t allocTrackerLock;
    AllocRecord*    allocRecords;
    int             allocRecordHead;        /* most-recently-added entry */
    int             allocRecordCount;       /* #of valid entries */

    /*
     * When a profiler is enabled, this is incremented.  Distinct profilers
     * include "dmtrace" method tracing, emulator method tracing, and
     * possibly instruction counting.
     *
     * The purpose of this is to have a single value that shows whether any
     * profiling is going on.  Individual thread will normally check their
     * thread-private subMode flags to take any profiling action.
     */
    volatile int activeProfilers;

    /*
     * State for method-trace profiling.
     */
    MethodTraceState methodTrace;
    Method*     methodTraceGcMethod;
    Method*     methodTraceClassPrepMethod;

    /*
     * State for emulator tracing.
     */
    void*       emulatorTracePage;
    int         emulatorTraceEnableCount;

    /*
     * Global state for memory allocation profiling.
     */
    AllocProfState allocProf;

    /*
     * Pointers to the original methods for things that have been inlined.
     * This makes it easy for us to output method entry/exit records for
     * the method calls we're not actually making.  (Used by method
     * profiling.)
     */
    Method**    inlinedMethods;

    /*
     * Dalvik instruction counts (kNumPackedOpcodes entries).
     */
    int*        executedInstrCounts;
    int         instructionCountEnableCount;

    /*
     * Signal catcher thread (for SIGQUIT).
     */
    pthread_t   signalCatcherHandle;
    bool        haltSignalCatcher;

    /*
     * Stdout/stderr conversion thread.
     */
    bool            haltStdioConverter;
    bool            stdioConverterReady;
    pthread_t       stdioConverterHandle;
    pthread_mutex_t stdioConverterLock;
    pthread_cond_t  stdioConverterCond;
    int             stdoutPipe[2];
    int             stderrPipe[2];

    /*
     * pid of the system_server process. We track it so that when system server
     * crashes the Zygote process will be killed and restarted.
     */
    pid_t systemServerPid;

    int kernelGroupScheduling;

//#define COUNT_PRECISE_METHODS
#ifdef COUNT_PRECISE_METHODS
    PointerSet* preciseMethods;
#endif

    /* some RegisterMap statistics, useful during development */
    void*       registerMapStats;

#ifdef VERIFIER_STATS
    VerifierStats verifierStats;
#endif

    /* String pointed here will be deposited on the stack frame of dvmAbort */
    const char *lastMessage;
};

struct DvmGlobals_19 {
    /*
     * Some options from the command line or environment.
     */
    char*       bootClassPathStr;
    char*       classPathStr;
    size_t      heapStartingSize;
    size_t      heapMaximumSize;
    size_t      heapGrowthLimit;
    bool        lowMemoryMode;
    double      heapTargetUtilization;
    size_t      heapMinFree;
    size_t      heapMaxFree;
    size_t      stackSize;
    size_t      mainThreadStackSize;
    bool        verboseGc;
    bool        verboseJni;
    bool        verboseClass;
    bool        verboseShutdown;
    bool        jdwpAllowed;        // debugging allowed for this process?
    bool        jdwpConfigured;     // has debugging info been provided?
    JdwpTransportType jdwpTransport;
    bool        jdwpServer;
    char*       jdwpHost;
    int         jdwpPort;
    bool        jdwpSuspend;
    ProfilerClockSource profilerClockSource;
    /*
     * Lock profiling threshold value in milliseconds.  Acquires that
     * exceed threshold are logged.  Acquires within the threshold are
     * logged with a probability of $\frac{time}{threshold}$ .  If the
     * threshold is unset no additional logging occurs.
     */
    u4          lockProfThreshold;
    int         (*vfprintfHook)(FILE*, const char*, va_list);
    void        (*exitHook)(int);
    void        (*abortHook)(void);
    bool        (*isSensitiveThreadHook)(void);
    char*       jniTrace;
    bool        reduceSignals;
    bool        noQuitHandler;
    bool        verifyDexChecksum;
    char*       stackTraceFile;     // for SIGQUIT-inspired output
    bool        logStdio;
    DexOptimizerMode    dexOptMode;
    DexClassVerifyMode  classVerifyMode;
    bool        generateRegisterMaps;
    RegisterMapMode     registerMapMode;
    bool        monitorVerification;
    bool        dexOptForSmp;
    /*
     * GC option flags.
     */
    bool        preciseGc;
    bool        preVerify;
    bool        postVerify;
    bool        concurrentMarkSweep;
    bool        verifyCardTable;
    bool        disableExplicitGc;
    int         assertionCtrlCount;
    AssertionControl*   assertionCtrl;
    ExecutionMode   executionMode;
    bool        commonInit; /* whether common stubs are generated */
    bool        constInit; /* whether global constants are initialized */
    /*
     * VM init management.
     */
    bool        initializing;
    bool        optimizing;
    /*
     * java.lang.System properties set from the command line with -D.
     * This is effectively a set, where later entries override earlier
     * ones.
     */
    std::vector<std::string>* properties;
    /*
     * Where the VM goes to find system classes.
     */
    ClassPathEntry* bootClassPath;
    /* used by the DEX optimizer to load classes from an unfinished DEX */
    DvmDex*     bootClassPathOptExtra;
    bool        optimizingBootstrapClass;
    /*
     * Loaded classes, hashed by class name.  Each entry is a ClassObject*,
     * allocated in GC space.
     */
    HashTable*  loadedClasses;
    /*
     * Value for the next class serial number to be assigned.  This is
     * incremented as we load classes.  Failed loads and races may result
     * in some numbers being skipped, and the serial number is not
     * guaranteed to start at 1, so the current value should not be used
     * as a count of loaded classes.
     */
    volatile int classSerialNumber;
    /*
     * Classes with a low classSerialNumber are probably in the zygote, and
     * their InitiatingLoaderList is not used, to promote sharing. The list is
     * kept here instead.
     */
    InitiatingLoaderList* initiatingLoaderList;
    /*
     * Interned strings.
     */
    /* A mutex that guards access to the interned string tables. */
    pthread_mutex_t internLock;
    /* Hash table of strings interned by the user. */
    HashTable*  internedStrings;
    /* Hash table of strings interned by the class loader. */
    HashTable*  literalStrings;
    /*
     * Classes constructed directly by the vm.
     */
    /* the class Class */
    ClassObject* classJavaLangClass;
    /* synthetic classes representing primitive types */
    ClassObject* typeVoid;
    ClassObject* typeBoolean;
    ClassObject* typeByte;
    ClassObject* typeShort;
    ClassObject* typeChar;
    ClassObject* typeInt;
    ClassObject* typeLong;
    ClassObject* typeFloat;
    ClassObject* typeDouble;
    /* synthetic classes for arrays of primitives */
    ClassObject* classArrayBoolean;
    ClassObject* classArrayByte;
    ClassObject* classArrayShort;
    ClassObject* classArrayChar;
    ClassObject* classArrayInt;
    ClassObject* classArrayLong;
    ClassObject* classArrayFloat;
    ClassObject* classArrayDouble;
    /*
     * Quick lookups for popular classes used internally.
     */
    ClassObject* classJavaLangClassArray;
    ClassObject* classJavaLangClassLoader;
    ClassObject* classJavaLangObject;
    ClassObject* classJavaLangObjectArray;
    ClassObject* classJavaLangString;
    ClassObject* classJavaLangThread;
    ClassObject* classJavaLangVMThread;
    ClassObject* classJavaLangThreadGroup;
    ClassObject* classJavaLangStackTraceElement;
    ClassObject* classJavaLangStackTraceElementArray;
    ClassObject* classJavaLangAnnotationAnnotationArray;
    ClassObject* classJavaLangAnnotationAnnotationArrayArray;
    ClassObject* classJavaLangReflectAccessibleObject;
    ClassObject* classJavaLangReflectConstructor;
    ClassObject* classJavaLangReflectConstructorArray;
    ClassObject* classJavaLangReflectField;
    ClassObject* classJavaLangReflectFieldArray;
    ClassObject* classJavaLangReflectMethod;
    ClassObject* classJavaLangReflectMethodArray;
    ClassObject* classJavaLangReflectProxy;
    ClassObject* classJavaLangSystem;
    ClassObject* classJavaNioDirectByteBuffer;
    ClassObject* classLibcoreReflectAnnotationFactory;
    ClassObject* classLibcoreReflectAnnotationMember;
    ClassObject* classLibcoreReflectAnnotationMemberArray;
    ClassObject* classOrgApacheHarmonyDalvikDdmcChunk;
    ClassObject* classOrgApacheHarmonyDalvikDdmcDdmServer;
    ClassObject* classJavaLangRefFinalizerReference;
    /*
     * classes representing exception types. The names here don't include
     * packages, just to keep the use sites a bit less verbose. All are
     * in java.lang, except where noted.
     */
    ClassObject* exAbstractMethodError;
    ClassObject* exArithmeticException;
    ClassObject* exArrayIndexOutOfBoundsException;
    ClassObject* exArrayStoreException;
    ClassObject* exClassCastException;
    ClassObject* exClassCircularityError;
    ClassObject* exClassFormatError;
    ClassObject* exClassNotFoundException;
    ClassObject* exError;
    ClassObject* exExceptionInInitializerError;
    ClassObject* exFileNotFoundException; /* in java.io */
    ClassObject* exIOException;           /* in java.io */
    ClassObject* exIllegalAccessError;
    ClassObject* exIllegalAccessException;
    ClassObject* exIllegalArgumentException;
    ClassObject* exIllegalMonitorStateException;
    ClassObject* exIllegalStateException;
    ClassObject* exIllegalThreadStateException;
    ClassObject* exIncompatibleClassChangeError;
    ClassObject* exInstantiationError;
    ClassObject* exInstantiationException;
    ClassObject* exInternalError;
    ClassObject* exInterruptedException;
    ClassObject* exLinkageError;
    ClassObject* exNegativeArraySizeException;
    ClassObject* exNoClassDefFoundError;
    ClassObject* exNoSuchFieldError;
    ClassObject* exNoSuchFieldException;
    ClassObject* exNoSuchMethodError;
    ClassObject* exNullPointerException;
    ClassObject* exOutOfMemoryError;
    ClassObject* exRuntimeException;
    ClassObject* exStackOverflowError;
    ClassObject* exStaleDexCacheError;    /* in dalvik.system */
    ClassObject* exStringIndexOutOfBoundsException;
    ClassObject* exThrowable;
    ClassObject* exTypeNotPresentException;
    ClassObject* exUnsatisfiedLinkError;
    ClassObject* exUnsupportedOperationException;
    ClassObject* exVerifyError;
    ClassObject* exVirtualMachineError;
    /* method offsets - Object */
    int         voffJavaLangObject_equals;
    int         voffJavaLangObject_hashCode;
    int         voffJavaLangObject_toString;
    /* field offsets - String */
    int         offJavaLangString_value;
    int         offJavaLangString_count;
    int         offJavaLangString_offset;
    int         offJavaLangString_hashCode;
    /* field offsets - Thread */
    int         offJavaLangThread_vmThread;
    int         offJavaLangThread_group;
    int         offJavaLangThread_daemon;
    int         offJavaLangThread_name;
    int         offJavaLangThread_priority;
    int         offJavaLangThread_uncaughtHandler;
    int         offJavaLangThread_contextClassLoader;
    /* method offsets - Thread */
    int         voffJavaLangThread_run;
    /* field offsets - ThreadGroup */
    int         offJavaLangThreadGroup_name;
    int         offJavaLangThreadGroup_parent;
    /* field offsets - VMThread */
    int         offJavaLangVMThread_thread;
    int         offJavaLangVMThread_vmData;
    /* method offsets - ThreadGroup */
    int         voffJavaLangThreadGroup_removeThread;
    /* field offsets - Throwable */
    int         offJavaLangThrowable_stackState;
    int         offJavaLangThrowable_cause;
    /* method offsets - ClassLoader */
    int         voffJavaLangClassLoader_loadClass;
    /* direct method pointers - ClassLoader */
    Method*     methJavaLangClassLoader_getSystemClassLoader;
    /* field offsets - java.lang.reflect.* */
    int         offJavaLangReflectConstructor_slot;
    int         offJavaLangReflectConstructor_declClass;
    int         offJavaLangReflectField_slot;
    int         offJavaLangReflectField_declClass;
    int         offJavaLangReflectMethod_slot;
    int         offJavaLangReflectMethod_declClass;
    /* field offsets - java.lang.ref.Reference */
    int         offJavaLangRefReference_referent;
    int         offJavaLangRefReference_queue;
    int         offJavaLangRefReference_queueNext;
    int         offJavaLangRefReference_pendingNext;
    /* field offsets - java.lang.ref.FinalizerReference */
    int offJavaLangRefFinalizerReference_zombie;
    /* method pointers - java.lang.ref.ReferenceQueue */
    Method* methJavaLangRefReferenceQueueAdd;
    /* method pointers - java.lang.ref.FinalizerReference */
    Method* methJavaLangRefFinalizerReferenceAdd;
    /* constructor method pointers; no vtable involved, so use Method* */
    Method*     methJavaLangStackTraceElement_init;
    Method*     methJavaLangReflectConstructor_init;
    Method*     methJavaLangReflectField_init;
    Method*     methJavaLangReflectMethod_init;
    Method*     methOrgApacheHarmonyLangAnnotationAnnotationMember_init;
    /* static method pointers - android.lang.annotation.* */
    Method*
        methOrgApacheHarmonyLangAnnotationAnnotationFactory_createAnnotation;
    /* direct method pointers - java.lang.reflect.Proxy */
    Method*     methJavaLangReflectProxy_constructorPrototype;
    /* field offsets - java.lang.reflect.Proxy */
    int         offJavaLangReflectProxy_h;
    /* direct method pointer - java.lang.System.runFinalization */
    Method*     methJavaLangSystem_runFinalization;
    /* field offsets - java.io.FileDescriptor */
    int         offJavaIoFileDescriptor_descriptor;
    /* direct method pointers - dalvik.system.NativeStart */
    Method*     methDalvikSystemNativeStart_main;
    Method*     methDalvikSystemNativeStart_run;
    /* assorted direct buffer helpers */
    Method*     methJavaNioDirectByteBuffer_init;
    int         offJavaNioBuffer_capacity;
    int         offJavaNioBuffer_effectiveDirectAddress;
    /* direct method pointers - org.apache.harmony.dalvik.ddmc.DdmServer */
    Method*     methDalvikDdmcServer_dispatch;
    Method*     methDalvikDdmcServer_broadcast;
    /* field offsets - org.apache.harmony.dalvik.ddmc.Chunk */
    int         offDalvikDdmcChunk_type;
    int         offDalvikDdmcChunk_data;
    int         offDalvikDdmcChunk_offset;
    int         offDalvikDdmcChunk_length;
    /*
     * Thread list.  This always has at least one element in it (main),
     * and main is always the first entry.
     *
     * The threadListLock is used for several things, including the thread
     * start condition variable.  Generally speaking, you must hold the
     * threadListLock when:
     *  - adding/removing items from the list
     *  - waiting on or signaling threadStartCond
     *  - examining the Thread struct for another thread (this is to avoid
     *    one thread freeing the Thread struct while another thread is
     *    perusing it)
     */
    Thread*     threadList;
    pthread_mutex_t threadListLock;
    pthread_cond_t threadStartCond;
    /*
     * The thread code grabs this before suspending all threads.  There
     * are a few things that can cause a "suspend all":
     *  (1) the GC is starting;
     *  (2) the debugger has sent a "suspend all" request;
     *  (3) a thread has hit a breakpoint or exception that the debugger
     *      has marked as a "suspend all" event;
     *  (4) the SignalCatcher caught a signal that requires suspension.
     *  (5) (if implemented) the JIT needs to perform a heavyweight
     *      rearrangement of the translation cache or JitTable.
     *
     * Because we use "safe point" self-suspension, it is never safe to
     * do a blocking "lock" call on this mutex -- if it has been acquired,
     * somebody is probably trying to put you to sleep.  The leading '_' is
     * intended as a reminder that this lock is special.
     */
    pthread_mutex_t _threadSuspendLock;
    /*
     * Guards Thread->suspendCount for all threads, and
     * provides the lock for the condition variable that all suspended threads
     * sleep on (threadSuspendCountCond).
     *
     * This has to be separate from threadListLock because of the way
     * threads put themselves to sleep.
     */
    pthread_mutex_t threadSuspendCountLock;
    /*
     * Suspended threads sleep on this.  They should sleep on the condition
     * variable until their "suspend count" is zero.
     *
     * Paired with "threadSuspendCountLock".
     */
    pthread_cond_t  threadSuspendCountCond;
    /*
     * Sum of all threads' suspendCount fields. Guarded by
     * threadSuspendCountLock.
     */
    int  sumThreadSuspendCount;
    /*
     * MUTEX ORDERING: when locking multiple mutexes, always grab them in
     * this order to avoid deadlock:
     *
     *  (1) _threadSuspendLock      (use lockThreadSuspend())
     *  (2) threadListLock          (use dvmLockThreadList())
     *  (3) threadSuspendCountLock  (use lockThreadSuspendCount())
     */
    /*
     * Thread ID bitmap.  We want threads to have small integer IDs so
     * we can use them in "thin locks".
     */
    BitVector*  threadIdMap;
    /*
     * Manage exit conditions.  The VM exits when all non-daemon threads
     * have exited.  If the main thread returns early, we need to sleep
     * on a condition variable.
     */
    int         nonDaemonThreadCount;   /* must hold threadListLock to access */
    pthread_cond_t  vmExitCond;
    /*
     * The set of DEX files loaded by custom class loaders.
     */
    HashTable*  userDexFiles;
    /*
     * JNI global reference table.
     */
    IndirectRefTable jniGlobalRefTable;
    IndirectRefTable jniWeakGlobalRefTable;
    pthread_mutex_t jniGlobalRefLock;
    pthread_mutex_t jniWeakGlobalRefLock;
    /*
     * JNI pinned object table (used for primitive arrays).
     */
    ReferenceTable  jniPinRefTable;
    pthread_mutex_t jniPinRefLock;
    /*
     * Native shared library table.
     */
    HashTable*  nativeLibs;
    /*
     * GC heap lock.  Functions like gcMalloc() acquire this before making
     * any changes to the heap.  It is held throughout garbage collection.
     */
    pthread_mutex_t gcHeapLock;
    /*
     * Condition variable to queue threads waiting to retry an
     * allocation.  Signaled after a concurrent GC is completed.
     */
    pthread_cond_t gcHeapCond;
    /* Opaque pointer representing the heap. */
    GcHeap*     gcHeap;
    /* The card table base, modified as needed for marking cards. */
    u1*         biasedCardTableBase;
    /*
     * Pre-allocated throwables.
     */
    Object*     outOfMemoryObj;
    Object*     internalErrorObj;
    Object*     noClassDefFoundErrorObj;
    /* Monitor list, so we can free them */
    /*volatile*/ Monitor* monitorList;
    /* Monitor for Thread.sleep() implementation */
    Monitor*    threadSleepMon;
    /* set when we create a second heap inside the zygote */
    bool        newZygoteHeapAllocated;
    /*
     * TLS keys.
     */
    pthread_key_t pthreadKeySelf;       /* Thread*, for dvmThreadSelf */
    /*
     * Cache results of "A instanceof B".
     */
    AtomicCache* instanceofCache;
    /* inline substitution table, used during optimization */
    InlineSub*          inlineSubs;
    /*
     * Bootstrap class loader linear allocator.
     */
    LinearAllocHdr* pBootLoaderAlloc;
    /*
     * Compute some stats on loaded classes.
     */
    int         numLoadedClasses;
    int         numDeclaredMethods;
    int         numDeclaredInstFields;
    int         numDeclaredStaticFields;
    /* when using a native debugger, set this to suppress watchdog timers */
    bool        nativeDebuggerActive;
    /*
     * JDWP debugger support.
     *
     * Note: Each thread will normally determine whether the debugger is active
     * for it by referring to its subMode flags.  "debuggerActive" here should be
     * seen as "debugger is making requests of 1 or more threads".
     */
    bool        debuggerConnected;      /* debugger or DDMS is connected */
    bool        debuggerActive;         /* debugger is making requests */
    JdwpState*  jdwpState;
    /*
     * Registry of objects known to the debugger.
     */
    HashTable*  dbgRegistry;
    /*
     * Debugger breakpoint table.
     */
    BreakpointSet*  breakpointSet;
    /*
     * Single-step control struct.  We currently only allow one thread to
     * be single-stepping at a time, which is all that really makes sense,
     * but it's possible we may need to expand this to be per-thread.
     */
    StepControl stepControl;
    /*
     * DDM features embedded in the VM.
     */
    bool        ddmThreadNotification;
    /*
     * Zygote (partially-started process) support
     */
    bool        zygote;
    /*
     * Used for tracking allocations that we report to DDMS.  When the feature
     * is enabled (through a DDMS request) the "allocRecords" pointer becomes
     * non-NULL.
     */
    pthread_mutex_t allocTrackerLock;
    AllocRecord*    allocRecords;
    int             allocRecordHead;        /* most-recently-added entry */
    int             allocRecordCount;       /* #of valid entries */
    int             allocRecordMax;         /* Number of allocated entries. */
    /*
     * When a profiler is enabled, this is incremented.  Distinct profilers
     * include "dmtrace" method tracing, emulator method tracing, and
     * possibly instruction counting.
     *
     * The purpose of this is to have a single value that shows whether any
     * profiling is going on.  Individual thread will normally check their
     * thread-private subMode flags to take any profiling action.
     */
    volatile int activeProfilers;
    /*
     * State for method-trace profiling.
     */
    MethodTraceState methodTrace;
    Method*     methodTraceGcMethod;
    Method*     methodTraceClassPrepMethod;
    /*
     * State for emulator tracing.
     */
    void*       emulatorTracePage;
    int         emulatorTraceEnableCount;
    /*
     * Global state for memory allocation profiling.
     */
    AllocProfState allocProf;
    /*
     * Pointers to the original methods for things that have been inlined.
     * This makes it easy for us to output method entry/exit records for
     * the method calls we're not actually making.  (Used by method
     * profiling.)
     */
    Method**    inlinedMethods;
    /*
     * Dalvik instruction counts (kNumPackedOpcodes entries).
     */
    int*        executedInstrCounts;
    int         instructionCountEnableCount;
    /*
     * Signal catcher thread (for SIGQUIT).
     */
    pthread_t   signalCatcherHandle;
    bool        haltSignalCatcher;
    /*
     * Stdout/stderr conversion thread.
     */
    bool            haltStdioConverter;
    bool            stdioConverterReady;
    pthread_t       stdioConverterHandle;
    pthread_mutex_t stdioConverterLock;
    pthread_cond_t  stdioConverterCond;
    int             stdoutPipe[2];
    int             stderrPipe[2];
    /*
     * pid of the system_server process. We track it so that when system server
     * crashes the Zygote process will be killed and restarted.
     */
    pid_t systemServerPid;
    int kernelGroupScheduling;
//#define COUNT_PRECISE_METHODS
#ifdef COUNT_PRECISE_METHODS
    PointerSet* preciseMethods;
#endif
    /* some RegisterMap statistics, useful during development */
    void*       registerMapStats;
#ifdef VERIFIER_STATS
    VerifierStats verifierStats;
#endif
    /* String pointed here will be deposited on the stack frame of dvmAbort */
    const char *lastMessage;
};
struct DvmGlobals_18 {
    /*
     * Some options from the command line or environment.
     */
    char*       bootClassPathStr;
    char*       classPathStr;
    size_t      heapStartingSize;
    size_t      heapMaximumSize;
    size_t      heapGrowthLimit;

    double      heapTargetUtilization;
    size_t      heapMinFree;
    size_t      heapMaxFree;
    size_t      stackSize;
    size_t      mainThreadStackSize;
    bool        verboseGc;
    bool        verboseJni;
    bool        verboseClass;
    bool        verboseShutdown;
    bool        jdwpAllowed;        // debugging allowed for this process?
    bool        jdwpConfigured;     // has debugging info been provided?
    JdwpTransportType jdwpTransport;
    bool        jdwpServer;
    char*       jdwpHost;
    int         jdwpPort;
    bool        jdwpSuspend;
    ProfilerClockSource profilerClockSource;
    /*
     * Lock profiling threshold value in milliseconds.  Acquires that
     * exceed threshold are logged.  Acquires within the threshold are
     * logged with a probability of $\frac{time}{threshold}$ .  If the
     * threshold is unset no additional logging occurs.
     */
    u4          lockProfThreshold;
    int         (*vfprintfHook)(FILE*, const char*, va_list);
    void        (*exitHook)(int);
    void        (*abortHook)(void);
    bool        (*isSensitiveThreadHook)(void);
    int         jniGrefLimit;       // 0 means no limit
    char*       jniTrace;
    bool        reduceSignals;
    bool        noQuitHandler;
    bool        verifyDexChecksum;
    char*       stackTraceFile;     // for SIGQUIT-inspired output
    bool        logStdio;
    DexOptimizerMode    dexOptMode;
    DexClassVerifyMode  classVerifyMode;
    bool        generateRegisterMaps;
    RegisterMapMode     registerMapMode;
    bool        monitorVerification;
    bool        dexOptForSmp;
    /*
     * GC option flags.
     */
    bool        preciseGc;
    bool        preVerify;
    bool        postVerify;
    bool        concurrentMarkSweep;
    bool        verifyCardTable;
    bool        disableExplicitGc;
    int         assertionCtrlCount;
    AssertionControl*   assertionCtrl;
    ExecutionMode   executionMode;
    bool        commonInit; /* whether common stubs are generated */
    bool        constInit; /* whether global constants are initialized */
    /*
     * VM init management.
     */
    bool        initializing;
    bool        optimizing;
    /*
     * java.lang.System properties set from the command line with -D.
     * This is effectively a set, where later entries override earlier
     * ones.
     */
    std::vector<std::string>* properties;
    /*
     * Where the VM goes to find system classes.
     */
    ClassPathEntry* bootClassPath;
    /* used by the DEX optimizer to load classes from an unfinished DEX */
    DvmDex*     bootClassPathOptExtra;
    bool        optimizingBootstrapClass;
    /*
     * Loaded classes, hashed by class name.  Each entry is a ClassObject*,
     * allocated in GC space.
     */
    HashTable*  loadedClasses;
    /*
     * Value for the next class serial number to be assigned.  This is
     * incremented as we load classes.  Failed loads and races may result
     * in some numbers being skipped, and the serial number is not
     * guaranteed to start at 1, so the current value should not be used
     * as a count of loaded classes.
     */
    volatile int classSerialNumber;
    /*
     * Classes with a low classSerialNumber are probably in the zygote, and
     * their InitiatingLoaderList is not used, to promote sharing. The list is
     * kept here instead.
     */
    InitiatingLoaderList* initiatingLoaderList;
    /*
     * Interned strings.
     */
    /* A mutex that guards access to the interned string tables. */
    pthread_mutex_t internLock;
    /* Hash table of strings interned by the user. */
    HashTable*  internedStrings;
    /* Hash table of strings interned by the class loader. */
    HashTable*  literalStrings;
    /*
     * Classes constructed directly by the vm.
     */
    /* the class Class */
    ClassObject* classJavaLangClass;
    /* synthetic classes representing primitive types */
    ClassObject* typeVoid;
    ClassObject* typeBoolean;
    ClassObject* typeByte;
    ClassObject* typeShort;
    ClassObject* typeChar;
    ClassObject* typeInt;
    ClassObject* typeLong;
    ClassObject* typeFloat;
    ClassObject* typeDouble;
    /* synthetic classes for arrays of primitives */
    ClassObject* classArrayBoolean;
    ClassObject* classArrayByte;
    ClassObject* classArrayShort;
    ClassObject* classArrayChar;
    ClassObject* classArrayInt;
    ClassObject* classArrayLong;
    ClassObject* classArrayFloat;
    ClassObject* classArrayDouble;
    /*
     * Quick lookups for popular classes used internally.
     */
    ClassObject* classJavaLangClassArray;
    ClassObject* classJavaLangClassLoader;
    ClassObject* classJavaLangObject;
    ClassObject* classJavaLangObjectArray;
    ClassObject* classJavaLangString;
    ClassObject* classJavaLangThread;
    ClassObject* classJavaLangVMThread;
    ClassObject* classJavaLangThreadGroup;
    ClassObject* classJavaLangStackTraceElement;
    ClassObject* classJavaLangStackTraceElementArray;
    ClassObject* classJavaLangAnnotationAnnotationArray;
    ClassObject* classJavaLangAnnotationAnnotationArrayArray;
    ClassObject* classJavaLangReflectAccessibleObject;
    ClassObject* classJavaLangReflectConstructor;
    ClassObject* classJavaLangReflectConstructorArray;
    ClassObject* classJavaLangReflectField;
    ClassObject* classJavaLangReflectFieldArray;
    ClassObject* classJavaLangReflectMethod;
    ClassObject* classJavaLangReflectMethodArray;
    ClassObject* classJavaLangReflectProxy;

    ClassObject* classJavaNioDirectByteBuffer;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationFactory;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationMember;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationMemberArray;
    ClassObject* classOrgApacheHarmonyDalvikDdmcChunk;
    ClassObject* classOrgApacheHarmonyDalvikDdmcDdmServer;
    ClassObject* classJavaLangRefFinalizerReference;
    /*
     * classes representing exception types. The names here don't include
     * packages, just to keep the use sites a bit less verbose. All are
     * in java.lang, except where noted.
     */
    ClassObject* exAbstractMethodError;
    ClassObject* exArithmeticException;
    ClassObject* exArrayIndexOutOfBoundsException;
    ClassObject* exArrayStoreException;
    ClassObject* exClassCastException;
    ClassObject* exClassCircularityError;
    ClassObject* exClassFormatError;
    ClassObject* exClassNotFoundException;
    ClassObject* exError;
    ClassObject* exExceptionInInitializerError;
    ClassObject* exFileNotFoundException; /* in java.io */
    ClassObject* exIOException;           /* in java.io */
    ClassObject* exIllegalAccessError;
    ClassObject* exIllegalAccessException;
    ClassObject* exIllegalArgumentException;
    ClassObject* exIllegalMonitorStateException;
    ClassObject* exIllegalStateException;
    ClassObject* exIllegalThreadStateException;
    ClassObject* exIncompatibleClassChangeError;
    ClassObject* exInstantiationError;
    ClassObject* exInstantiationException;
    ClassObject* exInternalError;
    ClassObject* exInterruptedException;
    ClassObject* exLinkageError;
    ClassObject* exNegativeArraySizeException;
    ClassObject* exNoClassDefFoundError;
    ClassObject* exNoSuchFieldError;
    ClassObject* exNoSuchFieldException;
    ClassObject* exNoSuchMethodError;
    ClassObject* exNullPointerException;
    ClassObject* exOutOfMemoryError;
    ClassObject* exRuntimeException;
    ClassObject* exStackOverflowError;
    ClassObject* exStaleDexCacheError;    /* in dalvik.system */
    ClassObject* exStringIndexOutOfBoundsException;
    ClassObject* exThrowable;
    ClassObject* exTypeNotPresentException;
    ClassObject* exUnsatisfiedLinkError;
    ClassObject* exUnsupportedOperationException;
    ClassObject* exVerifyError;
    ClassObject* exVirtualMachineError;
    /* method offsets - Object */
    int         voffJavaLangObject_equals;
    int         voffJavaLangObject_hashCode;
    int         voffJavaLangObject_toString;
    /* field offsets - String */
    int         offJavaLangString_value;
    int         offJavaLangString_count;
    int         offJavaLangString_offset;
    int         offJavaLangString_hashCode;
    /* field offsets - Thread */
    int         offJavaLangThread_vmThread;
    int         offJavaLangThread_group;
    int         offJavaLangThread_daemon;
    int         offJavaLangThread_name;
    int         offJavaLangThread_priority;
    int         offJavaLangThread_uncaughtHandler;
    int         offJavaLangThread_contextClassLoader;
    /* method offsets - Thread */
    int         voffJavaLangThread_run;
    /* field offsets - ThreadGroup */
    int         offJavaLangThreadGroup_name;
    int         offJavaLangThreadGroup_parent;
    /* field offsets - VMThread */
    int         offJavaLangVMThread_thread;
    int         offJavaLangVMThread_vmData;
    /* method offsets - ThreadGroup */
    int         voffJavaLangThreadGroup_removeThread;
    /* field offsets - Throwable */
    int         offJavaLangThrowable_stackState;
    int         offJavaLangThrowable_cause;
    /* method offsets - ClassLoader */
    int         voffJavaLangClassLoader_loadClass;
    /* direct method pointers - ClassLoader */
    Method*     methJavaLangClassLoader_getSystemClassLoader;
    /* field offsets - java.lang.reflect.* */
    int         offJavaLangReflectConstructor_slot;
    int         offJavaLangReflectConstructor_declClass;
    int         offJavaLangReflectField_slot;
    int         offJavaLangReflectField_declClass;
    int         offJavaLangReflectMethod_slot;
    int         offJavaLangReflectMethod_declClass;
    /* field offsets - java.lang.ref.Reference */
    int         offJavaLangRefReference_referent;
    int         offJavaLangRefReference_queue;
    int         offJavaLangRefReference_queueNext;
    int         offJavaLangRefReference_pendingNext;
    /* field offsets - java.lang.ref.FinalizerReference */
    int offJavaLangRefFinalizerReference_zombie;
    /* method pointers - java.lang.ref.ReferenceQueue */
    Method* methJavaLangRefReferenceQueueAdd;
    /* method pointers - java.lang.ref.FinalizerReference */
    Method* methJavaLangRefFinalizerReferenceAdd;
    /* constructor method pointers; no vtable involved, so use Method* */
    Method*     methJavaLangStackTraceElement_init;
    Method*     methJavaLangReflectConstructor_init;
    Method*     methJavaLangReflectField_init;
    Method*     methJavaLangReflectMethod_init;
    Method*     methOrgApacheHarmonyLangAnnotationAnnotationMember_init;
    /* static method pointers - android.lang.annotation.* */
    Method*
        methOrgApacheHarmonyLangAnnotationAnnotationFactory_createAnnotation;
    /* direct method pointers - java.lang.reflect.Proxy */
    Method*     methJavaLangReflectProxy_constructorPrototype;
    /* field offsets - java.lang.reflect.Proxy */
    int         offJavaLangReflectProxy_h;


    /* field offsets - java.io.FileDescriptor */
    int         offJavaIoFileDescriptor_descriptor;
    /* direct method pointers - dalvik.system.NativeStart */
    Method*     methDalvikSystemNativeStart_main;
    Method*     methDalvikSystemNativeStart_run;
    /* assorted direct buffer helpers */
    Method*     methJavaNioDirectByteBuffer_init;
    int         offJavaNioBuffer_capacity;
    int         offJavaNioBuffer_effectiveDirectAddress;
    /* direct method pointers - org.apache.harmony.dalvik.ddmc.DdmServer */
    Method*     methDalvikDdmcServer_dispatch;
    Method*     methDalvikDdmcServer_broadcast;
    /* field offsets - org.apache.harmony.dalvik.ddmc.Chunk */
    int         offDalvikDdmcChunk_type;
    int         offDalvikDdmcChunk_data;
    int         offDalvikDdmcChunk_offset;
    int         offDalvikDdmcChunk_length;
    /*
     * Thread list.  This always has at least one element in it (main),
     * and main is always the first entry.
     *
     * The threadListLock is used for several things, including the thread
     * start condition variable.  Generally speaking, you must hold the
     * threadListLock when:
     *  - adding/removing items from the list
     *  - waiting on or signaling threadStartCond
     *  - examining the Thread struct for another thread (this is to avoid
     *    one thread freeing the Thread struct while another thread is
     *    perusing it)
     */
    Thread*     threadList;
    pthread_mutex_t threadListLock;
    pthread_cond_t threadStartCond;
    /*
     * The thread code grabs this before suspending all threads.  There
     * are a few things that can cause a "suspend all":
     *  (1) the GC is starting;
     *  (2) the debugger has sent a "suspend all" request;
     *  (3) a thread has hit a breakpoint or exception that the debugger
     *      has marked as a "suspend all" event;
     *  (4) the SignalCatcher caught a signal that requires suspension.
     *  (5) (if implemented) the JIT needs to perform a heavyweight
     *      rearrangement of the translation cache or JitTable.
     *
     * Because we use "safe point" self-suspension, it is never safe to
     * do a blocking "lock" call on this mutex -- if it has been acquired,
     * somebody is probably trying to put you to sleep.  The leading '_' is
     * intended as a reminder that this lock is special.
     */
    pthread_mutex_t _threadSuspendLock;
    /*
     * Guards Thread->suspendCount for all threads, and
     * provides the lock for the condition variable that all suspended threads
     * sleep on (threadSuspendCountCond).
     *
     * This has to be separate from threadListLock because of the way
     * threads put themselves to sleep.
     */
    pthread_mutex_t threadSuspendCountLock;
    /*
     * Suspended threads sleep on this.  They should sleep on the condition
     * variable until their "suspend count" is zero.
     *
     * Paired with "threadSuspendCountLock".
     */
    pthread_cond_t  threadSuspendCountCond;
    /*
     * Sum of all threads' suspendCount fields. Guarded by
     * threadSuspendCountLock.
     */
    int  sumThreadSuspendCount;
    /*
     * MUTEX ORDERING: when locking multiple mutexes, always grab them in
     * this order to avoid deadlock:
     *
     *  (1) _threadSuspendLock      (use lockThreadSuspend())
     *  (2) threadListLock          (use dvmLockThreadList())
     *  (3) threadSuspendCountLock  (use lockThreadSuspendCount())
     */
    /*
     * Thread ID bitmap.  We want threads to have small integer IDs so
     * we can use them in "thin locks".
     */
    BitVector*  threadIdMap;
    /*
     * Manage exit conditions.  The VM exits when all non-daemon threads
     * have exited.  If the main thread returns early, we need to sleep
     * on a condition variable.
     */
    int         nonDaemonThreadCount;   /* must hold threadListLock to access */
    pthread_cond_t  vmExitCond;
    /*
     * The set of DEX files loaded by custom class loaders.
     */
    HashTable*  userDexFiles;
    /*
     * JNI global reference table.
     */
    IndirectRefTable jniGlobalRefTable;
    IndirectRefTable jniWeakGlobalRefTable;
    pthread_mutex_t jniGlobalRefLock;
    pthread_mutex_t jniWeakGlobalRefLock;
    int         jniGlobalRefHiMark;
    int         jniGlobalRefLoMark;
    /*
     * JNI pinned object table (used for primitive arrays).
     */
    ReferenceTable  jniPinRefTable;
    pthread_mutex_t jniPinRefLock;
    /*
     * Native shared library table.
     */
    HashTable*  nativeLibs;
    /*
     * GC heap lock.  Functions like gcMalloc() acquire this before making
     * any changes to the heap.  It is held throughout garbage collection.
     */
    pthread_mutex_t gcHeapLock;
    /*
     * Condition variable to queue threads waiting to retry an
     * allocation.  Signaled after a concurrent GC is completed.
     */
    pthread_cond_t gcHeapCond;
    /* Opaque pointer representing the heap. */
    GcHeap*     gcHeap;
    /* The card table base, modified as needed for marking cards. */
    u1*         biasedCardTableBase;
    /*
     * Pre-allocated throwables.
     */
    Object*     outOfMemoryObj;
    Object*     internalErrorObj;
    Object*     noClassDefFoundErrorObj;
    /* Monitor list, so we can free them */
    /*volatile*/ Monitor* monitorList;
    /* Monitor for Thread.sleep() implementation */
    Monitor*    threadSleepMon;
    /* set when we create a second heap inside the zygote */
    bool        newZygoteHeapAllocated;
    /*
     * TLS keys.
     */
    pthread_key_t pthreadKeySelf;       /* Thread*, for dvmThreadSelf */
    /*
     * Cache results of "A instanceof B".
     */
    AtomicCache* instanceofCache;
    /* inline substitution table, used during optimization */
    InlineSub*          inlineSubs;
    /*
     * Bootstrap class loader linear allocator.
     */
    LinearAllocHdr* pBootLoaderAlloc;
    /*
     * Compute some stats on loaded classes.
     */
    int         numLoadedClasses;
    int         numDeclaredMethods;
    int         numDeclaredInstFields;
    int         numDeclaredStaticFields;
    /* when using a native debugger, set this to suppress watchdog timers */
    bool        nativeDebuggerActive;
    /*
     * JDWP debugger support.
     *
     * Note: Each thread will normally determine whether the debugger is active
     * for it by referring to its subMode flags.  "debuggerActive" here should be
     * seen as "debugger is making requests of 1 or more threads".
     */
    bool        debuggerConnected;      /* debugger or DDMS is connected */
    bool        debuggerActive;         /* debugger is making requests */
    JdwpState*  jdwpState;
    /*
     * Registry of objects known to the debugger.
     */
    HashTable*  dbgRegistry;
    /*
     * Debugger breakpoint table.
     */
    BreakpointSet*  breakpointSet;
    /*
     * Single-step control struct.  We currently only allow one thread to
     * be single-stepping at a time, which is all that really makes sense,
     * but it's possible we may need to expand this to be per-thread.
     */
    StepControl stepControl;
    /*
     * DDM features embedded in the VM.
     */
    bool        ddmThreadNotification;
    /*
     * Zygote (partially-started process) support
     */
    bool        zygote;
    /*
     * Used for tracking allocations that we report to DDMS.  When the feature
     * is enabled (through a DDMS request) the "allocRecords" pointer becomes
     * non-NULL.
     */
    pthread_mutex_t allocTrackerLock;
    AllocRecord*    allocRecords;
    int             allocRecordHead;        /* most-recently-added entry */
    int             allocRecordCount;       /* #of valid entries */

    /*
     * When a profiler is enabled, this is incremented.  Distinct profilers
     * include "dmtrace" method tracing, emulator method tracing, and
     * possibly instruction counting.
     *
     * The purpose of this is to have a single value that shows whether any
     * profiling is going on.  Individual thread will normally check their
     * thread-private subMode flags to take any profiling action.
     */
    volatile int activeProfilers;
    /*
     * State for method-trace profiling.
     */
    MethodTraceState methodTrace;
    Method*     methodTraceGcMethod;
    Method*     methodTraceClassPrepMethod;
    /*
     * State for emulator tracing.
     */
    void*       emulatorTracePage;
    int         emulatorTraceEnableCount;
    /*
     * Global state for memory allocation profiling.
     */
    AllocProfState allocProf;
    /*
     * Pointers to the original methods for things that have been inlined.
     * This makes it easy for us to output method entry/exit records for
     * the method calls we're not actually making.  (Used by method
     * profiling.)
     */
    Method**    inlinedMethods;
    /*
     * Dalvik instruction counts (kNumPackedOpcodes entries).
     */
    int*        executedInstrCounts;
    int         instructionCountEnableCount;
    /*
     * Signal catcher thread (for SIGQUIT).
     */
    pthread_t   signalCatcherHandle;
    bool        haltSignalCatcher;
    /*
     * Stdout/stderr conversion thread.
     */
    bool            haltStdioConverter;
    bool            stdioConverterReady;
    pthread_t       stdioConverterHandle;
    pthread_mutex_t stdioConverterLock;
    pthread_cond_t  stdioConverterCond;
    int             stdoutPipe[2];
    int             stderrPipe[2];
    /*
     * pid of the system_server process. We track it so that when system server
     * crashes the Zygote process will be killed and restarted.
     */
    pid_t systemServerPid;
    int kernelGroupScheduling;
//#define COUNT_PRECISE_METHODS
#ifdef COUNT_PRECISE_METHODS
    PointerSet* preciseMethods;
#endif
    /* some RegisterMap statistics, useful during development */
    void*       registerMapStats;
#ifdef VERIFIER_STATS
    VerifierStats verifierStats;
#endif
    /* String pointed here will be deposited on the stack frame of dvmAbort */
    const char *lastMessage;
};

struct DvmGlobals_17 {
    /*
     * Some options from the command line or environment.
     */
    char*       bootClassPathStr;
    char*       classPathStr;
    size_t      heapStartingSize;
    size_t      heapMaximumSize;
    size_t      heapGrowthLimit;
    double      heapTargetUtilization;
    size_t      heapMinFree;
    size_t      heapMaxFree;
    size_t      stackSize;
    size_t      mainThreadStackSize;
    bool        verboseGc;
    bool        verboseJni;
    bool        verboseClass;
    bool        verboseShutdown;
    bool        jdwpAllowed;        // debugging allowed for this process?
    bool        jdwpConfigured;     // has debugging info been provided?
    JdwpTransportType jdwpTransport;
    bool        jdwpServer;
    char*       jdwpHost;
    int         jdwpPort;
    bool        jdwpSuspend;
    ProfilerClockSource profilerClockSource;
    /*
     * Lock profiling threshold value in milliseconds.  Acquires that
     * exceed threshold are logged.  Acquires within the threshold are
     * logged with a probability of $\frac{time}{threshold}$ .  If the
     * threshold is unset no additional logging occurs.
     */
    u4          lockProfThreshold;
    int         (*vfprintfHook)(FILE*, const char*, va_list);
    void        (*exitHook)(int);
    void        (*abortHook)(void);
    bool        (*isSensitiveThreadHook)(void);
    int         jniGrefLimit;       // 0 means no limit
    char*       jniTrace;
    bool        reduceSignals;
    bool        noQuitHandler;
    bool        verifyDexChecksum;
    char*       stackTraceFile;     // for SIGQUIT-inspired output
    bool        logStdio;
    DexOptimizerMode    dexOptMode;
    DexClassVerifyMode  classVerifyMode;
    bool        generateRegisterMaps;
    RegisterMapMode     registerMapMode;
    bool        monitorVerification;
    bool        dexOptForSmp;
    /*
     * GC option flags.
     */
    bool        preciseGc;
    bool        preVerify;
    bool        postVerify;
    bool        concurrentMarkSweep;
    bool        verifyCardTable;
    bool        disableExplicitGc;
    int         assertionCtrlCount;
    AssertionControl*   assertionCtrl;
    ExecutionMode   executionMode;
    bool        commonInit; /* whether common stubs are generated */
    bool        constInit; /* whether global constants are initialized */
    /*
     * VM init management.
     */
    bool        initializing;
    bool        optimizing;
    /*
     * java.lang.System properties set from the command line with -D.
     * This is effectively a set, where later entries override earlier
     * ones.
     */
    std::vector<std::string>* properties;
    /*
     * Where the VM goes to find system classes.
     */
    ClassPathEntry* bootClassPath;
    /* used by the DEX optimizer to load classes from an unfinished DEX */
    DvmDex*     bootClassPathOptExtra;
    bool        optimizingBootstrapClass;
    /*
     * Loaded classes, hashed by class name.  Each entry is a ClassObject*,
     * allocated in GC space.
     */
    HashTable*  loadedClasses;
    /*
     * Value for the next class serial number to be assigned.  This is
     * incremented as we load classes.  Failed loads and races may result
     * in some numbers being skipped, and the serial number is not
     * guaranteed to start at 1, so the current value should not be used
     * as a count of loaded classes.
     */
    volatile int classSerialNumber;
    /*
     * Classes with a low classSerialNumber are probably in the zygote, and
     * their InitiatingLoaderList is not used, to promote sharing. The list is
     * kept here instead.
     */
    InitiatingLoaderList* initiatingLoaderList;
    /*
     * Interned strings.
     */
    /* A mutex that guards access to the interned string tables. */
    pthread_mutex_t internLock;
    /* Hash table of strings interned by the user. */
    HashTable*  internedStrings;
    /* Hash table of strings interned by the class loader. */
    HashTable*  literalStrings;
    /*
     * Classes constructed directly by the vm.
     */
    /* the class Class */
    ClassObject* classJavaLangClass;
    /* synthetic classes representing primitive types */
    ClassObject* typeVoid;
    ClassObject* typeBoolean;
    ClassObject* typeByte;
    ClassObject* typeShort;
    ClassObject* typeChar;
    ClassObject* typeInt;
    ClassObject* typeLong;
    ClassObject* typeFloat;
    ClassObject* typeDouble;
    /* synthetic classes for arrays of primitives */
    ClassObject* classArrayBoolean;
    ClassObject* classArrayByte;
    ClassObject* classArrayShort;
    ClassObject* classArrayChar;
    ClassObject* classArrayInt;
    ClassObject* classArrayLong;
    ClassObject* classArrayFloat;
    ClassObject* classArrayDouble;
    /*
     * Quick lookups for popular classes used internally.
     */
    ClassObject* classJavaLangClassArray;
    ClassObject* classJavaLangClassLoader;
    ClassObject* classJavaLangObject;
    ClassObject* classJavaLangObjectArray;
    ClassObject* classJavaLangString;
    ClassObject* classJavaLangThread;
    ClassObject* classJavaLangVMThread;
    ClassObject* classJavaLangThreadGroup;
    ClassObject* classJavaLangStackTraceElement;
    ClassObject* classJavaLangStackTraceElementArray;
    ClassObject* classJavaLangAnnotationAnnotationArray;
    ClassObject* classJavaLangAnnotationAnnotationArrayArray;
    ClassObject* classJavaLangReflectAccessibleObject;
    ClassObject* classJavaLangReflectConstructor;
    ClassObject* classJavaLangReflectConstructorArray;
    ClassObject* classJavaLangReflectField;
    ClassObject* classJavaLangReflectFieldArray;
    ClassObject* classJavaLangReflectMethod;
    ClassObject* classJavaLangReflectMethodArray;
    ClassObject* classJavaLangReflectProxy;
    ClassObject* classJavaNioReadWriteDirectByteBuffer;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationFactory;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationMember;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationMemberArray;
    ClassObject* classOrgApacheHarmonyDalvikDdmcChunk;
    ClassObject* classOrgApacheHarmonyDalvikDdmcDdmServer;
    ClassObject* classJavaLangRefFinalizerReference;
    /*
     * classes representing exception types. The names here don't include
     * packages, just to keep the use sites a bit less verbose. All are
     * in java.lang, except where noted.
     */
    ClassObject* exAbstractMethodError;
    ClassObject* exArithmeticException;
    ClassObject* exArrayIndexOutOfBoundsException;
    ClassObject* exArrayStoreException;
    ClassObject* exClassCastException;
    ClassObject* exClassCircularityError;
    ClassObject* exClassFormatError;
    ClassObject* exClassNotFoundException;
    ClassObject* exError;
    ClassObject* exExceptionInInitializerError;
    ClassObject* exFileNotFoundException; /* in java.io */
    ClassObject* exIOException;           /* in java.io */
    ClassObject* exIllegalAccessError;
    ClassObject* exIllegalAccessException;
    ClassObject* exIllegalArgumentException;
    ClassObject* exIllegalMonitorStateException;
    ClassObject* exIllegalStateException;
    ClassObject* exIllegalThreadStateException;
    ClassObject* exIncompatibleClassChangeError;
    ClassObject* exInstantiationError;
    ClassObject* exInstantiationException;
    ClassObject* exInternalError;
    ClassObject* exInterruptedException;
    ClassObject* exLinkageError;
    ClassObject* exNegativeArraySizeException;
    ClassObject* exNoClassDefFoundError;
    ClassObject* exNoSuchFieldError;
    ClassObject* exNoSuchFieldException;
    ClassObject* exNoSuchMethodError;
    ClassObject* exNullPointerException;
    ClassObject* exOutOfMemoryError;
    ClassObject* exRuntimeException;
    ClassObject* exStackOverflowError;
    ClassObject* exStaleDexCacheError;    /* in dalvik.system */
    ClassObject* exStringIndexOutOfBoundsException;
    ClassObject* exThrowable;
    ClassObject* exTypeNotPresentException;
    ClassObject* exUnsatisfiedLinkError;
    ClassObject* exUnsupportedOperationException;
    ClassObject* exVerifyError;
    ClassObject* exVirtualMachineError;
    /* method offsets - Object */
    int         voffJavaLangObject_equals;
    int         voffJavaLangObject_hashCode;
    int         voffJavaLangObject_toString;
    /* field offsets - String */
    int         offJavaLangString_value;
    int         offJavaLangString_count;
    int         offJavaLangString_offset;
    int         offJavaLangString_hashCode;
    /* field offsets - Thread */
    int         offJavaLangThread_vmThread;
    int         offJavaLangThread_group;
    int         offJavaLangThread_daemon;
    int         offJavaLangThread_name;
    int         offJavaLangThread_priority;
    int         offJavaLangThread_uncaughtHandler;
    int         offJavaLangThread_contextClassLoader;
    /* method offsets - Thread */
    int         voffJavaLangThread_run;
    /* field offsets - ThreadGroup */
    int         offJavaLangThreadGroup_name;
    int         offJavaLangThreadGroup_parent;
    /* field offsets - VMThread */
    int         offJavaLangVMThread_thread;
    int         offJavaLangVMThread_vmData;
    /* method offsets - ThreadGroup */
    int         voffJavaLangThreadGroup_removeThread;
    /* field offsets - Throwable */
    int         offJavaLangThrowable_stackState;
    int         offJavaLangThrowable_cause;
    /* method offsets - ClassLoader */
    int         voffJavaLangClassLoader_loadClass;
    /* direct method pointers - ClassLoader */
    Method*     methJavaLangClassLoader_getSystemClassLoader;
    /* field offsets - java.lang.reflect.* */
    int         offJavaLangReflectConstructor_slot;
    int         offJavaLangReflectConstructor_declClass;
    int         offJavaLangReflectField_slot;
    int         offJavaLangReflectField_declClass;
    int         offJavaLangReflectMethod_slot;
    int         offJavaLangReflectMethod_declClass;
    /* field offsets - java.lang.ref.Reference */
    int         offJavaLangRefReference_referent;
    int         offJavaLangRefReference_queue;
    int         offJavaLangRefReference_queueNext;
    int         offJavaLangRefReference_pendingNext;
    /* field offsets - java.lang.ref.FinalizerReference */
    int offJavaLangRefFinalizerReference_zombie;
    /* method pointers - java.lang.ref.ReferenceQueue */
    Method* methJavaLangRefReferenceQueueAdd;
    /* method pointers - java.lang.ref.FinalizerReference */
    Method* methJavaLangRefFinalizerReferenceAdd;
    /* constructor method pointers; no vtable involved, so use Method* */
    Method*     methJavaLangStackTraceElement_init;
    Method*     methJavaLangReflectConstructor_init;
    Method*     methJavaLangReflectField_init;
    Method*     methJavaLangReflectMethod_init;
    Method*     methOrgApacheHarmonyLangAnnotationAnnotationMember_init;
    /* static method pointers - android.lang.annotation.* */
    Method*
        methOrgApacheHarmonyLangAnnotationAnnotationFactory_createAnnotation;
    /* direct method pointers - java.lang.reflect.Proxy */
    Method*     methJavaLangReflectProxy_constructorPrototype;
    /* field offsets - java.lang.reflect.Proxy */
    int         offJavaLangReflectProxy_h;
    /* field offsets - java.io.FileDescriptor */
    int         offJavaIoFileDescriptor_descriptor;
    /* direct method pointers - dalvik.system.NativeStart */
    Method*     methDalvikSystemNativeStart_main;
    Method*     methDalvikSystemNativeStart_run;
    /* assorted direct buffer helpers */
    Method*     methJavaNioReadWriteDirectByteBuffer_init;
    int         offJavaNioBuffer_capacity;
    int         offJavaNioBuffer_effectiveDirectAddress;
    /* direct method pointers - org.apache.harmony.dalvik.ddmc.DdmServer */
    Method*     methDalvikDdmcServer_dispatch;
    Method*     methDalvikDdmcServer_broadcast;
    /* field offsets - org.apache.harmony.dalvik.ddmc.Chunk */
    int         offDalvikDdmcChunk_type;
    int         offDalvikDdmcChunk_data;
    int         offDalvikDdmcChunk_offset;
    int         offDalvikDdmcChunk_length;
    /*
     * Thread list.  This always has at least one element in it (main),
     * and main is always the first entry.
     *
     * The threadListLock is used for several things, including the thread
     * start condition variable.  Generally speaking, you must hold the
     * threadListLock when:
     *  - adding/removing items from the list
     *  - waiting on or signaling threadStartCond
     *  - examining the Thread struct for another thread (this is to avoid
     *    one thread freeing the Thread struct while another thread is
     *    perusing it)
     */
    Thread*     threadList;
    pthread_mutex_t threadListLock;
    pthread_cond_t threadStartCond;
    /*
     * The thread code grabs this before suspending all threads.  There
     * are a few things that can cause a "suspend all":
     *  (1) the GC is starting;
     *  (2) the debugger has sent a "suspend all" request;
     *  (3) a thread has hit a breakpoint or exception that the debugger
     *      has marked as a "suspend all" event;
     *  (4) the SignalCatcher caught a signal that requires suspension.
     *  (5) (if implemented) the JIT needs to perform a heavyweight
     *      rearrangement of the translation cache or JitTable.
     *
     * Because we use "safe point" self-suspension, it is never safe to
     * do a blocking "lock" call on this mutex -- if it has been acquired,
     * somebody is probably trying to put you to sleep.  The leading '_' is
     * intended as a reminder that this lock is special.
     */
    pthread_mutex_t _threadSuspendLock;
    /*
     * Guards Thread->suspendCount for all threads, and
     * provides the lock for the condition variable that all suspended threads
     * sleep on (threadSuspendCountCond).
     *
     * This has to be separate from threadListLock because of the way
     * threads put themselves to sleep.
     */
    pthread_mutex_t threadSuspendCountLock;
    /*
     * Suspended threads sleep on this.  They should sleep on the condition
     * variable until their "suspend count" is zero.
     *
     * Paired with "threadSuspendCountLock".
     */
    pthread_cond_t  threadSuspendCountCond;
    /*
     * Sum of all threads' suspendCount fields. Guarded by
     * threadSuspendCountLock.
     */
    int  sumThreadSuspendCount;
    /*
     * MUTEX ORDERING: when locking multiple mutexes, always grab them in
     * this order to avoid deadlock:
     *
     *  (1) _threadSuspendLock      (use lockThreadSuspend())
     *  (2) threadListLock          (use dvmLockThreadList())
     *  (3) threadSuspendCountLock  (use lockThreadSuspendCount())
     */
    /*
     * Thread ID bitmap.  We want threads to have small integer IDs so
     * we can use them in "thin locks".
     */
    BitVector*  threadIdMap;
    /*
     * Manage exit conditions.  The VM exits when all non-daemon threads
     * have exited.  If the main thread returns early, we need to sleep
     * on a condition variable.
     */
    int         nonDaemonThreadCount;   /* must hold threadListLock to access */
    pthread_cond_t  vmExitCond;
    /*
     * The set of DEX files loaded by custom class loaders.
     */
    HashTable*  userDexFiles;
    /*
     * JNI global reference table.
     */
    IndirectRefTable jniGlobalRefTable;
    IndirectRefTable jniWeakGlobalRefTable;
    pthread_mutex_t jniGlobalRefLock;
    pthread_mutex_t jniWeakGlobalRefLock;
    int         jniGlobalRefHiMark;
    int         jniGlobalRefLoMark;
    /*
     * JNI pinned object table (used for primitive arrays).
     */
    ReferenceTable  jniPinRefTable;
    pthread_mutex_t jniPinRefLock;
    /*
     * Native shared library table.
     */
    HashTable*  nativeLibs;
    /*
     * GC heap lock.  Functions like gcMalloc() acquire this before making
     * any changes to the heap.  It is held throughout garbage collection.
     */
    pthread_mutex_t gcHeapLock;
    /*
     * Condition variable to queue threads waiting to retry an
     * allocation.  Signaled after a concurrent GC is completed.
     */
    pthread_cond_t gcHeapCond;
    /* Opaque pointer representing the heap. */
    GcHeap*     gcHeap;
    /* The card table base, modified as needed for marking cards. */
    u1*         biasedCardTableBase;
    /*
     * Pre-allocated throwables.
     */
    Object*     outOfMemoryObj;
    Object*     internalErrorObj;
    Object*     noClassDefFoundErrorObj;
    /* Monitor list, so we can free them */
    /*volatile*/ Monitor* monitorList;
    /* Monitor for Thread.sleep() implementation */
    Monitor*    threadSleepMon;
    /* set when we create a second heap inside the zygote */
    bool        newZygoteHeapAllocated;
    /*
     * TLS keys.
     */
    pthread_key_t pthreadKeySelf;       /* Thread*, for dvmThreadSelf */
    /*
     * Cache results of "A instanceof B".
     */
    AtomicCache* instanceofCache;
    /* inline substitution table, used during optimization */
    InlineSub*          inlineSubs;
    /*
     * Bootstrap class loader linear allocator.
     */
    LinearAllocHdr* pBootLoaderAlloc;
    /*
     * Compute some stats on loaded classes.
     */
    int         numLoadedClasses;
    int         numDeclaredMethods;
    int         numDeclaredInstFields;
    int         numDeclaredStaticFields;
    /* when using a native debugger, set this to suppress watchdog timers */
    bool        nativeDebuggerActive;
    /*
     * JDWP debugger support.
     *
     * Note: Each thread will normally determine whether the debugger is active
     * for it by referring to its subMode flags.  "debuggerActive" here should be
     * seen as "debugger is making requests of 1 or more threads".
     */
    bool        debuggerConnected;      /* debugger or DDMS is connected */
    bool        debuggerActive;         /* debugger is making requests */
    JdwpState*  jdwpState;
    /*
     * Registry of objects known to the debugger.
     */
    HashTable*  dbgRegistry;
    /*
     * Debugger breakpoint table.
     */
    BreakpointSet*  breakpointSet;
    /*
     * Single-step control struct.  We currently only allow one thread to
     * be single-stepping at a time, which is all that really makes sense,
     * but it's possible we may need to expand this to be per-thread.
     */
    StepControl stepControl;
    /*
     * DDM features embedded in the VM.
     */
    bool        ddmThreadNotification;
    /*
     * Zygote (partially-started process) support
     */
    bool        zygote;
    /*
     * Used for tracking allocations that we report to DDMS.  When the feature
     * is enabled (through a DDMS request) the "allocRecords" pointer becomes
     * non-NULL.
     */
    pthread_mutex_t allocTrackerLock;
    AllocRecord*    allocRecords;
    int             allocRecordHead;        /* most-recently-added entry */
    int             allocRecordCount;       /* #of valid entries */
    /*
     * When a profiler is enabled, this is incremented.  Distinct profilers
     * include "dmtrace" method tracing, emulator method tracing, and
     * possibly instruction counting.
     *
     * The purpose of this is to have a single value that shows whether any
     * profiling is going on.  Individual thread will normally check their
     * thread-private subMode flags to take any profiling action.
     */
    volatile int activeProfilers;
    /*
     * State for method-trace profiling.
     */
    MethodTraceState methodTrace;
    Method*     methodTraceGcMethod;
    Method*     methodTraceClassPrepMethod;
    /*
     * State for emulator tracing.
     */
    void*       emulatorTracePage;
    int         emulatorTraceEnableCount;
    /*
     * Global state for memory allocation profiling.
     */
    AllocProfState allocProf;
    /*
     * Pointers to the original methods for things that have been inlined.
     * This makes it easy for us to output method entry/exit records for
     * the method calls we're not actually making.  (Used by method
     * profiling.)
     */
    Method**    inlinedMethods;
    /*
     * Dalvik instruction counts (kNumPackedOpcodes entries).
     */
    int*        executedInstrCounts;
    int         instructionCountEnableCount;
    /*
     * Signal catcher thread (for SIGQUIT).
     */
    pthread_t   signalCatcherHandle;
    bool        haltSignalCatcher;
    /*
     * Stdout/stderr conversion thread.
     */
    bool            haltStdioConverter;
    bool            stdioConverterReady;
    pthread_t       stdioConverterHandle;
    pthread_mutex_t stdioConverterLock;
    pthread_cond_t  stdioConverterCond;
    int             stdoutPipe[2];
    int             stderrPipe[2];
    /*
     * pid of the system_server process. We track it so that when system server
     * crashes the Zygote process will be killed and restarted.
     */
    pid_t systemServerPid;
    int kernelGroupScheduling;
//#define COUNT_PRECISE_METHODS
#ifdef COUNT_PRECISE_METHODS
    PointerSet* preciseMethods;
#endif
    /* some RegisterMap statistics, useful during development */
    void*       registerMapStats;
#ifdef VERIFIER_STATS
    VerifierStats verifierStats;
#endif
    /* String pointed here will be deposited on the stack frame of dvmAbort */
    const char *lastMessage;
};

struct DvmGlobals_16 {
    /*
     * Some options from the command line or environment.
     */
    char*       bootClassPathStr;
    char*       classPathStr;
    size_t      heapStartingSize;
    size_t      heapMaximumSize;
    size_t      heapGrowthLimit;
    size_t      stackSize;
    size_t      mainThreadStackSize;
    bool        verboseGc;
    bool        verboseJni;
    bool        verboseClass;
    bool        verboseShutdown;
    bool        jdwpAllowed;        // debugging allowed for this process?
    bool        jdwpConfigured;     // has debugging info been provided?
    JdwpTransportType jdwpTransport;
    bool        jdwpServer;
    char*       jdwpHost;
    int         jdwpPort;
    bool        jdwpSuspend;
    ProfilerClockSource profilerClockSource;
    /*
     * Lock profiling threshold value in milliseconds.  Acquires that
     * exceed threshold are logged.  Acquires within the threshold are
     * logged with a probability of $\frac{time}{threshold}$ .  If the
     * threshold is unset no additional logging occurs.
     */
    u4          lockProfThreshold;
    int         (*vfprintfHook)(FILE*, const char*, va_list);
    void        (*exitHook)(int);
    void        (*abortHook)(void);
    bool        (*isSensitiveThreadHook)(void);
    int         jniGrefLimit;       // 0 means no limit
    char*       jniTrace;
    bool        reduceSignals;
    bool        noQuitHandler;
    bool        verifyDexChecksum;
    char*       stackTraceFile;     // for SIGQUIT-inspired output
    bool        logStdio;
    DexOptimizerMode    dexOptMode;
    DexClassVerifyMode  classVerifyMode;
    bool        generateRegisterMaps;
    RegisterMapMode     registerMapMode;
    bool        monitorVerification;
    bool        dexOptForSmp;
    /*
     * GC option flags.
     */
    bool        preciseGc;
    bool        preVerify;
    bool        postVerify;
    bool        concurrentMarkSweep;
    bool        verifyCardTable;
    bool        disableExplicitGc;
    int         assertionCtrlCount;
    AssertionControl*   assertionCtrl;
    ExecutionMode   executionMode;
    /*
     * VM init management.
     */
    bool        initializing;
    bool        optimizing;
    /*
     * java.lang.System properties set from the command line with -D.
     * This is effectively a set, where later entries override earlier
     * ones.
     */
    std::vector<std::string>* properties;
    /*
     * Where the VM goes to find system classes.
     */
    ClassPathEntry* bootClassPath;
    /* used by the DEX optimizer to load classes from an unfinished DEX */
    DvmDex*     bootClassPathOptExtra;
    bool        optimizingBootstrapClass;
    /*
     * Loaded classes, hashed by class name.  Each entry is a ClassObject*,
     * allocated in GC space.
     */
    HashTable*  loadedClasses;
    /*
     * Value for the next class serial number to be assigned.  This is
     * incremented as we load classes.  Failed loads and races may result
     * in some numbers being skipped, and the serial number is not
     * guaranteed to start at 1, so the current value should not be used
     * as a count of loaded classes.
     */
    volatile int classSerialNumber;
    /*
     * Classes with a low classSerialNumber are probably in the zygote, and
     * their InitiatingLoaderList is not used, to promote sharing. The list is
     * kept here instead.
     */
    InitiatingLoaderList* initiatingLoaderList;
    /*
     * Interned strings.
     */
    /* A mutex that guards access to the interned string tables. */
    pthread_mutex_t internLock;
    /* Hash table of strings interned by the user. */
    HashTable*  internedStrings;
    /* Hash table of strings interned by the class loader. */
    HashTable*  literalStrings;
    /*
     * Classes constructed directly by the vm.
     */
    /* the class Class */
    ClassObject* classJavaLangClass;
    /* synthetic classes representing primitive types */
    ClassObject* typeVoid;
    ClassObject* typeBoolean;
    ClassObject* typeByte;
    ClassObject* typeShort;
    ClassObject* typeChar;
    ClassObject* typeInt;
    ClassObject* typeLong;
    ClassObject* typeFloat;
    ClassObject* typeDouble;
    /* synthetic classes for arrays of primitives */
    ClassObject* classArrayBoolean;
    ClassObject* classArrayByte;
    ClassObject* classArrayShort;
    ClassObject* classArrayChar;
    ClassObject* classArrayInt;
    ClassObject* classArrayLong;
    ClassObject* classArrayFloat;
    ClassObject* classArrayDouble;
    /*
     * Quick lookups for popular classes used internally.
     */
    ClassObject* classJavaLangClassArray;
    ClassObject* classJavaLangClassLoader;
    ClassObject* classJavaLangObject;
    ClassObject* classJavaLangObjectArray;
    ClassObject* classJavaLangString;
    ClassObject* classJavaLangThread;
    ClassObject* classJavaLangVMThread;
    ClassObject* classJavaLangThreadGroup;
    ClassObject* classJavaLangStackTraceElement;
    ClassObject* classJavaLangStackTraceElementArray;
    ClassObject* classJavaLangAnnotationAnnotationArray;
    ClassObject* classJavaLangAnnotationAnnotationArrayArray;
    ClassObject* classJavaLangReflectAccessibleObject;
    ClassObject* classJavaLangReflectConstructor;
    ClassObject* classJavaLangReflectConstructorArray;
    ClassObject* classJavaLangReflectField;
    ClassObject* classJavaLangReflectFieldArray;
    ClassObject* classJavaLangReflectMethod;
    ClassObject* classJavaLangReflectMethodArray;
    ClassObject* classJavaLangReflectProxy;
    ClassObject* classJavaNioReadWriteDirectByteBuffer;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationFactory;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationMember;
    ClassObject* classOrgApacheHarmonyLangAnnotationAnnotationMemberArray;
    ClassObject* classOrgApacheHarmonyDalvikDdmcChunk;
    ClassObject* classOrgApacheHarmonyDalvikDdmcDdmServer;
    ClassObject* classJavaLangRefFinalizerReference;
    /*
     * classes representing exception types. The names here don't include
     * packages, just to keep the use sites a bit less verbose. All are
     * in java.lang, except where noted.
     */
    ClassObject* exAbstractMethodError;
    ClassObject* exArithmeticException;
    ClassObject* exArrayIndexOutOfBoundsException;
    ClassObject* exArrayStoreException;
    ClassObject* exClassCastException;
    ClassObject* exClassCircularityError;
    ClassObject* exClassFormatError;
    ClassObject* exClassNotFoundException;
    ClassObject* exError;
    ClassObject* exExceptionInInitializerError;
    ClassObject* exFileNotFoundException; /* in java.io */
    ClassObject* exIOException;           /* in java.io */
    ClassObject* exIllegalAccessError;
    ClassObject* exIllegalAccessException;
    ClassObject* exIllegalArgumentException;
    ClassObject* exIllegalMonitorStateException;
    ClassObject* exIllegalStateException;
    ClassObject* exIllegalThreadStateException;
    ClassObject* exIncompatibleClassChangeError;
    ClassObject* exInstantiationError;
    ClassObject* exInstantiationException;
    ClassObject* exInternalError;
    ClassObject* exInterruptedException;
    ClassObject* exLinkageError;
    ClassObject* exNegativeArraySizeException;
    ClassObject* exNoClassDefFoundError;
    ClassObject* exNoSuchFieldError;
    ClassObject* exNoSuchFieldException;
    ClassObject* exNoSuchMethodError;
    ClassObject* exNullPointerException;
    ClassObject* exOutOfMemoryError;
    ClassObject* exRuntimeException;
    ClassObject* exStackOverflowError;
    ClassObject* exStaleDexCacheError;    /* in dalvik.system */
    ClassObject* exStringIndexOutOfBoundsException;
    ClassObject* exThrowable;
    ClassObject* exTypeNotPresentException;
    ClassObject* exUnsatisfiedLinkError;
    ClassObject* exUnsupportedOperationException;
    ClassObject* exVerifyError;
    ClassObject* exVirtualMachineError;
    /* method offsets - Object */
    int         voffJavaLangObject_equals;
    int         voffJavaLangObject_hashCode;
    int         voffJavaLangObject_toString;
    /* field offsets - String */
    int         offJavaLangString_value;
    int         offJavaLangString_count;
    int         offJavaLangString_offset;
    int         offJavaLangString_hashCode;
    /* field offsets - Thread */
    int         offJavaLangThread_vmThread;
    int         offJavaLangThread_group;
    int         offJavaLangThread_daemon;
    int         offJavaLangThread_name;
    int         offJavaLangThread_priority;
    int         offJavaLangThread_uncaughtHandler;
    int         offJavaLangThread_contextClassLoader;
    /* method offsets - Thread */
    int         voffJavaLangThread_run;
    /* field offsets - ThreadGroup */
    int         offJavaLangThreadGroup_name;
    int         offJavaLangThreadGroup_parent;
    /* field offsets - VMThread */
    int         offJavaLangVMThread_thread;
    int         offJavaLangVMThread_vmData;
    /* method offsets - ThreadGroup */
    int         voffJavaLangThreadGroup_removeThread;
    /* field offsets - Throwable */
    int         offJavaLangThrowable_stackState;
    int         offJavaLangThrowable_cause;
    /* method offsets - ClassLoader */
    int         voffJavaLangClassLoader_loadClass;
    /* direct method pointers - ClassLoader */
    Method*     methJavaLangClassLoader_getSystemClassLoader;
    /* field offsets - java.lang.reflect.* */
    int         offJavaLangReflectConstructor_slot;
    int         offJavaLangReflectConstructor_declClass;
    int         offJavaLangReflectField_slot;
    int         offJavaLangReflectField_declClass;
    int         offJavaLangReflectMethod_slot;
    int         offJavaLangReflectMethod_declClass;
    /* field offsets - java.lang.ref.Reference */
    int         offJavaLangRefReference_referent;
    int         offJavaLangRefReference_queue;
    int         offJavaLangRefReference_queueNext;
    int         offJavaLangRefReference_pendingNext;
    /* field offsets - java.lang.ref.FinalizerReference */
    int offJavaLangRefFinalizerReference_zombie;
    /* method pointers - java.lang.ref.ReferenceQueue */
    Method* methJavaLangRefReferenceQueueAdd;
    /* method pointers - java.lang.ref.FinalizerReference */
    Method* methJavaLangRefFinalizerReferenceAdd;
    /* constructor method pointers; no vtable involved, so use Method* */
    Method*     methJavaLangStackTraceElement_init;
    Method*     methJavaLangReflectConstructor_init;
    Method*     methJavaLangReflectField_init;
    Method*     methJavaLangReflectMethod_init;
    Method*     methOrgApacheHarmonyLangAnnotationAnnotationMember_init;
    /* static method pointers - android.lang.annotation.* */
    Method*
        methOrgApacheHarmonyLangAnnotationAnnotationFactory_createAnnotation;
    /* direct method pointers - java.lang.reflect.Proxy */
    Method*     methJavaLangReflectProxy_constructorPrototype;
    /* field offsets - java.lang.reflect.Proxy */
    int         offJavaLangReflectProxy_h;
    /* field offsets - java.io.FileDescriptor */
    int         offJavaIoFileDescriptor_descriptor;
    /* direct method pointers - dalvik.system.NativeStart */
    Method*     methDalvikSystemNativeStart_main;
    Method*     methDalvikSystemNativeStart_run;
    /* assorted direct buffer helpers */
    Method*     methJavaNioReadWriteDirectByteBuffer_init;
    int         offJavaNioBuffer_capacity;
    int         offJavaNioBuffer_effectiveDirectAddress;
    /* direct method pointers - org.apache.harmony.dalvik.ddmc.DdmServer */
    Method*     methDalvikDdmcServer_dispatch;
    Method*     methDalvikDdmcServer_broadcast;
    /* field offsets - org.apache.harmony.dalvik.ddmc.Chunk */
    int         offDalvikDdmcChunk_type;
    int         offDalvikDdmcChunk_data;
    int         offDalvikDdmcChunk_offset;
    int         offDalvikDdmcChunk_length;
    /*
     * Thread list.  This always has at least one element in it (main),
     * and main is always the first entry.
     *
     * The threadListLock is used for several things, including the thread
     * start condition variable.  Generally speaking, you must hold the
     * threadListLock when:
     *  - adding/removing items from the list
     *  - waiting on or signaling threadStartCond
     *  - examining the Thread struct for another thread (this is to avoid
     *    one thread freeing the Thread struct while another thread is
     *    perusing it)
     */
    Thread*     threadList;
    pthread_mutex_t threadListLock;
    pthread_cond_t threadStartCond;
    /*
     * The thread code grabs this before suspending all threads.  There
     * are a few things that can cause a "suspend all":
     *  (1) the GC is starting;
     *  (2) the debugger has sent a "suspend all" request;
     *  (3) a thread has hit a breakpoint or exception that the debugger
     *      has marked as a "suspend all" event;
     *  (4) the SignalCatcher caught a signal that requires suspension.
     *  (5) (if implemented) the JIT needs to perform a heavyweight
     *      rearrangement of the translation cache or JitTable.
     *
     * Because we use "safe point" self-suspension, it is never safe to
     * do a blocking "lock" call on this mutex -- if it has been acquired,
     * somebody is probably trying to put you to sleep.  The leading '_' is
     * intended as a reminder that this lock is special.
     */
    pthread_mutex_t _threadSuspendLock;
    /*
     * Guards Thread->suspendCount for all threads, and
     * provides the lock for the condition variable that all suspended threads
     * sleep on (threadSuspendCountCond).
     *
     * This has to be separate from threadListLock because of the way
     * threads put themselves to sleep.
     */
    pthread_mutex_t threadSuspendCountLock;
    /*
     * Suspended threads sleep on this.  They should sleep on the condition
     * variable until their "suspend count" is zero.
     *
     * Paired with "threadSuspendCountLock".
     */
    pthread_cond_t  threadSuspendCountCond;
    /*
     * Sum of all threads' suspendCount fields. Guarded by
     * threadSuspendCountLock.
     */
    int  sumThreadSuspendCount;
    /*
     * MUTEX ORDERING: when locking multiple mutexes, always grab them in
     * this order to avoid deadlock:
     *
     *  (1) _threadSuspendLock      (use lockThreadSuspend())
     *  (2) threadListLock          (use dvmLockThreadList())
     *  (3) threadSuspendCountLock  (use lockThreadSuspendCount())
     */
    /*
     * Thread ID bitmap.  We want threads to have small integer IDs so
     * we can use them in "thin locks".
     */
    BitVector*  threadIdMap;
    /*
     * Manage exit conditions.  The VM exits when all non-daemon threads
     * have exited.  If the main thread returns early, we need to sleep
     * on a condition variable.
     */
    int         nonDaemonThreadCount;   /* must hold threadListLock to access */
    pthread_cond_t  vmExitCond;
    /*
     * The set of DEX files loaded by custom class loaders.
     */
    HashTable*  userDexFiles;
    /*
     * JNI global reference table.
     */
    IndirectRefTable jniGlobalRefTable;
    IndirectRefTable jniWeakGlobalRefTable;
    pthread_mutex_t jniGlobalRefLock;
    pthread_mutex_t jniWeakGlobalRefLock;
    int         jniGlobalRefHiMark;
    int         jniGlobalRefLoMark;
    /*
     * JNI pinned object table (used for primitive arrays).
     */
    ReferenceTable  jniPinRefTable;
    pthread_mutex_t jniPinRefLock;
    /*
     * Native shared library table.
     */
    HashTable*  nativeLibs;
    /*
     * GC heap lock.  Functions like gcMalloc() acquire this before making
     * any changes to the heap.  It is held throughout garbage collection.
     */
    pthread_mutex_t gcHeapLock;
    /*
     * Condition variable to queue threads waiting to retry an
     * allocation.  Signaled after a concurrent GC is completed.
     */
    pthread_cond_t gcHeapCond;
    /* Opaque pointer representing the heap. */
    GcHeap*     gcHeap;
    /* The card table base, modified as needed for marking cards. */
    u1*         biasedCardTableBase;
    /*
     * Pre-allocated throwables.
     */
    Object*     outOfMemoryObj;
    Object*     internalErrorObj;
    Object*     noClassDefFoundErrorObj;
    /* Monitor list, so we can free them */
    /*volatile*/ Monitor* monitorList;
    /* Monitor for Thread.sleep() implementation */
    Monitor*    threadSleepMon;
    /* set when we create a second heap inside the zygote */
    bool        newZygoteHeapAllocated;
    /*
     * TLS keys.
     */
    pthread_key_t pthreadKeySelf;       /* Thread*, for dvmThreadSelf */
    /*
     * Cache results of "A instanceof B".
     */
    AtomicCache* instanceofCache;
    /* inline substitution table, used during optimization */
    InlineSub*          inlineSubs;
    /*
     * Bootstrap class loader linear allocator.
     */
    LinearAllocHdr* pBootLoaderAlloc;
    /*
     * Compute some stats on loaded classes.
     */
    int         numLoadedClasses;
    int         numDeclaredMethods;
    int         numDeclaredInstFields;
    int         numDeclaredStaticFields;
    /* when using a native debugger, set this to suppress watchdog timers */
    bool        nativeDebuggerActive;
    /*
     * JDWP debugger support.
     *
     * Note: Each thread will normally determine whether the debugger is active
     * for it by referring to its subMode flags.  "debuggerActive" here should be
     * seen as "debugger is making requests of 1 or more threads".
     */
    bool        debuggerConnected;      /* debugger or DDMS is connected */
    bool        debuggerActive;         /* debugger is making requests */
    JdwpState*  jdwpState;
    /*
     * Registry of objects known to the debugger.
     */
    HashTable*  dbgRegistry;
    /*
     * Debugger breakpoint table.
     */
    BreakpointSet*  breakpointSet;
    /*
     * Single-step control struct.  We currently only allow one thread to
     * be single-stepping at a time, which is all that really makes sense,
     * but it's possible we may need to expand this to be per-thread.
     */
    StepControl stepControl;
    /*
     * DDM features embedded in the VM.
     */
    bool        ddmThreadNotification;
    /*
     * Zygote (partially-started process) support
     */
    bool        zygote;
    /*
     * Used for tracking allocations that we report to DDMS.  When the feature
     * is enabled (through a DDMS request) the "allocRecords" pointer becomes
     * non-NULL.
     */
    pthread_mutex_t allocTrackerLock;
    AllocRecord*    allocRecords;
    int             allocRecordHead;        /* most-recently-added entry */
    int             allocRecordCount;       /* #of valid entries */
    /*
     * When a profiler is enabled, this is incremented.  Distinct profilers
     * include "dmtrace" method tracing, emulator method tracing, and
     * possibly instruction counting.
     *
     * The purpose of this is to have a single value that shows whether any
     * profiling is going on.  Individual thread will normally check their
     * thread-private subMode flags to take any profiling action.
     */
    volatile int activeProfilers;
    /*
     * State for method-trace profiling.
     */
    MethodTraceState methodTrace;
    Method*     methodTraceGcMethod;
    Method*     methodTraceClassPrepMethod;
    /*
     * State for emulator tracing.
     */
    void*       emulatorTracePage;
    int         emulatorTraceEnableCount;
    /*
     * Global state for memory allocation profiling.
     */
    AllocProfState allocProf;
    /*
     * Pointers to the original methods for things that have been inlined.
     * This makes it easy for us to output method entry/exit records for
     * the method calls we're not actually making.  (Used by method
     * profiling.)
     */
    Method**    inlinedMethods;
    /*
     * Dalvik instruction counts (kNumPackedOpcodes entries).
     */
    int*        executedInstrCounts;
    int         instructionCountEnableCount;
    /*
     * Signal catcher thread (for SIGQUIT).
     */
    pthread_t   signalCatcherHandle;
    bool        haltSignalCatcher;
    /*
     * Stdout/stderr conversion thread.
     */
    bool            haltStdioConverter;
    bool            stdioConverterReady;
    pthread_t       stdioConverterHandle;
    pthread_mutex_t stdioConverterLock;
    pthread_cond_t  stdioConverterCond;
    int             stdoutPipe[2];
    int             stderrPipe[2];
    /*
     * pid of the system_server process. We track it so that when system server
     * crashes the Zygote process will be killed and restarted.
     */
    pid_t systemServerPid;
    int kernelGroupScheduling;
//#define COUNT_PRECISE_METHODS
#ifdef COUNT_PRECISE_METHODS
    PointerSet* preciseMethods;
#endif
    /* some RegisterMap statistics, useful during development */
    void*       registerMapStats;
#ifdef VERIFIER_STATS
    VerifierStats verifierStats;
#endif
    /* String pointed here will be deposited on the stack frame of dvmAbort */
    const char *lastMessage;
};


/* Use the top 16 bits of the access flags field for
 * other class flags.  Code should use the *CLASS_FLAG*()
 * macros to set/get these flags.
 */
enum ClassFlags {
    CLASS_ISFINALIZABLE        = (1<<31), // class/ancestor overrides finalize()
    CLASS_ISARRAY              = (1<<30), // class is a "[*"
    CLASS_ISOBJECTARRAY        = (1<<29), // class is a "[L*" or "[[*"
    CLASS_ISCLASS              = (1<<28), // class is *the* class Class

    CLASS_ISREFERENCE          = (1<<27), // class is a soft/weak/phantom ref
                                          // only ISREFERENCE is set --> soft
    CLASS_ISWEAKREFERENCE      = (1<<26), // class is a weak reference
    CLASS_ISFINALIZERREFERENCE = (1<<25), // class is a finalizer reference
    CLASS_ISPHANTOMREFERENCE   = (1<<24), // class is a phantom reference

    CLASS_MULTIPLE_DEFS        = (1<<23), // DEX verifier: defs in multiple DEXs

    /* unlike the others, these can be present in the optimized DEX file */
    CLASS_ISOPTIMIZED          = (1<<17), // class may contain opt instrs
    CLASS_ISPREVERIFIED        = (1<<16), // class has been pre-verified
};




struct ClassObject;
struct Method;
struct HashTable;
struct InstField;
struct StringObject;

typedef uint8_t             u1;
typedef uint16_t            u2;
typedef uint32_t            u4;
typedef uint64_t            u8;
typedef int8_t              s1;
typedef int16_t             s2;
typedef int32_t             s4;
typedef int64_t             s8;

/*
 * VM build number.  This must change whenever something that affects the
 * way classes load changes, e.g. field ordering or vtable layout.  Changing
 * this guarantees that the optimized form of the DEX file is regenerated.
 */
#define DALVIK_VM_BUILD     27
#define Z_NULL				0


bool dvmContinueOptimization(const char *packageLocation,void* fdBuf, u1** OutBuf,off_t dexOffset, u4 crc, u4 modWhen, long dexLength,
     int sdkType, long* OdexLength);


/* same, but for optimized DEX header */
#define DEX_OPT_MAGIC   "dey\n"
#define DEX_OPT_MAGIC_VERS  "036\0"

#define DEX_DEP_MAGIC   "deps"

/*
 * This holds some meta-data while we construct the set of register maps
 * for a DEX file.
 *
 * In particular, it keeps track of our temporary mmap region so we can
 * free it later.
 */
struct RegisterMapBuilder {
    /* public */
    void*       data;
    size_t      size;

    /* private */
    MemMapping  memMap;
};


/*
 * Set 4 little-endian bytes.
 */
inline void set4LE(u1* buf, u4 val)
{
    *buf++ = (u1)(val);
    *buf++ = (u1)(val >> 8);
    *buf++ = (u1)(val >> 16);
    *buf = (u1)(val >> 24);
}

inline void dvmAbort(void) {
    exit(1);
}

/* return the ClassDef with the specified index */
inline const DexClassDef* dexGetClassDef(const DexFile* pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->classDefsSize);
    return &pDexFile->pClassDefs[idx];
}

/* return the const char* string data referred to by the given string_id */
inline const char* dexGetStringData(const DexFile* pDexFile,
        const DexStringId* pStringId) {
    const u1* ptr = (const u1*)(pDexFile->baseAddr + pStringId->stringDataOff);

    // Skip the uleb128 length.
    while (*(ptr++) > 0x7f) /* empty */ ;

    return (const char*) ptr;
}
/* return the StringId with the specified index */
inline const DexStringId* dexGetStringId(const DexFile* pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->stringIdsSize);
    return &pDexFile->pStringIds[idx];
}
/* return the UTF-8 encoded string with the specified string_id index */
inline const char* dexStringById(const DexFile* pDexFile, u4 idx) {
    const DexStringId* pStringId = dexGetStringId(pDexFile, idx);
    return dexGetStringData(pDexFile, pStringId);
}

/* Return the UTF-8 encoded string with the specified string_id index,
 * also filling in the UTF-16 size (number of 16-bit code points).*/
const char* dexStringAndSizeById(const DexFile* pDexFile, u4 idx,
        u4* utf16Size);

/* return the TypeId with the specified index */
inline const DexTypeId* dexGetTypeId(const DexFile* pDexFile, u4 idx) {
    assert(idx < pDexFile->pHeader->typeIdsSize);
    return &pDexFile->pTypeIds[idx];
}
/*
 * Get the descriptor string associated with a given type index.
 * The caller should not free() the returned string.
 */
inline const char* dexStringByTypeIdx(const DexFile* pDexFile, u4 idx) {
    const DexTypeId* typeId = dexGetTypeId(pDexFile, idx);
    return dexStringById(pDexFile, typeId->descriptorIdx);
}
/* get full path of optimized DEX file */
inline const char* dvmGetJarFileCacheFileName(JarFile* pJarFile) {
    return pJarFile->cacheFileName;
}
/*
 * Pry the DexFile out of a RawDexFile.
 */
inline DvmDex* dvmGetRawDexFileDex(RawDexFile* pRawDexFile) {
    return pRawDexFile->pDvmDex;
}
/* pry the DexFile out of a JarFile */
inline DvmDex* dvmGetJarFileDex(JarFile* pJarFile) {
    return pJarFile->pDvmDex;
}

/*
 * Get/set class flags.
 */
#define SET_CLASS_FLAG(clazz, flag) \
    do { (clazz)->accessFlags |= (flag); } while (0)

#endif
