%module EncMode
%{
#define SWIG_FILE_WITH_INIT
#include"Genbin.h"
#include"aescrypt.h"
#include<iostream>
#include<fstream>
#include<string>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <stdio.h>
%}

extern bool GenBinFlow(char* SoPath, char* SoX86Path, char* DexPath, char* zipDexPath, char* fakeDexPath, char* KeyInput, char* KeySwitch, char* configPath);