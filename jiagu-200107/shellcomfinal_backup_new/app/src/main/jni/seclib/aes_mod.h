#ifndef AES_MOD_H_
#define AES_MOD_H_

void AES_Encrypt_128_ECB(const unsigned char *userk,const unsigned char *p,unsigned char *c,int length);
void AES_Decrypt_128_ECB(const unsigned char *userk,const unsigned char *c,unsigned char *p,int length);
void AES_Encrypt_128_CBC(const unsigned char *userk,const unsigned char *iv,const unsigned char *p,unsigned char *c,int length);
void AES_Decrypt_128_CBC(const unsigned char *userk,const unsigned char *iv,const unsigned char *c,unsigned char *p,int length);
void AES_Encrypt_128_OFB(const unsigned char *userk,const unsigned char *iv,const unsigned char *in,unsigned char *out,int len);

#endif
