%module ge_wb_data
%{
#define SWIG_FILE_WITH_INIT
#include "ge_wb_data.h"
#include <stdio.h>
#include "Global.h"
#include "Matrix_tool.h"
#include "Key_expansion.h"
#include "AES_wb_decrypt_generation.h"
%}

extern void GeWBData(const char *key);
