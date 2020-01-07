#include <string>
#include <fstream>
#include <ctime>
#include <stdlib.h>
#include <string.h>
using namespace std;

typedef unsigned char BYTE;

void key_map(BYTE* data , char* SoPath, char* switchFile, int ran_num);
char* Keyswitch(char* keyIn, char* SoPath, char* SoPathx86, char* switchFile);