
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <assert.h>
#include "zlib.h"
#include "Ans_dex.h"

#pragma comment(lib,"ZLib.lib")
#include "aescrypt.h"
#include "wb_aes.h"
using namespace std;
unsigned char *ppData;
/*
27 * Reads an unsigned LEB128 value, updating the given pointer to point
28 * just past the end of the read value. This function tolerates
29 * non-zero high-order bits in the fifth encoded byte.
30 */
static int readUnsignedLeb128(unsigned char* pStream) {
	   //unsigned char* ptr = pStream;
	    int result = *(pStream++);
	
		    if (result > 0x7f) {
		        int cur = *(pStream++);
		        result = (result & 0x7f) | ((cur & 0x7f) << 7);
		        if (cur > 0x7f) {
			           cur = *(pStream++);
			            result |= (cur & 0x7f) << 14;
			            if (cur > 0x7f) {
				                cur = *(pStream++);
				                result |= (cur & 0x7f) << 21;
				                if (cur > 0x7f) {
						                cur = *(pStream++);
					                    result |= cur << 28;
					
				}
				
			}
			
		}
		
	}
			ppData = pStream;
		//pStream = ptr;
	    return result;
	
}


int cmp_classname(int a,int b,char *str,char * clsname){
	for(int i = a;i <= b;i++)
		{
			
			if(str[i] != clsname[i])
			{
				return 0;
				//break;
			}

		}
		
	return 1;
}



unsigned int get_binary_from_char(char x) {
	unsigned int bar = (unsigned int)x;
	bar &= 0x000000ff;
	return bar;
}
unsigned int hex2oct(unsigned char* hex)
{
	unsigned int oct = 0;
	for (int i = 3, tmp = 1; i >= 0; --i) {
		oct += (unsigned int)hex[i] * tmp;
		tmp *= 256;
	}
	return oct;
}


/*
100 * Converts a single-character primitive type into its human-readable
101 * equivalent.
102 */
static const char* primitiveTypeLabel(char typeChar)
{
	switch (typeChar) {
	case 'B':   return "byte";
	case 'C':   return "char";
	case 'D':   return "double";
	case 'F':   return "float";
	case 'I':   return "int";
	case 'J':   return "long";
	case 'S':   return "short";
	case 'V':   return "void";
	case 'Z':   return "boolean";
	default:
		return "UNKNOWN";

	}
}

/*
121 * Converts a type descriptor to human-readable "dotted" form.  For
122 * example, "Ljava/lang/String;" becomes "java.lang.String", and
123 * "[I" becomes "int[]".  Also converts '$' to '.', which means this
124 * form can't be converted back to a descriptor.
125 */
static char* descriptorToDot(const char* str)
{
	int targetLen = strlen(str);
	int offset = 0;
	int arrayDepth = 0;
	char* newStr;

	/* strip leading [s; will be added to end */
	while (targetLen > 1 && str[offset] == '[') {
		offset++;
		targetLen--;

	}
	arrayDepth = offset;

	if (targetLen == 1) {
		/* primitive type */
		str = primitiveTypeLabel(str[offset]);
		offset = 0;
		targetLen = strlen(str);

	}
	else {
		/* account for leading 'L' and trailing ';' */
		if (targetLen >= 2 && str[offset] == 'L' &&
			str[offset + targetLen - 1] == ';')
		{
			targetLen -= 2;
			offset++;
		}

	}

	newStr = (char*)malloc(targetLen + arrayDepth * 2 + 1);

	/* copy class name over */
	int i;
	for (i = 0; i < targetLen; i++) {
		char ch = str[offset + i];
		newStr[i] = (ch == '/') ? '.' : ch;
//		newStr[i] = (ch == '/' || ch == '$') ? '.' : ch;

	}

	/* add the appropriate number of brackets for arrays */
	while (arrayDepth > 0) {
		newStr[i++] = '[';
		newStr[i++] = ']';
		arrayDepth--;
	}
	newStr[i] = '\0';
    assert(i == targetLen + arrayDepth * 2);

	return newStr;
}


int readConfig(char* path, int index){
	FILE* f = fopen(path, "r");
	int i=-1;
	int r=0;
	while(i<index){
		fscanf(f, "%d", &r);
		printf("readConfig Ansdex.cpp index:%d    i: %d     r:%d\n",index, i, r );
		i++;
	}
	fclose(f);
	return r;
}


long dexdump(char *dexpath,char *key, char* configPath)
{
	int is_write_back = readConfig(configPath, 2);
	long size;//dex文件长度
	std::fstream file;
	FILE *file_1;
	unsigned char buffer[4];
	unsigned char *a;
	unsigned int stringsize;//string列表字符串个数
	unsigned int typesize;//type列表类型个数
	unsigned int classsize;//class个数
	unsigned int methodsize;//方法个数
	int staticFieldsSize;
	unsigned int staticsize;
	int instanceFieldsSize;
    int directmethodsize;
	int virtualmethodsize;
	long find_map_string;
	long find_map_type;
	long class_defs_off;
	long classDataOff;
	int codelen;
	int codeoff;
	int codedataoff = 0;
	int codedataoff2 = 0;
	unsigned char *stringmap;
	unsigned char *typemap;
	size_t rtnvalue;
	long temp;
	FILE *code;
	FILE *code2;
	FILE *classdex;
	EncMode m_modeenc;
	EncMode m_modedec;	
	BYTE keytemp[16] = {0};
	memcpy(keytemp, key, 16);

	file_1 = fopen(dexpath , "rb");
	code = fopen("codedata.txt", "ab");
	code2 = fopen("codedata2.txt", "ab");
	classdex = fopen("classes.dex", "ab");
    /*得到dex文件长度*/
	fseek(file_1, 0, SEEK_END);
	size = ftell(file_1);

	/*将dex文件读入a中*/
	fseek(file_1, 0, SEEK_SET);
	a = (unsigned char*)malloc(size * sizeof(unsigned char));
	rtnvalue = fread(a, sizeof(unsigned char), size, file_1);

	/*得到string列表的string个数*/
	for (int i = 3, j = 56; i >= 0; i--, j++)
	{
		buffer[i] = a[j];
	}
	stringsize = hex2oct(buffer);
	//printf("%d\n", stringsize);

	/*得到type列表的类型个数*/
	for (int i = 3, j = 64; i >= 0; i--, j++)
	{
		buffer[i] = a[j];
	}
	typesize = hex2oct(buffer);
	//printf("%d\n", typesize);

	/*得到class的个数*/
	for (int i = 3, j = 96; i >= 0; i--, j++)
	{
		buffer[i] = a[j];
	}
	classsize = hex2oct(buffer);
	//printf("%d\n", classsize);


	/*得到string列表数据的地址，读入stringmap中*/
	for (int i = 3, j = 60; i >= 0; i--, j++)
	{
		buffer[i] = a[j];
	}
	find_map_string = hex2oct(buffer);//string列表数据基地址
	stringmap = (unsigned char*)malloc(stringsize * 4 * sizeof(unsigned char));
	fseek(file_1, find_map_string, 0);
	rtnvalue = fread(stringmap, sizeof(unsigned char), stringsize * 4, file_1);


	
	//得到string列表的数据
	char** stringList = NULL;
	stringList = (char**)malloc(sizeof(char*) * stringsize);
	memset(stringList, 0, sizeof(char*) * stringsize);
	int j = 0;
	for (int k = 0; k < stringsize; k++)
	{
		for (int i = 3; i >= 0; i--, j++)
		{
			buffer[i] = stringmap[j];
		}
		temp = hex2oct(buffer);
		if (0 == a[temp])
		{
			continue;
		}
		stringList[k] = (char*)malloc(sizeof(char) * (a[temp] + 1));
		memcpy(stringList[k], &a[temp + 1], a[temp]);
		stringList[k][a[temp]] = '\0';
	}

	/*得到type列表数据的地址，读入typemap中*/
	for (int i = 3, j = 68; i >= 0; i--, j++)
	{
		buffer[i] = a[j];
	}
	find_map_type = hex2oct(buffer);//type列表索引数据基地址
	typemap = (unsigned char*)malloc(typesize * 4 * sizeof(unsigned char));
	fseek(file_1, find_map_type, 0);
	rtnvalue = fread(typemap, sizeof(unsigned char), typesize * 4, file_1);


	//得到type列表的数据
	char** typeList = NULL;
	typeList = (char**)malloc(sizeof(char*) * typesize);
	memset(typeList, 0, sizeof(char*) * typesize);
	 j = 0;
	for (int k = 0; k < typesize; k++)
	{
		for (int i = 3; i >= 0; i--, j++)
		{
			buffer[i] = typemap[j];
		}
		temp = hex2oct(buffer);
		
		if (NULL == stringList[temp])
		{
			continue;
		}
		typeList[k] = (char*)malloc(sizeof(char) * (strlen(stringList[temp])+1));
		typeList[k] = stringList[temp];
		typeList[k][strlen(stringList[temp])] = '\0';
	}


	/*dump classname*/
	char* classname;
	for (int i = 3, j = 100; i >= 0; i--, j++)
	{
		buffer[i] = a[j];
	}
	class_defs_off = hex2oct(buffer);//类定义列表基地址
	classDataOff = class_defs_off + 24;

	
	FILE *code_info;
	code_info = fopen("codeinfo.txt", "wb");
	unsigned int infocount = classsize;
	
	//开始dump
	for (int k = 0; k < classsize; k++) 
	{
		for (int i = 3; i >= 0; i--, class_defs_off++)
		{
			buffer[i] = a[class_defs_off];//classname索引
		}

		temp = hex2oct(buffer);
		classname = descriptorToDot(typeList[temp]);


		char* cmp[16] = {"android.support","com.baidu.","com.amap","com.sina","com.google","com.tencent.","com.alipay","com.facebook","com.alibaba.","com.xiaomi","com.taobao", "org.jivesoftware", "com.easemob", "com.umeng", "com.bumptech", "okhttp3"};
		int res = 0;
		if(strlen(classname) >= 7)
		{
		res += cmp_classname(0,6,cmp[15],classname);
		}
		if(strlen(classname) >= 8)
		{
		res += cmp_classname(0,7,cmp[2],classname);
		res += cmp_classname(0,7,cmp[3],classname);
		}
		if(strlen(classname) >= 9){
		res += cmp_classname(0,8,cmp[13],classname);
		}
		if(strlen(classname) >= 10){
		res += cmp_classname(0,9,cmp[1],classname);
		res += cmp_classname(0,9,cmp[4],classname);
		res += cmp_classname(0,9,cmp[6],classname);
		res += cmp_classname(0,9,cmp[9],classname);
		res += cmp_classname(0,9,cmp[10],classname);
		}
		if(strlen(classname) >= 11){
		res += cmp_classname(0,10,cmp[12],classname);
		}
		if(strlen(classname) >= 12){
		res += cmp_classname(0,11,cmp[5],classname);
		res += cmp_classname(0,11,cmp[7],classname);
		res += cmp_classname(0,11,cmp[8],classname);
		res += cmp_classname(0,11,cmp[14],classname);
		}
		if(strlen(classname) >= 15){
		res += cmp_classname(0,14,cmp[0],classname);
		}
		if(strlen(classname) >= 16){
		res += cmp_classname(0,15,cmp[11],classname);
		}
		

		class_defs_off += 28;
		for (int j = 3; j >= 0; j--, classDataOff++)
		{
			buffer[j] = a[classDataOff];
		}
		classDataOff += 28;

		temp = hex2oct(buffer);//ClassData基地址即ClassDataHeader基地址，里面有methodsize
		
		if (0 == temp) {
			infocount--;
			continue;
		} 
		if(res >= 1){
			infocount--;
			continue;
		}
		
		fwrite(classname, strlen(classname), 1, code_info);
		fprintf(code_info, "\r\n");
		fseek(code_info, 0, SEEK_END);
		
		ppData = &a[temp];
		
/*dump classDataHeader 得到methodsize*/
		staticFieldsSize = readUnsignedLeb128(ppData);
		instanceFieldsSize = readUnsignedLeb128(ppData);
		directmethodsize = readUnsignedLeb128(ppData);
		virtualmethodsize = readUnsignedLeb128(ppData);
		methodsize = directmethodsize + virtualmethodsize;
		fprintf(code_info, "%d", methodsize);
		fprintf(code_info, "\r\n");
		fseek(code_info, 0, SEEK_END);
		
		//temp走过staticFields
	for (int i = 0; i < staticFieldsSize; i++) {
			int count = 0;
			while (count < 2) {
				if (*ppData <= 0x7f) {
					count++;
				}
				ppData += 1;
			}
		}
	for (int i = 0; i < instanceFieldsSize; i++) {
		int count = 0;
		while (count < 2) {
			if (*ppData <= 0x7f) {
				count++;
			}
			ppData += 1;
		}
	}

		/*dump codeoff*/
		
		unsigned int codesum = 0;
		unsigned int encodesum = 0;
		FILE *tempcodefile = fopen("tempcodefile.txt", "wb");
		for (int i = 0; i < methodsize; i++) {
			int count = 0;
			while (count < 3) {
				if (*ppData <= 0x7f) {
					count++;
					ppData += 1;
				}
				else {
					ppData += 1;
				}
				if (count == 2) {
					codeoff = readUnsignedLeb128(ppData);
					count += 1;
					if (!codeoff == 0) {
						
						int insnssize = a[codeoff + 12];
						int insnsoff = codeoff + 16;
						codelen = ((insnssize*2)+ 15) / 16 * 16;	
						fprintf(code_info, "%d ", insnsoff);
						fprintf(code_info, "%d ", insnssize*2);
						fprintf(code_info, "\r\n");
						fseek(code_info, 0, SEEK_END);	
						codesum = codesum + (insnssize * 2);	
						codedataoff +=  codelen;
						char *codeDatain = new char[codelen];
						char *codeCipher = new char[codelen];
						char *codedeCipher = new char[codelen];
						memset(codeDatain, 0, codelen);
						memcpy(codeDatain, &a[insnsoff], insnssize*2);						
						fwrite(codeDatain, 1, insnssize * 2, tempcodefile);
						fseek(tempcodefile, 0, SEEK_END);
						m_modeenc.EncryptMode(codeDatain, codelen,(char *)keytemp, codeCipher);
						fwrite(codeCipher, 1, codelen, code);
						m_modedec.DecryptMode(codeCipher,codelen,(char *)keytemp,codedeCipher);
						if(is_write_back==1){
							memset(&a[insnsoff], 0, insnssize * 2);
						}
						
						delete codeDatain;
						delete codeCipher;
						delete codedeCipher;
					}
					else{						
						fprintf(code_info, "%d ", 0);
						fprintf(code_info, "%d ", 0);
						fprintf(code_info, "\r\n");
						fseek(code_info, 0, SEEK_END);
					}

				}
				
			}
		}
		if(methodsize != 0){
	        fprintf(code_info, "%d\r\n", codesum);
		    fseek(code_info, 0, SEEK_END);
		    encodesum = (codesum + 15) / 16 * 16;
		    fprintf(code_info, "%d\r\n", encodesum);
		    fseek(code_info, 0, SEEK_END);
		    fprintf(code_info, "%d\r\n", codedataoff2);
		    codedataoff2 += encodesum;
		    fseek(code_info, 0, SEEK_END);
		    fprintf(tempcodefile, "\r\n");
		    fclose(tempcodefile);
		
		    FILE *tempfilesread = fopen("tempcodefile.txt", "rb");
		    char *allcode = new char [encodesum];
		    char *enallcode = new char [encodesum];
		    memset(allcode, 0, encodesum);
		    memset(enallcode, 0, encodesum);
		    size_t res;
		    res = fread(allcode, 1, codesum, tempfilesread);
		    fclose(tempfilesread);
		    WbAesEncrypt1m((unsigned char *)keytemp,(unsigned char *)allcode,(unsigned char *)enallcode,encodesum);
		    remove("tempcodefile.txt");
		    fwrite(enallcode, 1, encodesum, code2);
		    delete(allcode);
		    delete(enallcode);
		}
		else{
			fclose(tempcodefile);
			remove("tempcodefile.txt");
			
		}

   }

	unsigned char *b;
	unsigned int infosize;
	FILE *code_info2;
	code_info2 = fopen("codeinfo2.txt", "wb");

	fclose(code_info);
	fclose(code_info2);
	
	ofstream fout("codeinfo2.txt");
	ifstream fin("codeinfo.txt");
	char ch;
	fout << infocount;
	fout << "\r\n";
	
	while((ch = fin.get()) && (ch != EOF))
	{
		fout << ch;
	}


    const Bytef* start = (const Bytef*)a;
    uLong adler = adler32(0L, Z_NULL, 0);
    const int nonSum = 12;
    int checksum = (int)adler32(adler, start + nonSum, size - nonSum);
    memcpy(&a[8], &checksum, sizeof(int));
    fwrite(a, 1,size , classdex);
    fclose(code);
	fclose(file_1);
	fclose(classdex);
	fclose(code2);
	return size;

}
int Dumpdex(char *dexpath,char *key, char* configPath) {
	dexdump(dexpath,key, configPath);
	return 0;
}
