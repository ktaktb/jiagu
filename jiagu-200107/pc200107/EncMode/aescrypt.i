%module aescrypt
%{
#define SWIG_FILE_WITH_INIT
#include"Genbin.h"
#include"Ans_dex.h"
#include"wb_aes.h"
#include"aescrypt.h"
#include<iostream>
#include<fstream>
#include<string>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <stdio.h>
%}

bool EncCipher::DecryptAesEcb(const char *pPlainInput, int InputLen,const char *pkeySet, char *pCryptOutput);