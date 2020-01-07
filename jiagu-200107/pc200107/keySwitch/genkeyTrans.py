import os

cmdswig = "swig -c++ -python keySwitch.i"
cmd_o = "g++ -O2 -fPIC -c KeyTransform.cpp keySwitch_wrap.cxx -I /usr/include/python2.7/"
cmd_so = "g++ -shared keySwitch_wrap.o KeyTransform.o -o _keyTrans.so"

os.system(cmdswig)
os.system(cmd_o)
os.system(cmd_so)