#include "Key_expansion.h"
#include "Global.h"

static void ByteToBits(unsigned char ch, unsigned char bit[]);
static void RotWord(unsigned char c[], unsigned char temp[]);
static void SubWord(unsigned char temp[], unsigned char bit[]);
/*字节转换为二进制位存放在数组中*/
static void ByteToBits(unsigned char ch, unsigned char bit[])
{
	int i;
	for(i = 0; i < 8; i++)
	    bit[i] = ( ch >> i) & 1;
}

/*位置变换*/
static void RotWord(unsigned char c[], unsigned char temp[])
{
	int i;
	for(i = 1; i < 4; i++)
		temp[i - 1] = c[i];
	temp[3] = c[0];
}

/*小S盒变换*/ 
static void SubWord( unsigned char temp[], unsigned char bit[])
{
	int i;
	for(i = 0; i < 4; i++)
	{
		ByteToBits(temp[i],bit);
		temp[i] = s[( bit[7] * 8 + bit[6] * 4 + bit[5] * 2 + bit[4] )][( bit[3] * 8 + bit[2] * 4 + bit[1] * 2 + bit[0] )];
	}
}

/*密钥扩展函数*/
void KeyExpansion(unsigned char *k, unsigned char key[][4],unsigned char flag)
{
	int i, j;
	unsigned char t_swap;
	unsigned char temp[4];
	unsigned char bit[8];

	//col_row_swap(k);
	for(i = 0; i < 44; i++)
	{
	    for(j = 0; j < 4; j++)
	    {
	    	if(i < 4) 
			key[i][j] = *( k + 4 * i + j);
	    	else if((i != 0)&&(i % 4 == 0))
			{
	    		RotWord(key[i - 1], temp);
	    		SubWord(temp, bit);
	    		key[i][j] = key[i - 4][j] ^ temp[j] ^ Rcon[i/4][j];
	    	}
 	        else  key[i][j] = key[i - 1][j] ^ key[i - 4][j];
	    }
    }
	for(i = 0;i < 11;i++)
	{
		col_row_swap(&key[4*i][0]);
	}
	if(flag == 0)
		return;
	else
	{
		for(i = 0;i < 5;i++)
		{
			for(j = 0;j < 16;j++)
			{
				t_swap = *(&key[0][0]+i*16+j);
				*(&key[0][0]+i*16+j) = *(&key[0][0]+(10-i)*16+j);
				*(&key[0][0]+(10-i)*16+j) = t_swap;
			}
		}
	}
}
