#include "ge_wb_data.h"
#include <stdio.h>

#include "Global.h"
#include "Matrix_tool.h"
#include "Key_expansion.h"
#include "AES_wb_decrypt_generation.h"

void GeWBData(const char *key)
{
	uchar t_key[44][4];
	unsigned char mat[32][32],inv_mat[32][32];
	FILE *fp_mat;
	FILE *fp_inv_mat;
	int i;

	KeyExpansion((unsigned char *)key,t_key,1);	

	fp_mat = fopen("R.m","wb");
	fp_inv_mat = fopen("R_inv.m","wb");
	
	for(i = 0; i < 40; i++)
	{
		random_matrix(32,&mat[0][0],&inv_mat[0][0],i);	
		fwrite(mat,1,32*32,fp_mat);
		fwrite(inv_mat,1,32*32,fp_inv_mat);	
	}

	fclose(fp_mat);
	fclose(fp_inv_mat);

	fp_mat = fopen("L.m","wb");
	fp_inv_mat = fopen("L_inv.m","wb");
	
	for(i = 0; i < 160; i++)
	{
		random_matrix(8,&mat[0][0],&inv_mat[0][0],i);	
		fwrite(mat,1,8*8,fp_mat);
		fwrite(inv_mat,1,8*8,fp_inv_mat);	
	}
	fclose(fp_mat);
	fclose(fp_inv_mat);
	
	AES_wb_decrypt_generation(&t_key[0][0]);

}
