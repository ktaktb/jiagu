#include <stdio.h>
#include <fcntl.h>
#include <elf.h>
#include <stdlib.h>
#include <string.h>

typedef struct _SecInfo{
  Elf32_Off secOff;
  Elf32_Word secSize;
}SecInfo;

int so_size = 0;

Elf32_Ehdr ehdr;

static unsigned char s[256]={0};//s-box

//For Test
static void print_all(char *str, int len){
  int i;
  for(i=0;i<len;i++)
  {
    if(str[i] == 0)
      puts("");
    else
      printf("%d \n", str[i]);
  }
}



static int findTargetSectionAddr(const int fd, const char *secName, SecInfo *sec){
  Elf32_Shdr shdr;
  char *shstr = NULL;
  int i;
  unsigned long base, length;
  unsigned short nblock;  
  unsigned short nsize;  
  unsigned char block_size = 16;  
  
  
  lseek(fd, 0, SEEK_SET);
  if(read(fd, &ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)){
    puts("Read ELF header error");
    goto _error;
  }
  
  lseek(fd, ehdr.e_shoff + sizeof(Elf32_Shdr) * ehdr.e_shstrndx, SEEK_SET);
  
  if(read(fd, &shdr, sizeof(Elf32_Shdr)) != sizeof(Elf32_Shdr)){
    puts("Read ELF section string table error");
    goto _error;
  }
  
  if((shstr = (char *) malloc(shdr.sh_size)) == NULL){
    puts("Malloc space for section string table failed");
    goto _error;
  }
  
  lseek(fd, shdr.sh_offset, SEEK_SET);
  if(read(fd, shstr, shdr.sh_size) != shdr.sh_size){
    puts(shstr);
    puts("Read string table failed");
    goto _error;
  }
  
  lseek(fd, ehdr.e_shoff, SEEK_SET);
  for(i = 0; i < ehdr.e_shnum; i++){
    if(read(fd, &shdr, sizeof(Elf32_Shdr)) != sizeof(Elf32_Shdr)){
      puts("Find section .text procedure failed");
      goto _error;
    }
    if(strcmp(shstr + shdr.sh_name, secName) == 0){
      printf("Find section %s, addr = 0x%x\n", secName, shdr.sh_offset);
      printf("Find section %s, size = 0x%x\n", secName, shdr.sh_size);
      sec->secOff = shdr.sh_offset;
      sec->secSize = shdr.sh_size;
	  //ehdr.e_entry = shdr.sh_offset;
	  //ehdr.e_shoff = shdr.sh_offset;
	  //ehdr.e_shentsize = shdr.sh_size/65535;
	  //ehdr.e_shstrndx = shdr.sh_size%65535;
      break;
    }
  }
  
//  lseek(fd, 0, SEEK_SET);
//  if(write(fd, &ehdr, sizeof(Elf32_Ehdr)) != sizeof(Elf32_Ehdr)){
//    puts("Read ELF header error");
//    goto _error;
//  }

  
  free(shstr);
  return 0;
_error:
  return -1;
}



void rc4_init(unsigned char* pKey, unsigned long ulKeyLen, unsigned char* pSBox)
{
    printf("rc4_init\n");

 //   print_all(pKey, ulKeyLen);

    int i = 0,j = 0;
    char k[256]={0};
    unsigned char tmp = 0;
    for(i = 0; i < 256; i++)
    {
        pSBox[i] = i;
        k[i] = pKey[i % ulKeyLen];
    }
    for(i = 0;i < 256; i++)
    {
        j = (j + pSBox[i] + k[i]) % 256;
        tmp = pSBox[i];
        pSBox[i] = pSBox[j];//exchange pSBox[i] and pSBox[j]
        pSBox[j] = tmp;
    }
    //printf("sbox = %d\n", pSBox[0]);
}

void rc4_crypt(unsigned char *pSBox, unsigned char *pData,unsigned long ulDataLen,unsigned char *pDataEnc,unsigned long ulcrypt_offset){
    printf("in function rc4_crypt\n");
    int i = 0,j = 0,t = 0;
    unsigned long k = 0;
    unsigned char tmp={0};
    for(k = 0;k < ulcrypt_offset+ulDataLen; k++)
    {
        i = (i + 1) % 256;
        j = (j + pSBox[i]) % 256;
        tmp = pSBox[i];
        pSBox[i] = pSBox[j];//exchange pSBox[i] and pSBox[j]
        pSBox[j] = tmp;
        t= (pSBox[i] + pSBox[j]) % 256;
        if (k>=ulcrypt_offset)
        {//find the correct offset to xor
            pDataEnc[k-ulcrypt_offset]=pData[k-ulcrypt_offset]^pSBox[t];
        }
    }
}

int main(int argc, char **argv){
  puts("Start encryt app’s so!");
  char secName[] = ".text";
  char funcName[] = "JNI_OnLoad";
  // char funcName[9][30] = {"JNI_OnLoad", "protect_ndebug", "protect_jdebug", 
  //                          "decrypt", "start_hook_mmap", "start_hook_write", 
  //                          "SetUpDexClassLoader", "SetUpARTDexClassLoader", 
  //                          "dvmContinueOptimization"};

//  char funcName[] = "Java_com_thomas_crackmeso_MainActivity_verify";
  char *textcont = NULL;
  char *enctext = NULL;
  int fd, i, k;
  SecInfo  sec;
  
  char addr[8];
  char size[8];
  
  if(argc < 3){
    puts("Usage: shellAdder2 encKey libxxx.so .(section) function");
    return -1;
  }

  // printf("filename = %s\n", argv[2]);

  fd = open(argv[2], O_RDWR);
  if(fd < 0){
    printf("open %s failed!\n", argv[2]);
    goto _error;
  }
  


  char *key = argv[1];

  rc4_init((unsigned char *)key, strlen(key), s);
  unsigned char sTemp[256]={0};
  memcpy(sTemp,s,256);

  // for (i = 0; i < 9; ++i)
  // {


  if(findTargetSectionAddr(fd, secName, &sec) == -1){
      printf("Find section %s failed\n", secName);
      goto _error;
  }

  textcont = (char*) malloc(sec.secSize);
  enctext = (char*) malloc(sec.secSize);
  lseek(fd, sec.secOff, SEEK_SET);
  if(read(fd, textcont, sec.secSize) != sec.secSize){
      puts("Malloc space failed");
      goto _error;
    }
/*
	for(k = 0;k<sec.secSize;k++){
		enctext[k] = ~textcont[k];
	}
*/
  rc4_crypt(sTemp, (unsigned char *)textcont, sec.secSize, (unsigned char *)enctext, 0);

  lseek(fd, sec.secOff, SEEK_SET);
  if(write(fd, enctext, sec.secSize) != sec.secSize){
      puts("Write modified content to .so failed");   
  }
  close(fd);
  
  
  
  fd = open(argv[2], O_WRONLY|O_APPEND);
  if(fd < 0){
	  puts("open failed");
  }
  
  sprintf(addr, "%04x", sec.secOff);
  puts(addr);
  if(write(fd, addr, sizeof(addr)) != sizeof(addr)){
	  puts("Write secOff failed");
  }
  
  sprintf(size, "%04x", sec.secSize);
  puts(size);
  if(write(fd, size, sizeof(size)) != sizeof(size)){
	  puts("Write secSize failed");
  }
  
puts("Complete encryt app’s so!");
_error:
  free(textcont);
  free(enctext);
  close(fd);
  return 0;
}
