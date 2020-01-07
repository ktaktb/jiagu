import os

cmdswig = "swig -c++ -python EncMode.i"
cmd_o = "g++ -O2 -fPIC -c aescrypt.cpp EncMode_wrap.cxx EncryptSign.cpp Genbin.cpp wb_aes.cpp Ans_dex.cpp -I /usr/include/python2.7/"
cmd_so = "g++ -shared EncMode_wrap.o aescrypt.o EncryptSign.o wb_aes.o Genbin.o Ans_dex.o -L /usr/local/lib -lcrypto -lssl -o _EncMode.so"

os.system(cmdswig)
os.system(cmd_o)
os.system(cmd_so)
#-I/usr/include/python2.7/