#include <iostream>
//const unsigned char *ppData;
int Dumpdex(char *dexpath,char *key, char* configPath);
static int readUnsignedLeb128(unsigned char* pStream);
unsigned int get_binary_from_char(char x);
unsigned int hex2oct(unsigned char* hex);
static const char* primitiveTypeLabel(char typeChar);
static char* descriptorToDot(const char* str);
long dexdump(char *dexpath,char *key, char* configPath);
int readConfig(char* path, int index);