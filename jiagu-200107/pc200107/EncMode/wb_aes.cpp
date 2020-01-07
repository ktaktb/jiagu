#include "wb_aes.h"
#include "aescrypt.h"
//#include <openssl/aes.h>
#include "stdio.h"

void WbAesEncrypt1m(const unsigned char *userk,unsigned char *p, unsigned char *c,int length)
{
	AES_KEY aes_key;
	int i,j;
	unsigned char temp[16];

	i = 0;

	//int n = length / (512*1024);

	//if(n == 0)
	//	n = 1;

	AES_set_encrypt_key(userk,128,&aes_key);

/*	while((length / (16*n)) != 0)
	{
		AES_ecb_encrypt(p+i*16,c+i*16,&aes_key,AES_ENCRYPT);

		i += n;
		length = length - 16*n;
	}*/
	
	while((length / 16) != 0)
	{
		AES_ecb_encrypt(p+i*16,c+i*16,&aes_key,AES_ENCRYPT);
		i += 1;
		length = length - 16;
	}
	
}

// int main()
// {
// 	unsigned char data[32] = {
// 		0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,
// 		0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34,
// 		0x32,0x43,0xf6,0xa8,0x88,0x5a,0x30,0x8d,
// 		0x31,0x31,0x98,0xa2,0xe0,0x37,0x07,0x34};
// 	unsigned char key[16] = {
// 		0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,
// 		0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c};
// 	int i;

// 	WbAesEncrypt1m(key,data,data,32);

// 	for(i = 0; i < 32; i++)
// 	printf("%d  %x\n",i,data[i]);
// 	return 0;
// }