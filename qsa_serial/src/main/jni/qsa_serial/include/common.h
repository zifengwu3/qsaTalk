#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <signal.h>
#include <sys/stat.h>
#include <pthread.h>
#include <dirent.h>
#include <semaphore.h>       //sem_t

#define COMMMAX 2048
#define COMMSENDMAX  30

struct commbuf1
{
	int iput;
	int iget;
	int n;
	unsigned char buffer[COMMMAX];
	int stats;
};

struct Multi_Comm_Buff1{
	int isValid;
	int SendNum;
	int m_Comm;
	unsigned char buf[1500];
	int nlength;
	int DelayTime;
};

#ifndef CommonH
#define CommonH

//COMM
int Comm2fd;
int multi_comm_send_flag;
pthread_t multi_comm_send_thread;
void multi_comm_send_thread_func(void);
sem_t multi_comm_send_sem;
struct Multi_Comm_Buff1 Multi_Comm_Buff[COMMSENDMAX];

int OpenComm(int CommPort,int BautSpeed,int databits,int stopbits,int parity);
#else
extern int Comm2fd;
extern int multi_comm_send_flag;
extern pthread_t multi_comm_send_thread;
extern void multi_comm_send_thread_func(void);
extern sem_t multi_comm_send_sem;
extern struct Multi_Comm_Buff1 Multi_Comm_Buff[COMMSENDMAX];

extern int OpenComm(int CommPort,int BautSpeed,int databits,int stopbits,int parity);
#endif
