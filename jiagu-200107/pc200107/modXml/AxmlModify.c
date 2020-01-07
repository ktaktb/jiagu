/* AXML Ambiguity */

#include "AxmlModify.h"
#include "AxmlParser.h"
#include <stdio.h>

/* attribute structure within tag */
typedef struct{
	char uri[4];		/* uri of its namespace */
	char name[4];
	char string[4];	/* attribute value if type == ATTR_STRING */
	char type[4];		/* attribute type, == ATTR_*  指定属性值得类型，如string, int, float等*/
	char data[4];		/* attribute value, encoded on type 属性的值，该值根据属性的 type进行编码*/
} Attribute;

static void copyDataWithPosChange(FileAssit_t *to, FileAssit_t *src, size_t size){
	memcpy(to->data + to->cur, src->data + src->cur, size);
	to->cur += size;
	src->cur += size;
}

static uint32_t getUint32(FileAssit_t *hp){
	uint32_t value = 0;
	unsigned char *p = hp->data + hp->cur;
	value = p[0] | p[1]<<8 | p[2]<<16 | p[3]<<24;
	hp->cur += 4;
	return value;
}

static void skipUint32(FileAssit_t *hp){
	hp->cur += 4;
}

/* 将大端表示的uint_32转换成小端表示的4字节数据后，存储到目标地址 */
static void copyUint32(char *to, uint32_t value){
	/**/
	char p[4];
	p[0] = value & 0xff ;
	p[1] = (value >>8) &0xff;
	p[2] = (value >> 16) &0xff;
	p[3] = (value >> 24) &0xff;
	memcpy(to, p, 4);

}

/* 在string中插入字符串 */
void modifyStringChunk(FileAssit_t *in, FileAssit_t *out, size_t *externSize){
	//printf("modifyStringChunk!\n");
	int i;
	uint32_t stringChunkSize = 0;
	uint32_t stringCount = 0;
	uint32_t styleCount = 0;
	uint32_t stringOffset = 0;
	uint32_t styleOffset = 0;
	uint32_t stringLen = 0;
	uint32_t addStringOffset0 = 0;
	uint32_t addStringOffset_alied0 = 0;
	uint32_t addStringCount = 1;
	uint32_t appliStringSize = 0;

	// modified name string
	char appliName[34] = {0x4d,0x00,0x79,0x00,0x44,0x00,0x65,0x00,0x6d,0x00,0x6f,0x00,0x41,0x00,0x70,0x00,
		0x70,0x00,0x6c,0x00,0x69,0x00,0x63,0x00,0x61,0x00,0x74,0x00,0x69,0x00,0x6f,0x00,0x6e,0x00};// MyDemoApplication

	//问题，一定要进行4字节对齐操作！考虑到如果有styleData段，那么原string本身就没进行4字节对齐，直接在后面加上string，然后决定是否对新加入的string进行4字节对齐。
	//如果没styleData段，那么原string本身就进行了4字节对齐，我们只需要根据添加的string length来觉得是否进行4字节对齐即可。
	//这就涉及到stringchunksize的改变和styleOffset的改变,所以我们只有在最后才能确定两者的大小~
	//总之对齐操作比较麻烦，需要特别注意

	// generate application string
	int flag = 0;
	for (i = 0; i < sizeof(g_packName); i++)
	{
		if (g_packName[i] == 0x2e)
		{
			g_packNameLen = i + 2;
			flag++;
		}
		if (flag == 0)
		{
			g_packNameLen = 0;
		}
	}
	appliStringSize = g_packNameLen+sizeof(appliName)+4;
	char appliString[500] = {0};
	appliString[0] = appliStringSize/2 - 2;
	memcpy(&appliString[2], &g_packName, g_packNameLen);
	memcpy(&appliString[2+g_packNameLen], &appliName, sizeof(appliName));
	if (g_packNameLen%4 == 0)
	{
		appliStringSize = appliStringSize + 2;
	}
	
	copyDataWithPosChange(out, in, 4); //StringTag
	stringChunkSize = getUint32(in);
	out->cur += 4;

	stringCount = getUint32(in);
	g_curStringCount = stringCount + addStringCount;
		
	copyUint32(out->data + out->cur, g_curStringCount);
	out->cur += 4;
	
	styleCount = getUint32(in);
	in->cur -= 4;
	
	copyDataWithPosChange(out, in, 4*2); //stylesCount 和 reverse
	
	stringOffset = getUint32(in);
    
    copyUint32(out->data + out->cur, stringOffset + 4*addStringCount); //插入n个字符串，所以需要添加n个4字节的偏移条目，所以stringOffset要加4n
	out->cur += 4;
	
	styleOffset = getUint32(in); //styleOffset可能会改变，所以这里就不直接copy了
	if(styleOffset == 0){ //说明没有style，可以直接copy
		in->cur -= 4;
		copyDataWithPosChange(out, in, 4);
		stringLen = stringChunkSize - stringOffset;
	}else{ //说明有style，那么在插入string后styleOffset会改变
		//styleOffset最后再进行赋值
		out->cur += 4;
		stringLen = styleOffset - stringOffset;
	}
	
	//copy stringCount 个 string 偏移值
	copyDataWithPosChange(out, in, stringCount * 4);
	
	/*然后在此时的out->data中插入string偏移值，指向我们插入的字符串的首地址*/
	addStringOffset0 = stringLen;
	addStringOffset_alied0 = (addStringOffset0+0x03)&(~0x03);  
	//虽然在没有style的情况下stringLen本身就是4的整数倍，但在有style的情况下就不一定了~
	//为了方便以后扩展，这里addStringOffset2虽然是4的整数倍，但还是进行一次对齐吧
	
	*externSize += (addStringOffset_alied0 - addStringOffset0);

	//设置偏移值
	copyUint32(out->data + out->cur, addStringOffset_alied0);
	out->cur += 4;

	//然后就是连续的n个styles偏移值。
	copyDataWithPosChange(out, in, styleCount * 4);
	
	//然后就是string data
	copyDataWithPosChange(out, in, stringLen);

	memcpy(out->data + out->cur, appliString, appliStringSize);
	out->cur += appliStringSize;
	
	/*添加长度刚好为4的整数倍，所以添加的字符串本身不涉及对齐操作。如果要添加其他的字符串，进行相应的修改即可*/
	*externSize += appliStringSize; //字符串长度
	*externSize += 4;  //需要为该字符串添加一个4字节的偏移表项

	if(styleOffset != 0){
		//根据sxternSize的值,确定当前styleOffset值
		copyUint32(out->data + 0x20,styleOffset + *externSize);
	}
	//根据sxternSize的值，确定当前stringChunck的大小
	copyUint32(out->data + 0xc, stringChunkSize + *externSize);

}

void modifyResourceChunk(FileAssit_t *in, FileAssit_t *out, size_t *externSize){
	//首先获取原始resourceChunk大小
	uint32_t resChunkSize = 0;
	uint32_t resCount = 0;
	uint32_t needAppendCount = 0;
	resChunkSize = getUint32(in);
	resCount = resChunkSize /4 -2;
	needAppendCount = 0;

	copyUint32(out->data + out->cur, resChunkSize + needAppendCount * 4);
	out->cur += 4;

	//copy 原来的resCount个resourceID
	copyDataWithPosChange(out, in, resCount * 4);

	//用0x0填充剩下的resourcesID
	memset(out->data + out->cur, 0, needAppendCount * 4);
	out->cur += needAppendCount*4;

	*externSize += (needAppendCount * 4);
}

void modifyAppTag(FileAssit_t *in, FileAssit_t *out, size_t *externSize){
	uint32_t attrCount = 0;
	uint32_t chunkSize = 0;

	Attribute attr;
	memset(&attr, 0, sizeof(Attribute));

	if (g_appNameFlag)
	{
		printf("add a new attribute:step1\n");
		copyUint32(attr.uri, g_appURIindex);
		copyUint32(attr.name, g_nameIndex);
		copyUint32(attr.string, g_curStringCount - 1);
		copyUint32(attr.type, 0x03000008);
		copyUint32(attr.data,  g_curStringCount - 1);

		in->cur -= 0x10;
		chunkSize = getUint32(in);
		chunkSize += sizeof(Attribute);
		copyUint32(out->data + out->cur - 0x10, chunkSize);
		in->cur += 0xc;
	}
	copyDataWithPosChange(out, in, 8); //tagname and flags
	attrCount = getUint32(in);

	if (g_appNameFlag)
	{
		printf("add a new attribute:step2\n");
		attrCount = attrCount + 1;
	}

	copyUint32(out->data + out->cur, attrCount);//attribute count
	out->cur += 4;
	copyDataWithPosChange(out, in, 4); //classAttribute

	if (g_appNameFlag)
	{
		printf("add a new attribute:step3\n");
		copyDataWithPosChange(out, in, g_namePosIndex*sizeof(Attribute));
		memcpy(out->data + out->cur, (char*)&attr, sizeof(Attribute));
		out->cur += sizeof(Attribute);
		*externSize += sizeof(Attribute);
		// copyDataWithPosChange(out, in, (attrCount-1)*sizeof(Attribute));
	}
	else
	{
		printf("modify the old attribute\n");
		int i = 0;
			for (i = 0; i < attrCount; ++i)
			{
				if (i == g_appAttIndex)
				{
					copyDataWithPosChange(out, in, sizeof(Attribute));
					out->cur -= 0xc;
					copyUint32(out->data + out->cur, g_curStringCount-1);
					out->cur += 8;
					copyUint32(out->data + out->cur, g_curStringCount-1);
					out->cur += 4;
				}
				else
				{
					copyDataWithPosChange(out, in, sizeof(Attribute));
				}
			}
	}

}

int axmlModify(char* inbuf, size_t insize, char *out_filename){
	//printf("in modify\n");
	FILE *fp;
	char *outbuf;
	char *filename = out_filename;
	size_t externSize = 0;  //扩张的字节数
	uint32_t filesize = 0;
	size_t ret = 0;
	FileAssit_t input_h, output_h;
	fp = fopen(filename, "wb");

	if(fp == NULL)
	{
		fprintf(stderr, "Error: open output file failed.\n");
		return -1;
	}
	outbuf = (char *)malloc((insize + 10000) * sizeof(char));  //多分配字节，可以根据自己添加字符串的大小进行扩充。
	if(outbuf == NULL){
		fprintf(stderr, "Error: malloc outbuf failed.\n");
		return -1;
	}
	memset(outbuf, 0, insize);
	input_h.data = inbuf;
	input_h.cur = 0;
	output_h.data = outbuf;
	output_h.cur = 0;
	//首先copy魔数
	copyDataWithPosChange(&output_h, &input_h, 4);
	//然后获取文件大小
	filesize = getUint32(&input_h);
	//printf("filesize:%d\n", filesize);
	output_h.cur += 4;

	//插入string并更改stringChunk块
	modifyStringChunk(&input_h, &output_h, &externSize);
	
	//style data到g_res_ChunkSizeOffset之间的数据，由于不需要改动，所以直接copy
	copyDataWithPosChange(&output_h, &input_h, g_res_ChunkSizeOffset - input_h.cur);
	
	//根据curStringChunk的大小对ResourceChunk块进行填充，以便新添加的attr的ID号为0x00000000
	modifyResourceChunk(&input_h, &output_h, &externSize);
	
	copyDataWithPosChange(&output_h, &input_h, g_appTag_nameOff - input_h.cur);
	modifyAppTag(&input_h, &output_h, &externSize);

	//修改文件大小
	copyUint32(output_h.data + 4, filesize + externSize);

	//copy剩余的部分
	copyDataWithPosChange(&output_h, &input_h, filesize - input_h.cur);

	ret = fwrite(output_h.data, 1, output_h.cur, fp);
	if(ret != output_h.cur){
		fprintf(stderr, "Error: fwrite outbuf error.\n");
		return -1;
	}
	//free(output_h.data);
	fclose(fp);

	return 0;
}
