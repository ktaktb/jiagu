#include "Matrix_tool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

static void set_inverse_SM(uchar* SM);
static void set_SM(uchar* SM);
static void set_8E_matrix(uchar* EM,int n);
static int get_inverse(int n,unsigned char M[128][128],unsigned char E[128][128]);

static int get_inverse(int n,unsigned char M[128][128],unsigned char E[128][128])    //求逆矩阵
{
	int i,j,k,l,t;
	unsigned char max,temp;
	for (i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
		{
			if(i==j)
				E[i][j] = 1;
			else
				E[i][j] = 0;
		}
	}
	for(l=0;l<n;l++)
	{
		t = l;
		max = M[t][t];
		k = t;
		for (i = t+1 ;i < n; i++)
		{
			if(max == 0x1)
				break;
			if (max<M[i][t])
			{
				max = M[i][t];  
				k = i;		
				break;
			}
		}
		if (max==0)
			return 0;
		if (t!=k)
		{
			for (j = 0; j < n; j++)
			{
					temp = M[t][j];
					M[t][j] = M[k][j];
					M[k][j] = temp;

					temp = E[t][j];
					E[t][j] = E[k][j];
					E[k][j] = temp; 
			}
		}
		for(i=0;i<n;i++)
		{
			if(t!=i && M[i][t]==1)
			{
				for(j=0;j<n;j++)
				{
					M[i][j] = M[i][j] ^ M[t][j];
					E[i][j] = E[i][j] ^ E[t][j];
				}
			}
		}
	}
	return 1;	
}

void random_matrix(int n,unsigned char * result_matrix,unsigned char * result_inverse,int rand_i)
{
	int i,j,have_inverse = 0;
	struct timeval t1;
	unsigned char M[128][128], E[128][128];
	unsigned char rand_num;
	gettimeofday(&t1,NULL);
	srand(t1.tv_usec+rand_i);
	while(!have_inverse)
	{
		for(i=0;i<n;i++)
		{
			for(j=0;j<n;j++)
			{
				M[i][j] = rand() % 2;
			}
		}
		for(i = 0; i < n; i++)
		{
			memcpy(result_matrix+n*i,&M[0][0]+128*i,n);
		}
		have_inverse = get_inverse(n,M,E);
	}
	for(i = 0; i < n; i++)
	{
		memcpy(result_inverse+n*i,&E[0][0]+128*i,n);
	}
}
void matrix_reader(int row,int col,const char* file_name,uchar* random_matrix)
{
	FILE *fp;
	uchar* matrix_temp;
	int i,j;

	matrix_temp = (uchar*)malloc(row*col);
	memset(random_matrix,0,row*col/8);

	if((row == 128)/* || (row == 32) || (row == 8)*/)
	{
		fp = fopen(file_name,"r");
		fread(random_matrix,sizeof(uchar),row*col/8,fp);
		fclose(fp);
	}
	else
	{
		fp = fopen(file_name,"r");
		fread(matrix_temp,sizeof(uchar),row*col,fp);
		fclose(fp);

		for(i = 0;i < row;i++)
		{
			for(j = 0;j < col;j++)
			{
				(*(random_matrix+i*col/8+j/8)) |= (*(matrix_temp+i*col+j)) << (7-j%8);
			}
		}
	
	}
}
void matrix_reader_n(int row,int col,const char* file_name,uchar* random_matrix,int n)
{
	FILE *fp;
	uchar* matrix_temp;
	int i,j;

	matrix_temp = (uchar*)malloc(row*col);
	memset(random_matrix,0,row*col/8);

	if((row == 128)/* || (row == 32) || (row == 8)*/)
	{
		fp = fopen(file_name,"rb");
		fread(random_matrix,sizeof(uchar),row*col/8,fp);
		fclose(fp);
	}
	else
	{
		fp = fopen(file_name,"rb");
		fseek(fp,n*row*col,SEEK_SET);
		fread(matrix_temp,sizeof(uchar),row*col,fp);
		fclose(fp);

		for(i = 0;i < row;i++)
		{
			for(j = 0;j < col;j++)
			{
				(*(random_matrix+i*col/8+j/8)) |= (*(matrix_temp+i*col+j)) << (7-j%8);
			}
		}
	
	}
}

void inv_M_matrix_generation()
{
	uchar G[128][16],F[128][16],SM[128][16],M[11][128][16],
	      M_temp[128][16],L_temp[8][1],R_temp[32][4];
	int t,i,j;
	FILE *fp;
	
	memset(&M_temp[0][0],0,128*16);
	memset(&M[0][0][0],0,11*128*16);
	matrix_reader(128,128,"G.m",&G[0][0]);
	matrix_reader(128,128,"F.m",&F[0][0]);
	set_inverse_SM(&SM[0][0]);
	
/*	for(i = 0;i < 128;i++)
	{
		for(j = 0;j < 16;j++)
			memset(&M[0][i][j],G[i][j],1);
	}
*/
	memcpy(&M[0][0][0],&G[0][0],128*16);

	for(i = 0;i < 16;i++)
	{
		matrix_reader_n(8,8,"L_inv.m",&L_temp[0][0],i);
		for(j = 0;j < 8;j++)
		{
			memset(&M_temp[8*i+j][i],L_temp[j][0],1);
		}
	}
	for(i = 0;i < 128;i++)
	{
		matrix_multiply(&M[0][i][0],&SM[0][0],128);
		matrix_multiply(&M[0][i][0],&M_temp[0][0],128);
	}
	for(t = 1;t < 11;t++)
	{
		for(i = 0;i < 4;i++)
		{
			matrix_reader_n(32,32,"R_inv.m",&R_temp[0][0],4*(t-1)+i);
			for(j = 0;j < 32;j++)
			{
				memset(&M[t][32*i+j][4*i],R_temp[j][0],1);
				memset(&M[t][32*i+j][4*i+1],R_temp[j][1],1);
				memset(&M[t][32*i+j][4*i+2],R_temp[j][2],1);
				memset(&M[t][32*i+j][4*i+3],R_temp[j][3],1);  
			}
		}
		if(10 == t)
		{
			for(i = 0;i < 128;i++)
			{
				matrix_multiply(&M[10][i][0],&F[0][0],128);
			}		
		}
		else
		{
			for(i = 0;i < 16;i++)
			{
				matrix_reader_n(8,8,"L_inv.m",&L_temp[0][0],16*t+i);
				for(j = 0;j < 8;j++)
				{
					memset(&M_temp[8*i+j][i],L_temp[j][0],1);
				}
			}
			for(i = 0;i < 128;i++)
			{
				matrix_multiply(&M[t][i][0],&SM[0][0],128);
				matrix_multiply(&M[t][i][0],&M_temp[0][0],128);
			}			
		}
	}
	fp = fopen("inv_M.m","w");
	fwrite(&M[0][0][0],sizeof(uchar),11*128*16,fp);
	fclose(fp);
	
}


static void set_inverse_SM(uchar* SM)
{
	int n = 16;
	int row = 128;
	memset(SM,0,row*n);
/*
	set_8E_matrix(SM+0*n+0,128);
	set_8E_matrix(SM+8*n+1,128);
	set_8E_matrix(SM+16*n+2,128);
	set_8E_matrix(SM+24*n+3,128);
	set_8E_matrix(SM+32*n+5,128);
	set_8E_matrix(SM+40*n+6,128);
	set_8E_matrix(SM+48*n+7,128);
	set_8E_matrix(SM+56*n+4,128);
	set_8E_matrix(SM+64*n+10,128);
	set_8E_matrix(SM+72*n+11,128);
	set_8E_matrix(SM+80*n+8,128);
	set_8E_matrix(SM+88*n+9,128);
	set_8E_matrix(SM+96*n+15,128);
	set_8E_matrix(SM+104*n+12,128);
	set_8E_matrix(SM+112*n+13,128);
	set_8E_matrix(SM+120*n+14,128);
*/
	set_8E_matrix(SM+0*n+0,128);
	set_8E_matrix(SM+8*n+5,128);
	set_8E_matrix(SM+16*n+10,128);
	set_8E_matrix(SM+24*n+15,128);
	set_8E_matrix(SM+32*n+4,128);
	set_8E_matrix(SM+40*n+9,128);
	set_8E_matrix(SM+48*n+14,128);
	set_8E_matrix(SM+56*n+3,128);
	set_8E_matrix(SM+64*n+8,128);
	set_8E_matrix(SM+72*n+13,128);
	set_8E_matrix(SM+80*n+2,128);
	set_8E_matrix(SM+88*n+7,128);
	set_8E_matrix(SM+96*n+12,128);
	set_8E_matrix(SM+104*n+1,128);
	set_8E_matrix(SM+112*n+6,128);
	set_8E_matrix(SM+120*n+11,128);
}

static void set_SM(uchar* SM)
{
	int n = 16;
	int row = 128;
	memset(SM,0,row*n);
/*
	set_8E_matrix(SM+0*n+0,128);
	set_8E_matrix(SM+8*n+1,128);
	set_8E_matrix(SM+16*n+2,128);
	set_8E_matrix(SM+24*n+3,128);
	set_8E_matrix(SM+32*n+7,128);
	set_8E_matrix(SM+40*n+4,128);
	set_8E_matrix(SM+48*n+5,128);
	set_8E_matrix(SM+56*n+6,128);
	set_8E_matrix(SM+64*n+10,128);
	set_8E_matrix(SM+72*n+11,128);
	set_8E_matrix(SM+80*n+8,128);
	set_8E_matrix(SM+88*n+9,128);
	set_8E_matrix(SM+96*n+13,128);
	set_8E_matrix(SM+104*n+14,128);
	set_8E_matrix(SM+112*n+15,128);
	set_8E_matrix(SM+120*n+12,128);
*/
	set_8E_matrix(SM+0*n+0,128);
	set_8E_matrix(SM+8*n+13,128);
	set_8E_matrix(SM+16*n+10,128);
	set_8E_matrix(SM+24*n+7,128);
	set_8E_matrix(SM+32*n+4,128);
	set_8E_matrix(SM+40*n+1,128);
	set_8E_matrix(SM+48*n+14,128);
	set_8E_matrix(SM+56*n+11,128);
	set_8E_matrix(SM+64*n+8,128);
	set_8E_matrix(SM+72*n+5,128);
	set_8E_matrix(SM+80*n+2,128);
	set_8E_matrix(SM+88*n+15,128);
	set_8E_matrix(SM+96*n+12,128);
	set_8E_matrix(SM+104*n+9,128);
	set_8E_matrix(SM+112*n+6,128);
	set_8E_matrix(SM+120*n+3,128);
}

static void set_8E_matrix(uchar* EM,int n)
{
	int i;
	unsigned char base_value = 0x80;

	for(i = 0; i < 8;i++)
	{	
		(*(EM+i*n/8)) ^= (base_value >> i);
	}
}

void matrix_multiply(uchar* data,uchar* matrix,int n)
{
	uchar temp[16] = {0x0};
	int i,j;
	int n1 = n >> 3;
	
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


