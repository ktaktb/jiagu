#include "AES_wb_decrypt_generation.h"
#include <stdio.h>
#include <stdlib.h>
#include "Matrix_tool.h"
#include "Global.h"

static uint invMixcol_1(const unsigned char c);
static uint invMixcol_2(const unsigned char c);
static uint invMixcol_3(const unsigned char c);
static uint invMixcol_4(const unsigned char c);
static unsigned char ByteMultiply(unsigned char c);
static unsigned char inv_T_box(unsigned char byte,unsigned char* key,int r,int x,int y);
static void table_generation(int r,int n,uint* table,unsigned char* key);

static uint invMixcol_1(const unsigned char c)
{
	unsigned char temp;
	uint result = 0;
	uint result_temp = 0;

	temp =  (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(ByteMultiply(c))^ByteMultiply(c)); //0x0E乘法
	result_temp = temp;
	result |= result_temp << 24;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ c); //0x09乘法
	result_temp = temp;
	result |= result_temp << 16;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(ByteMultiply(c)) ^ c); //0x0D乘法
	result_temp = temp;
	result |= result_temp << 8;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(c) ^ c );//0x0B乘法
	result_temp = temp;
	result |= result_temp;

	return result;
}

static uint invMixcol_2(const unsigned char c)
{
	unsigned char temp;
	uint result = 0;
	uint result_temp = 0;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(c) ^ c ); //0x0B乘法
	result_temp = temp;
	result |= result_temp << 24;

	temp =  (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(ByteMultiply(c))^ByteMultiply(c)); //0x0E乘法
	result_temp = temp;
	result |= result_temp << 16;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ c); //0x09乘法
	result_temp = temp;
	result |= result_temp << 8;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(ByteMultiply(c)) ^ c); //0x0D乘法
	result_temp = temp;
	result |= result_temp;

	return result;
}

static uint invMixcol_3(const unsigned char c)
{
	unsigned char temp;
	uint result = 0;
	uint result_temp = 0;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(ByteMultiply(c)) ^ c); //0x0D乘法
	result_temp = temp;
	result |= result_temp << 24;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(c) ^ c ); //0x0B乘法
	result_temp = temp;
	result |= result_temp << 16;

	temp =  (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(ByteMultiply(c))^ByteMultiply(c)); //0x0E乘法
	result_temp = temp;
	result |= result_temp << 8;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ c); //0x09乘法
	result_temp = temp;
	result |= result_temp;

	return result;
}

static uint invMixcol_4(const unsigned char c)
{
	unsigned char temp;
	uint result = 0;
	uint result_temp = 0;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ c); //0x09乘法
	result_temp = temp;
	result |= result_temp << 24;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(ByteMultiply(c)) ^ c); //0x0D乘法
	result_temp = temp;
	result |= result_temp << 16;

	temp = (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(c) ^ c ); //0x0B乘法
	result_temp = temp;
	result |= result_temp << 8;

	temp =  (ByteMultiply(ByteMultiply(ByteMultiply(c))) ^ ByteMultiply(ByteMultiply(c))^ByteMultiply(c)); //0x0E乘法
	result_temp = temp;
	result |= result_temp;

	return result;
}

static unsigned char ByteMultiply(unsigned char c)
{
	unsigned char temp;
	temp = c << 1;
	if(c & 0x80)
	{
		temp ^= 0x1b;
	}
	return temp;
}

static unsigned char inv_T_box(unsigned char byte,unsigned char* key,int r,int x,int y)
{
	unsigned char temp,temp_l,temp_r;

	if(0 != r)
	{
		temp = byte;
		temp_l = temp >> 4;
		temp_r = temp & 0x0F;
		temp = 	inv_s[temp_l][temp_r];
		temp ^= (*(key+4*x+y));
	}
	else
	{
		temp = byte ^ (*(key+4*x+(y+4-x)%4));
		temp_l = temp >> 4;
		temp_r = temp & 0x0F;
		temp = 	inv_s[temp_l][temp_r];
		temp ^= (*(key+16+4*x+y));	
	}
	
	return temp;	
}

static void table_generation(int r,int n,uint* table,unsigned char* key)
{
	uint temp,temp_t;
	uint i;
	uchar data,data_temp[4];
	uchar L[8][1],R[32][4];
	matrix_reader_n(8,8,"L.m",&L[0][0],r*16+n);
	matrix_reader_n(32,32,"R.m",&R[0][0],r*4+n/4);

	for(i = 0;i < 256;i++)
	{
		data = i;
		matrix_multiply(&data,&L[0][0],8);
		 
		data = inv_T_box(data,key,r,n%4,n/4);

		switch(n%4)
		{
			case 0:
			{
				if(9 == r)
				{
					temp = data;
					temp = (temp << 24);
				}
				else
					temp = invMixcol_1(data);
				break;
			}
			case 1:
			{
				if(9 == r)
				{
					temp = data;					
					temp = (temp << 16);
				}
				else
					temp = invMixcol_2(data);
				break;
			}
			case 2:
			{
				if(9 == r)
				{
					temp = data;
					temp = (temp << 8);
				}
				else
					temp = invMixcol_3(data);
				break;
			}
			case 3:
			{
				if(9 == r)
				{
					temp = data;
				}
				else
					temp = invMixcol_4(data);
				break;
			}
		}

		data_temp[0] = (temp >> 24) & 0xFF;
		data_temp[1] = (temp >> 16) & 0xFF;
		data_temp[2] = (temp >> 8) & 0xFF;
		data_temp[3] = temp & 0xFF;
		matrix_multiply(&data_temp[0],&R[0][0],32);
		temp = 0;
		temp_t = data_temp[0];
		temp |= (temp_t << 24);
		temp_t = data_temp[1];
		temp |= (temp_t << 16);
		temp_t = data_temp[2];
		temp |= (temp_t << 8);
		temp_t = data_temp[3];
		temp |= (temp_t);
		(*(table+i)) = temp;
	}
}

void AES_wb_decrypt_generation(unsigned char* key)
{
	uint* table;
	int i,j;
	FILE *fp;
	table = (uint*)malloc(10*16*256*sizeof(uint));

	inv_M_matrix_generation();

	for(i = 0;i < 10;i++)
	{
		for(j = 0;j < 16;j++)
		{
			if(0 == i)
				table_generation(i,j,(table+i*16*256+j*256),(key+16*i));
			else
				table_generation(i,j,(table+i*16*256+j*256),(key+16+16*i));
		}
	}
	
	fp = fopen("inv_tables.t","w");
	fwrite(table,sizeof(uint),10*16*256,fp);
	fclose(fp);
	free(table);
}
