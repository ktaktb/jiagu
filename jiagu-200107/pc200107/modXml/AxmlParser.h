/* AXML Parser */

#ifndef AXMLPARSER_H
#define AXMLPARSER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint32_t g_appTag_nameOff;  //保存application tag中的tagname成员较文件头的偏移值，tagname成员的值为一个字符串索引。我们可以根据该偏移值进行attribution插入和相关修改操作。
extern int g_appAttIndex;

extern int g_packflag;
extern int g_appNameFlag;
extern char g_packName[400];
extern uint32_t g_packNameLen;

extern uint32_t g_styleDataOff;   //style数据块的起始地址
extern uint32_t g_curStringCount; //保存当前stringChunk总共含有的string个数
extern uint32_t g_appURIindex; //保存application所属命名空间的uri索引值
extern int g_namePosIndex;
extern uint32_t g_nameIndex; //保存name属性的索引值
extern uint32_t g_res_ChunkSizeOffset; //保存resourcesChunk的chunksize偏移值

typedef enum{
	AE_STARTDOC = 0,
	AE_ENDDOC,
	AE_STARTTAG,
	AE_ENDTAG,
	AE_TEXT,
	AE_ERROR,
} AxmlEvent_t;

/* chunks' magic numbers */
enum{
	CHUNK_HEAD	= 0x00080003,

	CHUNK_STRING	= 0x001c0001,
	CHUNK_RESOURCE 	= 0x00080180,

	CHUNK_STARTNS	= 0x00100100,
	CHUNK_ENDNS	= 0x00100101,
	CHUNK_STARTTAG	= 0x00100102,
	CHUNK_ENDTAG	= 0x00100103,
	CHUNK_TEXT	= 0x00100104,
};

/* attributes' types */
enum{
	ATTR_NULL 	= 0,
	ATTR_REFERENCE 	= 1,
	ATTR_ATTRIBUTE 	= 2,
	ATTR_STRING 	= 3,
	ATTR_FLOAT 	= 4,
	ATTR_DIMENSION 	= 5,
	ATTR_FRACTION 	= 6,

	ATTR_FIRSTINT 	= 16,

	ATTR_DEC 	= 16,
	ATTR_HEX	= 17,
	ATTR_BOOLEAN	= 18,

	ATTR_FIRSTCOLOR	= 28,
	ATTR_ARGB8 	= 28,
	ATTR_RGB8	= 29,
	ATTR_ARGB4	= 30,
	ATTR_RGB4	= 31,
	ATTR_LASTCOLOR	= 31,

	ATTR_LASTINT	= 31,
};

/* string table */
typedef struct{
	uint32_t count;		/* count of all strings */
	uint32_t *offsets;	/* each string's offset in raw data block */

	unsigned char *data;	/* raw data block, contains all strings encoded by UTF-16LE */
	size_t len;		/* length of raw data block */

	unsigned char **strings;/* string table, point to strings encoded by UTF-8 */
} StringTable_t;

/* attribute structure within tag */
typedef struct{
	uint32_t uri;		/* uri of its namespace */
	uint32_t name;
	uint32_t string;	/* attribute value if type == ATTR_STRING */
	uint32_t type;		/* attribute type, == ATTR_*  指定属性值得类型，如string, int, float等*/
	uint32_t data;		/* attribute value, encoded on type 属性的值，该值根据属性的 type进行编码*/
} Attribute_t;

typedef struct AttrStack_t{
	Attribute_t *list;	/* attributes of current tag */
	uint32_t count;		/* count of these attributes */
	struct AttrStack_t *next;
} AttrStack_t;

/* namespace record */
typedef struct NsRecord{
	uint32_t prefix;
	uint32_t uri;
	struct NsRecord *next;	/* yes, it's a single linked list */
} NsRecord_t;

/* a parser, also a axml parser handle for user */
typedef struct {
	unsigned char *buf;	/* origin raw data, to be parsed */
	size_t size;		/* size of raw data */
	size_t cur;		/* current parsing position in raw data */

	StringTable_t *st;

	NsRecord_t *nsList;
	int nsNew;		/* if a new namespace coming */

	uint32_t tagName;	/* current tag's name */
	uint32_t tagUri;	/* current tag's namespace's uri */
	uint32_t text;		/* when tag is text, its content */

	AttrStack_t *attr;	/* attributes */
} Parser_t;

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void *AxmlOpen(char *buffer, size_t size);

AxmlEvent_t AxmlNext(void *axml);

char *AxmlGetTagPrefix(void *axml);
char *AxmlGetTagName(void *axml);

int AxmlNewNamespace(void *axml);
char *AxmlGetNsPrefix(void *axml);
char *AxmlGetNsUri(void *axml);

uint32_t AxmlGetAttrCount(void *axml);
char *AxmlGetAttrPrefix(void *axml, uint32_t i);
char *AxmlGetAttrName(void *axml, uint32_t i);
char *AxmlGetAttrValue(void *axml, uint32_t i);

char *AxmlGetText(void *axml);

int AxmlClose(void *axml);

int AxmlToXml(char **outbuf, size_t *outsize, char *inbuf, size_t insize);

#ifdef __cplusplus
#if __cplusplus
};
#endif
#endif

#endif
