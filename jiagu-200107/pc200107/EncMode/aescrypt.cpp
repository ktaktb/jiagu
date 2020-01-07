#include "aescrypt.h"

EncCipher::EncCipher()
{
    for(int i=0; i<AES_BLOCK_SIZE; ++i) pIv[i] = 0;
}
EncCipher::~EncCipher(){}

bool EncCipher::GenSHA1(BYTE* pInput, int iInputLength, BYTE* pOutput)
{
	SHA_CTX ShaKey;
	
	if(!SHA1_Init(&ShaKey))
		return false;
	if(!SHA1_Update(&ShaKey, pInput, iInputLength))
		return false;
	if(!SHA1_Final(pOutput, &ShaKey))
		return false;
	return true;
};

bool EncCipher::EncryptAesEcb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if(!pPlainInput || !pkeySet || !pCryptOutput) return false;

    AES_KEY AesKey;
    if(AES_set_encrypt_key((unsigned char*)pkeySet, 128, &AesKey) < 0) return false;

    if(InputLen%16 != 0) return false;
	

    for(int i=0; i<InputLen/16; i++)
        AES_ecb_encrypt((unsigned char*)(pPlainInput + i*AES_BLOCK_SIZE), (unsigned char*)(pCryptOutput + i*AES_BLOCK_SIZE), &AesKey, AES_ENCRYPT);

    return true;
};

bool EncCipher::DecryptAesEcb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if(!pPlainInput || !pkeySet || !pCryptOutput) return false;

    AES_KEY AesKey;
    if(AES_set_decrypt_key((unsigned char*)pkeySet, 128, &AesKey) < 0) return false;

    if(InputLen%16 != 0) return false;

    for(int i=0; i<InputLen/16; i++)
        AES_ecb_encrypt((unsigned char*)(pPlainInput + i*AES_BLOCK_SIZE), (unsigned char*)(pCryptOutput + i*AES_BLOCK_SIZE), &AesKey, AES_DECRYPT);

    return true;
}

bool EncCipher::EncryptAesCbc(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if(!pPlainInput || !pkeySet || !pCryptOutput) return false;

	AES_KEY AesKey;
	if(AES_set_encrypt_key((unsigned char*)pkeySet, 128, &AesKey) < 0) return false;
    for(int i=0; i<AES_BLOCK_SIZE; ++i) pIv[i] = 0;
	AES_cbc_encrypt((unsigned char*)pPlainInput, (unsigned char*)pCryptOutput, InputLen, &AesKey, pIv, AES_ENCRYPT);
	return true;
};

bool EncCipher::DecryptAesCbc(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if(!pPlainInput || !pkeySet || !pCryptOutput) return false;
//    unsigned char iv[AES_BLOCK_SIZE];
//    for(int i=0; i<AES_BLOCK_SIZE; ++i) iv[i] = 0;

	AES_KEY AesKey;
	if(AES_set_decrypt_key((unsigned char*)pkeySet, 128, &AesKey) < 0) return false;

	AES_cbc_encrypt((unsigned char*)pPlainInput, (unsigned char*)pCryptOutput, InputLen, &AesKey, pIv, AES_DECRYPT);
	return true;
}

bool EncCipher::EncryptAesCfb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if(!pPlainInput || !pkeySet || !pCryptOutput) return false;

	AES_KEY AesKey;
	int usedBytes = 0;

	memset(&AesKey, 0, sizeof(AES_KEY));

	if(AES_set_encrypt_key((unsigned char*)pkeySet, 128, &AesKey) < 0) return false;

	AES_cfb128_encrypt((unsigned char*)pPlainInput, (unsigned char*)pCryptOutput, InputLen, &AesKey, pIv, &usedBytes, AES_ENCRYPT);
	return true;
};

bool EncCipher::DecryptAesCfb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if(!pPlainInput || !pkeySet || !pCryptOutput) return false;
	AES_KEY AesKey;
	int usedBytes = 0;

	memset(&AesKey, 0, sizeof(AES_KEY));

	if(AES_set_decrypt_key((unsigned char*)pkeySet, 128, &AesKey) < 0) return false;

	AES_cfb128_encrypt((unsigned char*)pPlainInput, (unsigned char*)pCryptOutput, InputLen, &AesKey, pIv, &usedBytes, AES_DECRYPT);
	return true;
}

bool EncCipher::EncryptAesOfb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if(!pPlainInput || !pkeySet || !pCryptOutput) return false;

	AES_KEY AesKey;
	int usedBytes = 0;

	memset(&AesKey, 0, sizeof(AES_KEY));

	if(AES_set_encrypt_key((unsigned char*)pkeySet, 128, &AesKey) < 0) return false;

	AES_ofb128_encrypt((unsigned char*)pPlainInput, (unsigned char*)pCryptOutput, InputLen, &AesKey, pIv, &usedBytes);
	return true;
};

bool EncCipher::SetIv(char * inIv)
{
    if(strlen(inIv) != AES_BLOCK_SIZE) return false;
    for(int i=0; i<AES_BLOCK_SIZE; ++i) pIv[i] = inIv[i];
    return true;
}

bool EncCipher::GenMac( BYTE*pInput, int iInputLen, BYTE *btRtKek, BYTE* btLastKey, BYTE *pOutput )
{
	//pOutput =new BYTE[16];

	amac_ctx ctx;
	amac_init(&ctx, btRtKek, btLastKey);
	amac_calc(&ctx, pInput, iInputLen);
	amac_result(&ctx, pOutput);

	//delete []pOutput;
	return true;
}

bool EncCipher::GenHash( BYTE *pInput, int iInputLen, BYTE *pOutput, int iOutputLen/* = HASH_LEN*/ )
{
	BYTE btShaOutput[20] = {0};//?
	if( false == GenSHA1(pInput, iInputLen, btShaOutput))
		return false;
	memcpy(pOutput, btShaOutput, iOutputLen);
	return true;
}

void EncCipher::amac_init( amac_ctx* amac,unsigned char* key,unsigned char* key_last )
{
	memset(&(amac->cache)[0],0,16);
	memcpy(&(amac->key)[0],key,16);
	memcpy(&(amac->key_last)[0],key_last,16);

	amac->is_padding = 0;
}

int EncCipher::amac_calc( amac_ctx* amac, unsigned char *data,unsigned long length )
{
	int i,j;
	unsigned char temp[16];
	AES_KEY AesKey;

	if(1 == amac->is_padding)
		return 0;

	i = 0;

	while((length / 16) != 0)
	{
		memcpy(&temp[0],(data+i*16),16);

		for(j = 0;j < 16;j++)
		{
			 temp[j] ^= amac->cache[j];
		}

		if(AES_set_encrypt_key(&(amac->key)[0], 128, &AesKey) < 0) return false;
		AES_encrypt(&temp[0],&temp[0],&AesKey);

		memcpy(&(amac->cache)[0],temp,16);

		i++;
		length = length - 16;
	}

	if(length != 0)
	{
		memcpy(&temp[0],(data+i*16),length);

		memset(&temp[length],0,(16-length));

		for(j = 0;j < 16;j++)
		{
			 temp[j] ^= amac->cache[j];
		}

		if(AES_set_encrypt_key(&(amac->key)[0], 128, &AesKey) < 0) return false;
		AES_encrypt(&temp[0],&temp[0],&AesKey);

		memcpy(&(amac->cache[0]),temp,16);

		amac->is_padding = 1;
	}

	return 1;
}

void EncCipher::amac_result( amac_ctx* amac,unsigned char *result )
{
	AES_KEY AesKey;

	AES_set_encrypt_key(&(amac->key_last)[0], 128, &AesKey);
	AES_encrypt(&(amac->cache)[0],&(amac->cache)[0], &AesKey);

	memcpy(result,&(amac->cache)[0],16);
}

//void key_map(unsigned char* data)
//{
//	static unsigned char mask1[16] = {0x00,0x35,0xd0,0xe5,0x3d,0x08,0xed,0xd8,0xe9,0xdc,0x39,0x0c,0xd4,0xe1,0x04,0x31};
//	static unsigned char mask2[16] = {0x00,0x01,0x2e,0x2f,0x49,0x48,0x67,0x66,0x43,0x42,0x6d,0x6c,0x0a,0x0b,0x24,0x25};
//
//	//	static unsigned char mask1[16] = {0x00,0xb4,0xd3,0x67,0xe7,0x53,0x34,0x80,0x92,0x26,0x41,0xf5,0x75,0xc1,0xa6,0x12};
//	//	static unsigned char mask2[16] = {0x00,0xae,0xde,0x70,0xd4,0x7a,0x0a,0xa4,0x93,0x3d,0x4d,0xe3,0x47,0xe9,0x99,0x37};
//	unsigned char mask = 0x0f;
//	//	unsigned char mask[16] = {0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f};
//	//	unsigned char result[16] = {0x0};
//	//	int i;
//	//	__asm__ __volatile__(
//	//
//	//			"MOV r6, %0 \n\t"
//	//			"MOV r5, %1 \n\t"
//	//			"MOV r4, %2 \n\t"
//	//			"MOV r8, %3 \n\t"
//	//			"MOV r7, %4 \n\t"
//	//
//	//			"VLD1.8 {Q4}, [r6] \n\t"
//	//			"VLD1.8 {Q3}, [r5] \n\t"
//	//			"VLD1.8 {Q2}, [r4] \n\t"
//	//			"VLD1.8 {Q1}, [r7] \n\t"
//	//			"VLD1.8 {Q0}, [r7] \n\t"
//	//
//	//			"vshr.u16 q0, q0, #4 \n\t"
//	//			"vand q0, q0, q4 \n\t"
//	//			"vtbl.8 d0, {d6, d7}, d0 \n\t"
//	//			"vtbl.8 d1, {d6, d7}, d1 \n\t"
//	//			"vshl.i16 q1, q1, #4 \n\t"
//	//			"vshr.u16 q1, q1, #4 \n\t"
//	//			"vand q1, q1, q4 \n\t"
//	//			"vtbl.8 d2, {d4, d5}, d2 \n\t"
//	//			"vtbl.8 d3, {d4, d5}, d3 \n\t"
//	//			"VEOR q1, q0, q1 \n\t"
//	//			"VST1.8 {Q1}, [r8] \n\t"
//	//			:
//	//			:"r"(mask),"r"(mask1),"r"(mask2),"r"(result),"r"(data)
//	//			:"r8","r7","r6","r5","r4","r3","r2"
//	//	);
//	//
//	//	for(i = 0;i < 16;i++)
//	//		memset(data+i,result[i],1);
//	unsigned char temp,temp_l,temp_r;
//
//	int i;
//
//	for(i = 0; i < 16; i++)
//	{
//		temp = *(data + i);
//		temp_r = temp & mask;
//		temp_l = temp >> 4;
//		temp_r = mask2[temp_r];
//		temp_l = mask1[temp_l];
//		*(data + i) = temp_l ^ temp_r;
//	}
//}




/*********************************************************************************************************
*
*EncMode part
*
*********************************************************************************************************/
EncMode::EncMode()
{
    for(int i=0; i<AES_BLOCK_SIZE; ++i) mIv[i] =0;
    mMode = MODE_CBC;
    mCipher = new EncCipher();
}

EncMode::~EncMode()
{

}

bool EncMode::SetMode(int iMode)
{
    mMode = iMode;
    if(iMode != MODE_ECB)
        printf("Please call SetIv function!\n Otherwise we will make it all zero!\n");
    return true;
}

bool EncMode::SetIv(char* inIv)
{
    if(strlen(inIv) != AES_BLOCK_SIZE) return false;
    for(int i=0; i<AES_BLOCK_SIZE; ++i) mIv[i] = inIv[i];
    if(mCipher->SetIv(inIv) != true)
        return false;
    return true;
}

bool EncMode::EncryptMode(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if( mMode == MODE_ECB){
        if( (mCipher->EncryptAesEcb(pPlainInput, InputLen, pkeySet, pCryptOutput))!=true )
            return false;
	}
    else if( mMode == MODE_CBC){
        if((mCipher->EncryptAesCbc(pPlainInput, InputLen, pkeySet, pCryptOutput))!= true )
            return false;
	}
    else if( mMode == MODE_CFB){
        if((mCipher->EncryptAesCfb(pPlainInput, InputLen, pkeySet, pCryptOutput))!= true )
            return false;
	}
    else if( mMode == MODE_OFB){
        if((mCipher->EncryptAesOfb(pPlainInput, InputLen, pkeySet, pCryptOutput))!= true )
            return false;
	}


    return true;
}

bool EncMode::DecryptMode(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput)
{
    if( mMode == MODE_ECB){
        if( (mCipher->DecryptAesEcb(pPlainInput, InputLen, pkeySet, pCryptOutput))!=true )
			return false;}
    else if( mMode == MODE_CBC){
        if((mCipher->DecryptAesCbc(pPlainInput, InputLen, pkeySet, pCryptOutput))!= true )
			return false;}
    else if( mMode == MODE_CFB){
        if((mCipher->DecryptAesCfb(pPlainInput, InputLen, pkeySet, pCryptOutput))!= true )
			return false;}
    else if( mMode == MODE_OFB){
        if((mCipher->EncryptAesOfb(pPlainInput, InputLen, pkeySet, pCryptOutput))!= true )
			return false;}

    return true;
}
