#include "KeyTransform.h"

static long soFileSeek = 0x48360;
static long soFileSeekx86 = 0xeb40;
static long keySeek = 0x4e004;


void key_map( BYTE* data , char* SoPath, char* SoPathx86, char* switchFile, int ran_num )
{
	ifstream maskfile(switchFile,ios::in|ios::binary);
	ofstream soFile(SoPath,ios_base::in | ios::out | ios::binary);
	ofstream soFilex86(SoPathx86,ios_base::in | ios::out | ios::binary);
	BYTE mask1[16] = {0};
	BYTE mask2[16] = {0};
	BYTE mask3[32] = {0};

	maskfile.seekg(ran_num*64);
	maskfile.read((char*)mask1, 16);
	maskfile.read((char*)mask2, 16);
	maskfile.read((char*)mask3, 32);
	maskfile.close();

	soFile.seekp(soFileSeek, ios::beg);
	soFile.write((char*)mask3+16, 16);
	soFile.write((char*)mask3, 16);

	soFilex86.seekp(soFileSeekx86, ios::beg);
	soFilex86.write((char*)mask3+16, 16);
	soFilex86.write((char*)mask3, 16);
	soFilex86.close();

	BYTE mask = 0x0f;
	int i;

	BYTE temp,temp_l,temp_r;

	for(i = 0; i < 16; i++)
	{
		temp = *(data + i);
		temp_r = temp & mask;
		temp_l = temp >> 4;
		temp_r = mask1[temp_r];
		temp_l = mask2[temp_l];
		*(data + i) = temp_l ^ temp_r;
	}
	
	soFile.seekp(keySeek, ios::beg);
	soFile.write((char*)data, 16);
	soFile.close();

}

char* Keyswitch( char* keyIn, char* SoPath, char* SoPathx86, char* switchFile )
{
	srand((unsigned)time(0));
	const int ran_num = rand() % 1000;
	static char keytemp[16] = {0};
	memcpy(keytemp, keyIn, 16);
	key_map((BYTE*)keytemp, SoPath, SoPathx86, switchFile, ran_num);

	return keytemp;
}
