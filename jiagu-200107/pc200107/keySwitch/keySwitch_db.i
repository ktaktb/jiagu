%module keyTrans_db
%{
#include "KeyTransform.h"
#include <string>
#include <fstream>
#include <ctime>
#include <stdlib.h>
#include <string.h>	
%}

using namespace std;


char* Keyswitch( char* keyIn, char* SoPath, char* SoPathx86, char* switchFile );