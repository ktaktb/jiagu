#include "ndebug.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include <android/log.h>
#include <jni.h>
#define LOG_TAG "JNI_info"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

static char FD5NAME[200];

typedef struct f_parameter_p1
{
	int fd1;
	int fd5;
	pid_t gpid;
}f_parameter_p_kill;

typedef struct p_parameter
{
	pid_t gpid;
	pid_t pid;
	int ms;
	int fd5;
}p_parameter_stop;

static void* p_father_kill(void* arg);
static void* p_moniter_stop(void* arg);
static int setProcessPriority(int pid, int delta);

void protect_ndebug(const char *pkname)
{
//	LOGI("zhr-test-ndebug begin!");
	int fd1[2],fd2[2];
	pid_t c1_pid;
	pid_t buf_c2_pid;
	int res;

	strncpy(FD5NAME,"/data/data/",12);
	strcat(FD5NAME,pkname);
	strcat(FD5NAME,"/fifo5");
	pid_t gpid = getpid();
	if(pipe(fd1) != 0)
	{
		LOGI("fd1_err");
		exit(-1);
	}


	if(pipe(fd2) != 0)
	{
		LOGI("fd2_err");
		exit(-1);
	}

	if(access(FD5NAME, F_OK) != -1)
	{
		unlink(FD5NAME);
		LOGI("access_err_%d",errno);

	}
	res = mkfifo(FD5NAME, S_IRUSR | S_IWUSR);
	if(res != 0)
	{
		LOGI("fifo5_err_%d",errno);
		exit(-1);
	}

	c1_pid = fork();

	if (c1_pid < 0)
	{
		LOGI("c1_err");
		exit(-1);
	}
	else if (c1_pid == 0)
	{//child_1
//		LOGI("zhr-test-ndebug child 1 begin at first!!!!");
		pid_t c2_pid;
		int fd3[2];
		char buf1[1];
		int res;

		if(pipe(fd3) != 0)
		{
			LOGI("fd3_err");
			exit(-1);
		}

		c2_pid = fork();

		if (c2_pid < 0)
		{
			LOGI("c2_err");
			exit(-1);
		}
		else if (c2_pid == 0)
		{//child_2
//			LOGI("zhr-test-ndebug child 2!!!!");
			pthread_t p_moniter_p,p_moniter_c1;
			p_parameter_stop *p_p_s_p,*p_p_s_c1;

			int res;
			fd_set rdst;
			int nfds;
			int fd5 = open(FD5NAME, O_WRONLY);
			if (fd5 < 0)
			{
				exit(-1);
			}

//			LOGI("zhr-test-ndebug child2 set ppsp ppsc1!!!!");
			p_p_s_p = (p_parameter_stop*)malloc(sizeof(p_parameter_stop));
			p_p_s_c1 = (p_parameter_stop*)malloc(sizeof(p_parameter_stop));

			p_p_s_p->gpid = gpid;
			p_p_s_p->pid = gpid;
			p_p_s_p->ms = 10000;
			p_p_s_p->fd5 = fd5;

			p_p_s_c1->gpid = gpid;
			p_p_s_c1->pid = getppid();
			p_p_s_c1->ms = 10000;
			p_p_s_c1->fd5 = fd5;

//			LOGI("zhr-test-ndebug child 2 create pthead p,c1!!!!");
			pthread_create (&p_moniter_p, NULL, &p_moniter_stop, p_p_s_p);
			pthread_create (&p_moniter_c1, NULL, &p_moniter_stop, p_p_s_c1);


			nfds = fd5;
			FD_ZERO(&rdst);
			FD_SET(fd5, &rdst);
			if (select(FD_SETSIZE, &rdst, NULL, NULL, NULL) > 0)
			{
				if (FD_ISSET(nfds, &rdst))
				{
					kill(gpid, SIGKILL);
					close(fd5);
					unlink(FD5NAME);
					pthread_kill(pthread_self(), SIGKILL);
				}

			}

		}
		else
		{//child_1
//			LOGI("zhr-test-ndebug child 1!!!!");
			fd_set rdst;
			int nfds;
			pthread_t p_moniter_p,p_moniter_c2;
			p_parameter_stop *p_p_s_p,*p_p_s_c2;

			setProcessPriority(getpid(), -14);

//			LOGI("zhr-test-ndebug child 1 set ppsp ppsc2!!!!");
			p_p_s_p = (p_parameter_stop*)malloc(sizeof(p_parameter_stop));
			p_p_s_c2 = (p_parameter_stop*)malloc(sizeof(p_parameter_stop));

			p_p_s_p->gpid = gpid;
			p_p_s_p->pid = gpid;
			p_p_s_p->ms = 10000;
			p_p_s_p->fd5 = 0;

			p_p_s_c2->gpid = gpid;
			p_p_s_c2->pid = c2_pid;
			p_p_s_c2->ms = 10000;
			p_p_s_c2->fd5 = 0;

//			LOGI("zhr-test-ndebug child 1 create thread p c2!!!!");
			pthread_create (&p_moniter_p, NULL, &p_moniter_stop, p_p_s_p);
			pthread_create (&p_moniter_c2, NULL, &p_moniter_stop, p_p_s_c2);

			close(fd2[0]);
			res = write(fd2[1],&c2_pid,4);

			close(fd3[0]);

			nfds = fd3[1];
			FD_ZERO(&rdst);
			FD_SET(fd3[1], &rdst);
			if (select(FD_SETSIZE, &rdst, NULL, NULL, NULL) > 0)
			{
				if (FD_ISSET(nfds, &rdst))
				{
					kill(gpid, SIGKILL);
					close(fd3[1]);
					unlink(FD5NAME);
					pthread_kill(pthread_self(), SIGKILL);
				}

			}

		}
	}
	else
	{//father
//		LOGI("zhr-test-ndebug father!!!!");
		pthread_t p_kill,p_moniter_c1,p_moniter_c2;
		f_parameter_p_kill* f_p_p_k;
		p_parameter_stop *p_p_s_c1,*p_p_s_c2;
		int fd5 = open(FD5NAME, O_RDONLY);
		if (fd5 < 0)
		{
			LOGI("father_err_%d",errno);
			exit(-1);
		}

		setProcessPriority(getpid(), -15);

//		LOGI("zhr-test-ndebug father close fd2 fd1!!!!");
		close(fd2[1]);
		close(fd1[0]);

//		LOGI("zhr-test-ndebug father set fppk ppsc1 ppsc2!!!!");
		f_p_p_k = (f_parameter_p_kill*)malloc(sizeof(f_parameter_p_kill));
		p_p_s_c1 = (p_parameter_stop*)malloc(sizeof(p_parameter_stop));
		p_p_s_c2 = (p_parameter_stop*)malloc(sizeof(p_parameter_stop));

		f_p_p_k->fd1 = fd1[1];
		f_p_p_k->fd5 = fd5;
		f_p_p_k->gpid = gpid;

		res = read(fd2[0],&buf_c2_pid,4);

		//LOGI("father_%d",f_p_p_k->fd1);
		p_p_s_c1->gpid = gpid;
		p_p_s_c1->pid = c1_pid;
		p_p_s_c1->ms = 50000;
		p_p_s_c1->fd5 = 0;

		p_p_s_c2->gpid = gpid;
		p_p_s_c2->pid = buf_c2_pid;
		p_p_s_c2->ms = 50000;
		p_p_s_c2->fd5 = 0;

//		LOGI("zhr-test-ndebug father create thread kill c1 c2!!!!");
		pthread_create (&p_kill, NULL, &p_father_kill, f_p_p_k);
		pthread_create (&p_moniter_c1, NULL, &p_moniter_stop, p_p_s_c1);
		pthread_create (&p_moniter_c2, NULL, &p_moniter_stop, p_p_s_c2);

		return;
	}
}

static void* p_father_kill(void* arg)
{
//	LOGI("zhr-test-ndebug p_father_kill!!!!");
	f_parameter_p_kill *f_p_p_k;
	fd_set rdst;
	int nfds;

	f_p_p_k = (f_parameter_p_kill*)arg;

	nfds = f_p_p_k->fd1;

	FD_ZERO(&rdst);
	FD_SET(f_p_p_k->fd1, &rdst);

	if (select(FD_SETSIZE, &rdst, NULL, NULL, NULL) > 0)
	{
		if (FD_ISSET(nfds, &rdst))
		{
			kill(f_p_p_k->gpid, SIGKILL);
			close(f_p_p_k->fd1);
			close(f_p_p_k->fd5);
			unlink(FD5NAME);
//			free(f_p_p_k);
			pthread_kill(pthread_self(), SIGKILL);
		}

	}

    return NULL;
}

static int readProcessStateFromFile(char *state, const char *filePath)
{
    const char sep[] = " ";
    const int sep_size = 1;
    char *strpos = NULL;
    char *target = NULL;
    int cnt;
    int fd = open(filePath, O_RDONLY);
    if (fd < 0)
    {
        return 0;
    }

    char buffer[100] = {'\0'};
    const int len = read(fd, buffer, sizeof(buffer)-1);
    close(fd);
    if (len < 0)
    {
        return 0;
    }

    target = buffer;
    for (cnt = 0;cnt < 2; cnt++)
    {
        strpos = strstr(target, sep);
        target = strpos + sep_size;
    }
    if (strpos)
    {
        *state = target[0];
        return 1;
    }

    return 0;

}

static void* p_moniter_stop(void* arg)
{
	p_parameter_stop* p_p_s;

	char filepathProcStat[100];

	p_p_s = (p_parameter_stop*)arg;
	sprintf(filepathProcStat, "/proc/%d/stat", p_p_s->pid);

	do
	{
		char state;

		if (readProcessStateFromFile(&state, filepathProcStat))
		{

			if (state == 't' || state == 'T'||state == 'z' || state == 'Z')
			{

				if(p_p_s->fd5 != 0)
				{
					unlink(FD5NAME);
				}

//				free(p_p_s);
				kill(p_p_s->pid, SIGKILL);
				kill(p_p_s->gpid, SIGKILL);

			}
			else
			{
				usleep(p_p_s->ms);
			}
		}
//
//		kill(p_p_s->gpid, SIGKILL);
//		kill(p_p_s->pid, SIGKILL);
//		if(p_p_s->fd5 != 0)
//		{
//			close(p_p_s->fd5);
//			unlink(FD5NAME);
//		}
//
//		free(p_p_s);
//		exit(1);


	} while(1);
}


static int setProcessPriority(int pid, int delta)
{
    const int nice = getpriority(PRIO_PROCESS, pid);

    if (setpriority(PRIO_PROCESS, pid, nice + delta))
    {
        return 0;
    }

    return 1;
}

