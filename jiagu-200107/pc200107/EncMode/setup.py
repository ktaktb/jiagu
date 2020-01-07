from distutils.core import setup, Extension


#EncMode_module=Extension('_EncMode',sources=['EncMode_wrap.cxx','aescrypt.cpp','Ans_dex.cpp','EncryptSign.cpp','Genbin.cpp','wb_aes.cpp'],)
EncMode_module=Extension('_aescrypt',sources=['aescrypt_wrap.cxx','aescrypt.cpp'],)

# setup (
       # name='EncMode',
	   # version='0.1',
	   # author='SWIG Docs',
	   # description="try",
	   # ext_modules=[EncMode_module],
	   # py_modules=["EncMode"],
	   # )
	   
	   
setup (
       name='aescrypt',
	   version='0.1',
	   author='SWIG Docs',
	   description="try",
	   ext_modules=[EncMode_module],
	   py_modules=["aescrypt"],
	   )