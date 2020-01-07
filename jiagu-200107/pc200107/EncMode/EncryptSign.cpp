#include "aescrypt.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool EncryptDex(char* filePath, char* pOutput, char* pKey)
{
	fstream fileDex;
	EncMode encryptMode;
	fileDex.open(filePath, ios::in|ios::binary);

	if(!fileDex)
		cout<<"文件读取发生错误！"<<endl;

	fileDex.seekg(0,ios::end);
	int size = fileDex.tellg();
	if (size%16 != 0)
		size = size + size%16;

	char* pInput = new char[size];
	memset(pInput, 0, size);
	//pOutput = new char[size+AES_BLOCK_SIZE];
	
	fileDex.seekg(0,ios::beg);
	fileDex.read(pInput,size);
	fileDex.close();

	encryptMode.SetMode(MODE_CBC);
	if (! encryptMode.EncryptMode(pInput, size, pKey, pOutput))
		return false;
	delete []pInput;

	return true;

};

bool SignDex( char* filePath, char* pKey, char* lastKey, char* pOutput )
{
	string Inputfile,line;
	fstream fileDex;
	EncCipher m_Signdex;
	fileDex.open(filePath, ios::in|ios::binary);

	if(!fileDex)
		cout<<"文件读取发生错误！"<<endl;

	fileDex.seekg(0,ios::end);
	int size = fileDex.tellg();
	//char* pInput = new char[size];
	//pOutput = new char[SIGNDEX_LEN];

	while(getline(fileDex,line))
		Inputfile += line + '\n';


	const char* InputData = Inputfile.c_str();
	//char* InputData = new char[size];
	//memcpy(InputData,InputDatafile,size);

	if(!m_Signdex.GenMac((BYTE*)InputData, size, (BYTE*)pKey, (BYTE*)lastKey, (BYTE*)pOutput))
		return false;

	fileDex.close();
	delete []InputData;
	return true;

};


