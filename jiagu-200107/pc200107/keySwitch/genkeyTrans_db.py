import os

cmdswig = "swig -c++ -python keySwitch_db.i"
cmd_o = "g++ -O2 -fPIC -c KeyTransform_db.cpp keySwitch_db_wrap.cxx -I /usr/include/python2.7/"
cmd_so = "g++ -shared keySwitch_db_wrap.o KeyTransform_db.o -o _keyTrans_db.so"

os.system(cmdswig)
os.system(cmd_o)
os.system(cmd_so)