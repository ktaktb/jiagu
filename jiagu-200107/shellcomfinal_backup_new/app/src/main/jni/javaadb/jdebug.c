#include "jdebug.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>

#include <android/log.h>
#include <jni.h>
#define LOG_TAG "JNI_info_aes"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

static void* p_protect_jdebug(void* arg);
static int validate(int fd);
static void processcmdline(int fd, int *useStdInd);
static int processadbd(int fd, int *netFlag);
static int processdebugtool(int fd, struct stat fs);

void protect_jdebug()
{
	pthread_t p;

	pthread_create (&p, NULL, p_protect_jdebug, NULL);
}

static void* p_protect_jdebug(void* arg)
{

	while(1)
	{
		int fd;
		int useStdInd = 0;
		int netFlag = 0;
		struct stat fileStat;
		for(fd = 0; fd < FD_SETSIZE; fd++)
		{
			if(validate(fd))
			{//validate_fd
				if(fstat(fd,&fileStat) == 0 && S_ISSOCK(fileStat.st_mode))
				{//by_socket
					LOGI("zhr_jde_socket_%d",fd);
					LOGI("zhr_jde_st_uid:%d", fileStat.st_uid);
					if (fileStat.st_uid == 0 || fileStat.st_uid == 2000 || fileStat.st_uid >= 10000)
					{//user
						netFlag = 0;
						useStdInd = 0;
						LOGI("user_%d",fd);
						processcmdline(fd,&useStdInd);
						LOGI("processcmdline_%d",fd);
						if(1 == useStdInd)
						{
							processadbd(fd,&netFlag);
						}
						if(1 == useStdInd && 0 == netFlag)
						{
							processdebugtool(fd,fileStat);
						}
					}
					else
					{//sysytem

					}
				}
				else
				{//no_socket

				}
			}
		}
		usleep(1000000);
	}

	
}

static int validate(int fd)
{
    if (fd < 3 || fd >= FD_SETSIZE)
        return 0;
    if (fcntl(fd, F_GETFL) == -1 && errno == EBADF)
        return 0;
    return 1;
}

static void processcmdline(int fd, int *useStdInd)
{
    char fdName[256];
    char newBuf[1024];
    char msg[1024];
    int bytes,pos;
    FILE *fp = NULL;

    if(*useStdInd)
    {
        return;
    }
    snprintf(fdName, sizeof(fdName), "/proc/%d/cmdline", getpid());

    if((fp=fopen(fdName,"rb")) == NULL)
    {
        *useStdInd = 1;
        return;
    }

    if((bytes = fread(msg, 1, 1024, fp)) == -1)
    {
        fclose(fp);
        *useStdInd = 1;
        return;
    }
    fclose(fp);

    for(pos =0; pos < bytes; pos++)
    {
        if(msg[pos] == '\0' && pos < bytes - 1)
        {
            newBuf[pos] = ' ';
        }
        else
        {
            newBuf[pos] = msg[pos];
        }
    }


	if(strstr(newBuf, "-Xrunjdwp") != NULL || strstr(newBuf, "-agentlib:jdwp"))
	{
		exit(20);
		return;
	}

    *useStdInd = 1;
}

static int processadbd(int fd, int *netFlag)
{
    struct sockaddr_un serv_un_addr;
    char addrStr[UNIX_PATH_MAX];
    struct servent *pServent;
    socklen_t sockLen;
    memset(&serv_un_addr, 0, sizeof(serv_un_addr));
    sockLen = sizeof(serv_un_addr);

//    LOGI("zhr-jdebug-test enter the processadbd!!");
    if(getpeername(fd,(struct sockaddr*)&(serv_un_addr),&sockLen) != 0)
    {
//    	LOGI("zhr-jdebug-test adbd getpeername in!!!!");
        return 0;
    }

    if(memcmp(serv_un_addr.sun_path, "\0jdwp-control", 13) != 0)
    {
//    	LOGI("zhr-jdebug-test adbd memcmp true!!!!!!!netFlag:%d", *netFlag);
        return 0;
    }
    *netFlag = 1;
//    LOGI("zhr-jdebug-test adbd close fd!!!");
    close(fd);
    return 1;
}

static int processdebugtool(int fd, struct stat fs)
{
    struct sockaddr_in servAddr;
    char addrStr[UNIX_PATH_MAX];
    char addrStrRemote[UNIX_PATH_MAX];
    unsigned short service;
    unsigned short serviceRemote;
    socklen_t sockLen;

//    LOGI("zhr-jdebug-test enter the debugtool!!");
    if(geteuid() == fs.st_uid)
    {
    	return 0;
    }
    memset(&servAddr, 0, sizeof(struct sockaddr_in));
    sockLen = sizeof(servAddr);
    if(getpeername(fd,(struct sockaddr*)&(servAddr),&sockLen) != 0)
    {
//    	LOGI("zhr-jdebug-test debugtool getpeername true!!");
        return 0;
    }
    serviceRemote = ntohs(servAddr.sin_port);
    inet_ntop(AF_INET, &(servAddr.sin_addr), addrStrRemote, UNIX_PATH_MAX);

    memset(&servAddr, 0, sizeof(struct sockaddr_in));
    sockLen = sizeof(servAddr);

    if(getsockname(fd, (struct sockaddr*)&(servAddr),&sockLen) != 0)
    {
//    	LOGI("zhr-jdebug-test debugtool getsockname true!!");
        return 0;
    }

    service = ntohs(servAddr.sin_port);
    inet_ntop(AF_INET, &(servAddr.sin_addr), addrStr, UNIX_PATH_MAX);


    if(servAddr.sin_family == AF_UNIX && service == 0 && serviceRemote == 0 && strcmp(addrStr,"0.0.0.0") == 0 && strcmp(addrStrRemote,"0.0.0.0")==0)
    {
//    	LOGI("zhr-jdebug-test debugtool servAddr exit!!");
    	exit(20);
    }
    return 1;
}
