#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include "aescrypt.h"
#include "Ans_dex.h"
using namespace std;

#define textName "class.bin"		//默认生成的bin文件的名称	--zhr

#define type_SoSign		1
#define type_DexSign	2
#define type_Cipher		3

#define LenBase		4
#define LenHeader	28
#define LenEnd		12
#define LenSign		32
#define LenKey		16

#define head_name_header	0x64616548
#define head_name_sosign	0x00006F53
#define head_name_dexsign	0x6C656853
#define head_name_crypt		0x79636E45
#define head_name_end		0x6C696154

#define head_flag_header	0x00000091
#define head_flag_sign		0x00000009
#define head_flag_signfalse	0x00000009
#define head_flag_crypt		0x00000021
#define head_flag_end		0x00000019

#define size_head			0x0000001C
#define size_sign			0x00000020
#define size_end			0x0000000C

bool GenBinFlow(char* SoPath, char* SoX86Sign, char* DexPath, char* zipDexPath, char* fakeDexPath, char* KeyInput, char* KeySwitch, char* configPath);

class GenBin
{
public:
	GenBin();
	~GenBin();
	bool GenBinInit();						//用来生成新的空的bin文件	--zhr
	bool HeadInplement(BYTE* keyInput);		//用来填充头部分，以及添加被变换过的Key	--zhr
	bool AddSoSign(int lenSoSign, const char* SoSign);		//用来填充So库的Mac值	--zhr
	bool AddDexSign(int lenDexSign, const char* DexSign);	//用来填充Dex的Mac值	--zhr
	bool AddCipher(int lenOrigin, int lenCipher, char* CipherFile);		//用来填充密钥值		--zhr

	bool GetTextbyType(const char* BinFile, char* pOutput, int type, int _length);	//用于根据类型获得bin文件中内容	--zhr



private:
	char testName[20];
	bool flagSoSign;
	bool flagDexSign;
	bool flagCipher;
	ofstream fileBin;

	bool EndInplement();	//填充完毕则进行文件尾的填充	--zhr
	bool checkFlag();		//检测是否三部分都填充完毕		--zhr
};

