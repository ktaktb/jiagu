#include "Genbin.h"
#include "wb_aes.h"
GenBin::GenBin()
{
	flagDexSign = false;
	flagCipher = false;
	flagSoSign = false;
	memset(testName, 0 ,20);
}

bool GenBin::GenBinInit()
{
    fstream fileCreate;
	fileCreate.open(textName, ios::out);
	fileCreate.close();

	return true;
}


GenBin::~GenBin()
{
//	fileBin.close();
}


bool GenBin::checkFlag()
{
	if(flagCipher && flagDexSign && flagSoSign)
		return true;
	else
		return false;
}


bool GenBin::HeadInplement( BYTE* keyInput )
{
	char HeadOutput[LenHeader];
	int pos = 0;
	long long header = head_name_header;
	long long flag = head_flag_header;
	long long size = size_head;
	memcpy(HeadOutput, &header, LenBase);
	pos += LenBase;
	memcpy(HeadOutput+pos, &flag, LenBase);
	pos+= LenBase;
	memcpy(HeadOutput+pos, &size, LenBase);
	pos += LenBase;
	memcpy(HeadOutput+pos, keyInput, LenKey);

	fileBin.open(textName,ios::out|ios::app);
	fileBin.write(HeadOutput, LenHeader);
	fileBin.close();

	return true;
}


bool GenBin::AddSoSign( int lenSoSign, const char* SoSign )
{
	char* SoSignOutput = new char[lenSoSign+ LenBase*3];
	int pos = 0;
	long long header = head_name_sosign;
	long long flag = head_flag_sign;
	fstream fileBintest;

	memcpy(SoSignOutput, &header, LenBase);
	pos+=LenBase;
	memcpy(SoSignOutput+pos, &flag, LenBase);
	pos += LenBase;
	memcpy(SoSignOutput+pos, &lenSoSign, LenBase);
	pos += LenBase;
	memcpy(SoSignOutput+pos, SoSign, lenSoSign);

	fileBintest.open(textName, ios::out|ios::app|ios::binary);
	fileBintest.write(SoSignOutput, lenSoSign+3*LenBase);
	fileBintest.close();

	delete []SoSignOutput;

	flagSoSign = true;
	if(checkFlag())
		EndInplement();

	return true;
}


bool GenBin::AddDexSign( int lenDexSign, const char* DexSign )
{
	char* DexSignOutput = new char[lenDexSign+ LenBase*3];
	int pos = 0;
	long long header = head_name_dexsign;
	long long flag = head_flag_sign;

	memcpy(DexSignOutput, &header, LenBase);
	pos+=LenBase;
	memcpy(DexSignOutput+pos, &flag, LenBase);
	pos += LenBase;
	memcpy(DexSignOutput+pos, &lenDexSign, LenBase);
	pos += LenBase;
	memcpy(DexSignOutput+pos, DexSign, lenDexSign);

	fileBin.open(textName, ios::out|ios::app|ios::binary);
	fileBin.write(DexSignOutput, lenDexSign + LenBase*3);
	fileBin.close();

	delete []DexSignOutput;

	flagDexSign = true;
	if(checkFlag())
		EndInplement();

	return true;
}


bool GenBin::AddCipher( int lenOrigin, int lenCipher, char* CipherFile )
{
    int trueLength = lenCipher;
	lenCipher = (lenCipher + 15) /16 *16;

	char* CipherOutput = new char[lenCipher+ LenBase*4];
	int pos = 0;
	long long header = head_name_crypt;
	long long flag = head_flag_crypt;

	memcpy(CipherOutput, &header, LenBase);
	pos+=LenBase;
	memcpy(CipherOutput+pos, &flag, LenBase);
	pos += LenBase;
	memcpy(CipherOutput+pos, &lenOrigin, LenBase);
	pos += LenBase;
	memcpy(CipherOutput+pos, &trueLength, LenBase);
	//pos += LenBase;
	//memcpy(CipherOutput+pos, CipherFile, lenCipher);

	fileBin.open(textName, ios::out|ios::app|ios::binary);
	fileBin.write(CipherOutput, LenBase*4);
	fileBin.close();

	delete []CipherOutput;

	flagCipher = true;
	if(checkFlag())
		EndInplement();

	return true;

}

bool GenBin::EndInplement()
{
	char EndOutput[LenEnd];
	int pos = 0;
	long long header = head_name_end;
	long long flag = head_flag_end;
	long long size = size_end;
	memcpy(EndOutput, &header, LenBase);
	pos += LenBase;
	memcpy(EndOutput+pos, &flag, LenBase);
	pos+= LenBase;
	memcpy(EndOutput+pos, &size, LenBase);

	fileBin.open(textName,ios::out|ios::app|ios::binary);
	fileBin.write(EndOutput, LenEnd);
	fileBin.close();

	return true;
}

bool GenBin::GetTextbyType( const char* BinFile, char* pOutput, int type, int _length )
{
	int pos = 0;
	int binLen = _length + 1;
	pos += 3 * LenBase + LenKey;
	long long sosignname = head_name_sosign;
	long long dexsignname = head_name_dexsign;
	long long ciphername = head_name_crypt;
	long long size=0;

	char* TypeName = new char[LenBase];

	if(type == type_SoSign)
		memcpy(TypeName, &sosignname, LenBase);
	else if(type == type_DexSign)
		memcpy(TypeName, &dexsignname, LenBase);
	else if(type == type_Cipher)
		memcpy(TypeName, &ciphername, LenBase);
	else
		return false;

	while (pos < binLen)
	{
		if(memcmp(BinFile+pos, TypeName, LenBase) == 0)
		{
			pos += 2*LenBase;
			memcpy(&size, BinFile+pos, LenBase);
			pos += LenBase;
			memcpy(pOutput, BinFile+pos, size);
			return true;
		}
		else
		{
			pos += 2*LenBase;
			memcpy(&size, BinFile+pos, LenBase);
			if(size < 0)
				return false;
			pos += LenBase + size;
		}
	}
	delete []TypeName;

	return false;
}

/****************************************************************************************************
*Designed by zhr
*GenBinFlow( char* SoPath, char* DexPath, char* KeyInput, char* KeySwitch ) 是整个so库最终的对外接口
*SoPath 为So文件的文件路径
*DexPath 为Dex文件的文件路径
*KeyInput 为对Dex加密，生成Mac值时所使用的密钥
*****************************************************************************************************/
bool GenBinFlow( char* SoPath, char* SoX86Path, char* DexPath, char* zipDexPath, char* fakeDexPath, char* KeyInput, char* KeySwitch , char* configPath)
{
	GenBin bintest;
	EncCipher m_cipher;
	EncMode m_modeenc;
	fstream Sofile;
	fstream SoX86file;
	fstream Dexfile;
	fstream zipDexfile;
	fstream fakeDexfile;
	string line, textSo, x86textSo, textDex, textZipDex, textfakeDex;
	bintest.GenBinInit();

	BYTE SoSign[20], DexSign[20], SoX86Sign[20];
	BYTE keytemp[16] = {0};
	memcpy(keytemp, KeyInput, LenKey);

	//添加bin文件的文件头	--zhr
	bintest.HeadInplement((BYTE*)KeySwitch);
	
	Sofile.open(SoPath,ios::in|ios::binary);
	SoX86file.open(SoX86Path, ios::in|ios::binary);
	Dexfile.open(DexPath,ios::in|ios::binary);
	zipDexfile.open(zipDexPath,ios::in|ios::binary);
	fakeDexfile.open(fakeDexPath, ios::in|ios::binary);

	//get the so sign	--zhr
	while(getline(Sofile,line))
		textSo += line + '\n';
	textSo = textSo.substr(0,textSo.length()-1);
	const char* SoData = textSo.c_str();
	int Sosize = textSo.length();
	Sofile.close();
	m_cipher.GenMac((BYTE*)SoData, Sosize, keytemp, keytemp, SoSign);
	//get the x86 So sign 	--zhr
	while(getline(SoX86file,line))
		x86textSo += line + '\n';
	x86textSo = x86textSo.substr(0,x86textSo.length()-1);
	const char* SoX86Data = x86textSo.c_str();
	int SoX86size = x86textSo.length();
	SoX86file.close();

	m_cipher.GenMac((BYTE*)SoX86Data, SoX86size, keytemp, keytemp, SoX86Sign);

	for(int i=0; i<20; i++)	SoSign[i] = SoSign[i]^SoX86Sign[i];
	bintest.AddSoSign(20,(char*)SoSign);

	//生成Dex文件的Mac值并添加进bin文件	--zhr
	while (getline(fakeDexfile, line))
		textfakeDex += line + '\n';
	textfakeDex = textfakeDex.substr(0, textfakeDex.length()-1);
	const char* fakeDexData = textfakeDex.c_str();
	fakeDexfile.close();

	m_cipher.GenMac((BYTE*)fakeDexData,textfakeDex.length(),keytemp,keytemp, DexSign);

	bintest.AddDexSign(20,(char*)DexSign);

	//生成Dex文件的Mac值并添加进bin文件	--zhr
	while (getline(Dexfile, line))
		textDex += line + '\n';
	textDex = textDex.substr(0, textDex.length()-1);
	const char* unzipDexData = textDex.c_str();
	Dexfile.close();
	
	
	while (getline(zipDexfile, line))
		textZipDex += line + '\n';
	textZipDex = textZipDex.substr(0, textZipDex.length()-1);
	const char* zipDexData = textZipDex.c_str();
	zipDexfile.close();
	
    Dumpdex(DexPath,(char *)keytemp, configPath);
	//加密Dex文件，并将之存入bin文件，并在最后添加文件尾	--zhr
	int cipherLen = (textZipDex.length() + 15) / 16 * 16;
	char *zipDexDatain = new char[cipherLen];
	char *zipDexCipher = new char[cipherLen];
	memset(zipDexDatain, 0, cipherLen);
	memcpy(zipDexDatain, zipDexData, cipherLen);

	m_modeenc.EncryptMode(zipDexDatain, cipherLen, (char*)keytemp, zipDexCipher);
    
	
	bintest.AddCipher(textDex.length(), textZipDex.length(), zipDexCipher);


	delete []zipDexDatain;
	delete []zipDexCipher;
	// delete []OutTest;
	return true;
}

