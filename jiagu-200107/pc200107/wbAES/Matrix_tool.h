#ifndef MATRIX_TOOL_H_
#define MATRIX_TOOL_H_

typedef unsigned char uchar;
typedef unsigned int uint;

void random_matrix(int n,unsigned char * result_matrix,unsigned char * result_inverse,int rand_i);

void matrix_reader(int row,int col,const char* file_name,uchar* random_matrix);
void matrix_reader_n(int row,int col,const char* file_name,uchar* random_matrix,int n);
void matrix_multiply(uchar* data,uchar* matrix,int n);
void inv_M_matrix_generation();

#endif
