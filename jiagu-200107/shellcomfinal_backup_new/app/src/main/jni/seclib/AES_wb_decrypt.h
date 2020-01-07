#ifndef AES_WB_DECRYPT_H_
#define AES_WB_DECRYPT_H_

typedef unsigned char uchar;
typedef unsigned int uint;

void AES_wb_decrypt(const uchar* c,uchar* p,uchar* M,uint* table);

#endif
