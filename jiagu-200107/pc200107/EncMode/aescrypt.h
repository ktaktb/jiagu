#pragma once

#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <string.h>
#include <stdio.h>

#define AES_BITS 128
#define MSG_LEN 128

#define SIGNDEX_LEN	20

#define MODE_ECB 0
#define MODE_CBC 1
#define MODE_CFB 2
#define MODE_OFB 3

typedef unsigned char BYTE;

typedef struct amac_ctx
{
	unsigned char cache[16];
	unsigned char key[16];
	unsigned char key_last[16];

	int is_padding;

}amac_ctx;

bool EncryptDex(char* filePath, char* pOutput, char* pKey);
bool SignDex(char* filePath, char* pKey, char* lastKey, char* pOutput);

class EncCipher
{
private:
    unsigned char pIv[AES_BLOCK_SIZE];
public:
	EncCipher();
	~EncCipher();

	bool GenHash(BYTE *pInput, int iInputLen, BYTE *pOutput, int iOutputLen/* = HASH_LEN*/);	//本项目最终未采用Openssl的库进行Mac和SHA1	--zhr
	bool GenMac(BYTE*pInput, int iInputLen, BYTE *btRtKek, BYTE* btLastKey, BYTE *pOutput);
	bool GenSHA1(BYTE* pInput, int iInputLength, BYTE* pOutput);
	bool EncryptAesEcb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);
	bool DecryptAesEcb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);
	bool EncryptAesCbc(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);
	bool DecryptAesCbc(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);
	bool EncryptAesCfb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);
	bool DecryptAesCfb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);
	bool EncryptAesOfb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);  //OFB has the same operation on encryption and decryption	--zhr
	//bool DecryptAesOfb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);

	bool SetIv(char * inIv);
	void amac_init(amac_ctx* amac,unsigned char* key,unsigned char* key_last);
	int amac_calc(amac_ctx* amac, unsigned char *data,unsigned long length);
	void amac_result(amac_ctx* amac,unsigned char *result);

};

class EncMode
{
private:
    EncCipher *mCipher;
    int mMode;
    unsigned char mIv[AES_BLOCK_SIZE];
public:
    EncMode();
    ~EncMode();

    bool SetMode(int iMode);
    bool SetIv(char* inIv);
    bool EncryptMode(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);
    bool DecryptMode(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);
};
