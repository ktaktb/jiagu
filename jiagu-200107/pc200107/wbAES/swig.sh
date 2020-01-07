#!/bin/bash

swig -c++ -python ge_wb_data.i
g++ -O2 -fPIC -c ge_wb_data.c ge_wb_data_wrap.cxx Global.c Key_expansion.c Matrix_tool.c AES_wb_decrypt_generation.c -I /usr/include/python2.7/
g++ -shared ge_wb_data.o ge_wb_data_wrap.o Global.o Key_expansion.o Matrix_tool.o AES_wb_decrypt_generation.o -o _ge_wb_data.so 
mv -f ./ge_wb_data.py ../apktool/ge_wb_data.py
mv -f ./_ge_wb_data.so ../apktool/_ge_wb_data.so
rm -f *.o