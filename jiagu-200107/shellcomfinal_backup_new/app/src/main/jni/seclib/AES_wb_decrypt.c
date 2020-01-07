#include "AES_wb_decrypt.h"
#include <android/log.h>
#include <string.h>

#define LOG_TAG "JNI_info_AES"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


inline static void col_row_swap(unsigned char *key);
inline static void swap(unsigned char *a,unsigned char *b);
static void matrix_multiply_128(uchar* data,uchar* matrix);
static inline uint lookup_table(int r,int n,uchar* data,uint* table);

inline static void col_row_swap(unsigned char *key)
{
	swap(key+1,key+4);
	swap(key+2,key+8);
	swap(key+3,key+12);
	swap(key+6,key+9);
	swap(key+7,key+13);
	swap(key+11,key+14);
}

inline static void swap(unsigned char *a,unsigned char *b)
{
	unsigned char temp;

	temp = *a;
	*a = *b;
	*b = temp;
}
static void matrix_multiply_128(uchar* data,uchar* matrix)
{
//	unsigned char uc_shift[4] = {0,0,0,0x80};
//	unsigned char result[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//	unsigned char *p_result;

	__asm__ __volatile__(

//				"MOV r2, %0 \n\t"
//				"LDR r8, [r2] \n\t"
				"MOV r8, #2147483648 \n\t"
				"MOV r5, #32 \n\t"
				"MOV r7, r5 \n\t"
				"MOV r9, #0 \n\t"
				"MOV r6, %1 \n\t"
//				"MOV r5, %3 \n\t"
				"MOV r4, %0 \n\t"
				"LDR r3, [r6] \n\t"
				"REV r3, r3 \n\t"
				"VLD1.16 {Q0}, [r4]! \n\t"
//				"VLD1.16 {Q1}, [r5] \n\t"
				"VEOR q1, q1, q1 \n\t"

				"AND r2, r8, r3 \n\t"
				"CMP r2, r9 \n\t"
				"BEQ zytloop_1 \n\t"
				"VEOR q1, q0, q1 \n\t"
			"zytloop_1: \n\t"
				"SUB r7, r7, #1 \n\t"
				"CMP r7, r9 \n\t"
				"BEQ zytpart_2 \n\t"
				"VLD1.16 {Q0}, [r4]! \n\t"
				"LSR r8, r8, #1 \n\t"
				"AND r2, r8, r3 \n\t"
				"CMP r2, r9 \n\t"
				"BEQ zytloop_1 \n\t"
				"VEOR q1, q0, q1 \n\t"
				"B zytloop_1 \n\t"
			"zytpart_2: \n\t"
				"LDR r3, [r6,#4] \n\t"
				"REV r3, r3 \n\t"
				"MOV r7, r5 \n\t"
//				"MOV r2, %0 \n\t"
//				"LDR r8, [r2] \n\t"
				"MOV r8, #2147483648 \n\t"
				"VLD1.16 {Q0}, [r4]! \n\t"
				"AND r2, r8, r3 \n\t"
				"CMP r2, r9 \n\t"
				"BEQ zytloop_2 \n\t"
				"VEOR q1, q0, q1 \n\t"
			"zytloop_2: \n\t"
				"SUB r7, r7, #1 \n\t"
				"CMP r7, r9 \n\t"
				"BEQ zytpart_3 \n\t"
				"VLD1.16 {Q0}, [r4]! \n\t"
				"LSR r8, r8, #1 \n\t"
				"AND r2, r8, r3 \n\t"
				"CMP r2, r9 \n\t"
				"BEQ zytloop_2 \n\t"
				"VEOR q1, q0, q1 \n\t"
				"B zytloop_2 \n\t"
			"zytpart_3: \n\t"
				"LDR r3, [r6,#8] \n\t"
				"REV r3, r3 \n\t"
				"MOV r7, r5 \n\t"
//				"MOV r2, %0 \n\t"
				"MOV r8, #2147483648 \n\t"
//				"LDR r8, [r2] \n\t"
				"VLD1.16 {Q0}, [r4]! \n\t"
				"AND r2, r8, r3 \n\t"
				"CMP r2, r9 \n\t"
				"BEQ zytloop_3 \n\t"
				"VEOR q1, q0, q1 \n\t"
			"zytloop_3: \n\t"
				"SUB r7, r7, #1 \n\t"
				"CMP r7, r9 \n\t"
				"BEQ zytpart_4 \n\t"
				"VLD1.16 {Q0}, [r4]! \n\t"
				"LSR r8, r8, #1 \n\t"
				"AND r2, r8, r3 \n\t"
				"CMP r2, r9 \n\t"
				"BEQ zytloop_3 \n\t"
				"VEOR q1, q0, q1 \n\t"
				"B zytloop_3 \n\t"
			"zytpart_4: \n\t"
				"LDR r3, [r6,#12] \n\t"
				"REV r3, r3 \n\t"
				"MOV r7, r5\n\t"
//				"MOV r2, %0 \n\t"
				"MOV r8, #2147483648 \n\t"
//				"LDR r8, [r2] \n\t"
				"VLD1.16 {Q0}, [r4]! \n\t"
				"AND r2, r8, r3 \n\t"
				"CMP r2, r9 \n\t"
				"BEQ zytloop_4 \n\t"
				"VEOR q1, q0, q1 \n\t"
			"zytloop_4: \n\t"
				"SUB r7, r7, #1 \n\t"
				"CMP r7, r9 \n\t"
				"BEQ zytfinal \n\t"
				"VLD1.16 {Q0}, [r4]! \n\t"
				"LSR r8, r8, #1 \n\t"
				"AND r2, r8, r3 \n\t"
				"CMP r2, r9 \n\t"
				"BEQ zytloop_4 \n\t"
				"VEOR q1, q0, q1 \n\t"
				"B zytloop_4 \n\t"
			"zytfinal: \n\t"
				"VST1.16 {Q1}, [r6] \n\t"
//				"MOV r5, %0 \n\t"
				:
				:"r"(matrix),"r"(data)
				:"r9","r8","r7","r6","r5","r4","r3","r2"
		);
		//LOGI("dizhi_%x",data[0]);
		//memcpy(data,result,16);
		return;

//	int n = 128;
//	uchar temp[16];
//	int i,j;
//	int n1 = n >> 3;
//
//	for(i = 0;i < 16;i++)
//			memset(&temp[i],0,1);
//	for(i = 0;i < n;i++)
//	{
//		if(0x0 == (((*(data+(i >> 3))) >> (7-(i%8))) & 0x1))
//			continue;
//
//		for(j = 0;j < n1;j++)
//		{
//			temp[j] ^= (*(matrix+i*n1+j));
//		}
//	}
//
//	memcpy(data,temp,n1);

}

//static uint lookup_table(int r,int n,uchar* data,uint* table)
//{
//	uint result,temp = 0;
//	uint temp_1 = 0;
//
//	temp = (*(data));
//	temp_1 = (*(data+1));
//	temp = (temp << 8) | temp_1;
//
//	result = (*(table+r*8*65536+n*65536+temp));
//}
//
//void AES_wb_decrypt(const uchar* c,uchar* p,uchar* M,uint* table)
//{
//	int i,j;
//	uint temp,temp_h,temp_l;
//
//	memcpy(p,c,16);
//	col_row_swap(p);
//	matrix_multiply_128(p,M);
//
//	for(i = 0;i < 10;i++)
//	{
//		for(j = 0;j < 4;j++)
//		{
//			temp_h = lookup_table(i,(j << 1),p+(j << 2),table);
//			temp_l = lookup_table(i,(j << 1)+1,p+(j << 2)+2,table);
//			temp = temp_h ^ temp_l;
//			*(p+(j << 2)) = (temp >> 24) & 0xFF;
//			*(p+(j << 2)+1) = (temp >> 16) & 0xFF;
//			*(p+(j << 2)+2) = (temp >> 8) & 0xFF;
//			*(p+(j << 2)+3) = temp & 0xFF;
//		}
//		matrix_multiply_128(p,(M+128*16*(i+1)));
//	}
//	col_row_swap(p);
//}
static void matrix_multiply(uchar* data,uchar* matrix,int n)
{
	uchar temp[16] = {0x0};
	int i,j;
	int n1 = n >> 3;//计算n是多少字节

	for(i = 0;i < n;i++)
	{
		if(0x0 == (((*(data+(i >> 3))) >> (7-(i%8))) & 0x1))
			continue;

		for(j = 0;j < n1;j++)
		{
			temp[j] ^= (*(matrix+i*n1+j));
			//byte_xor ^= ((((*(data+j/8)) >> (j%8)) & base_value) ^ (((*(matrix+j*n/8+i/8)) >> (i%8)) & base_value));
		}
	}

	for(i = 0;i < n1;i++)
		memset(data+i,temp[i],1);
}

static inline uint lookup_table(int r,int n,uchar* data,uint* table)
{
	uint result;

	result = (*(table+r*16*256+n*256+(*data)));
}

void AES_wb_decrypt(const uchar* c,uchar* p,uchar* M,uint* table)
{
	int i,j;
	uint temp,temp_1,temp_2,temp_3,temp_4;
	int *i16256;
	int j4;
	uchar * pj4;

	memcpy(p,c,16);

//	col_row_swap(p);

//	matrix_multiply(p,M,128);
	matrix_multiply_128(p,M);

	for(i = 0;i < 10;i++)
	{
		i16256 = table+i*16*256;
		for(j = 0;j < 4;j++)
		{
			j4 = j*4;
			pj4 = p + j*4;
			temp_1 = (*(i16256+(j4+0)*256+(*(pj4+0))));
			temp_2 = (*(i16256+(j4+1)*256+(*(pj4+1))));
			temp_3 = (*(i16256+(j4+2)*256+(*(pj4+2))));
			temp_4 = (*(i16256+(j4+3)*256+(*(pj4+3))));
			temp = temp_1 ^ temp_2 ^ temp_3 ^ temp_4;
			*(pj4) = (temp >> 24) & 0xFF;
			*(pj4+1) = (temp >> 16) & 0xFF;
			*(pj4+2) = (temp >> 8) & 0xFF;
			*(pj4+3) = temp & 0xFF;
		}
//		matrix_multiply(p,(M+128*16*(i+1)),128);
		matrix_multiply_128(p,(M+128*16*(i+1)));
	}

//	col_row_swap(p);

}
