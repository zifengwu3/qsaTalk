/************************************************************
 *  File:
 *  Author:
 *  Version :
 *  Date:
 *  Description:
 *  History:
 *            <Zhou Xiaomin> <2015-12-02>
 ***********************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "include/common.h"

#include "../include/log_jni.h"


void OnlineCheckFunc(void);
void TalkCtrlFunc(void);

int timer_fd;
int timer_thread_flag;
pthread_t timer_thread;
void timer_thread_task(void);

int Init_Timer(void);
int Uninit_Timer(void);

extern void Talk_Call_Task(int uFlag, const char *call_addr, const char *call_ip);
extern void Talk_Call_End_Task(void);
extern void Talk_Call_TimeOut_Task(void);

int Init_Timer(void) {
	pthread_attr_t attr;
	timer_thread_flag = 1;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&timer_thread, &attr, (void *) timer_thread_task, NULL);
	pthread_attr_destroy(&attr);
	if (timer_thread == 0) {
		LOGD("don't create timer thread \n");
		return (-1);
	}
	return 0;
}

int Uninit_Timer(void) {
	//timer
	timer_thread_flag = 0;
	usleep(40 * 1000);
	return 0;
}

void timer_thread_task(void) {

	int timenum;

	LOGD("create timer thread :0.001\n");

	timenum = 0;
	while (timer_thread_flag) {

		if (Local.OnlineFlag == 1) {
			OnlineCheckFunc();
		}

		timenum++;

		if (timenum > 0xFFFFFF) {
			timenum = 0;
		}
		usleep((INTRTIME - 10) * 1000); //40ms
	}

}
//---------------------------------------------------------------------------
void OnlineCheckFunc(void)
{
	unsigned char send_b[1520];
	int sendlength;
	int RemotePort;
	if (Local.Timer1Num > INTRPERSEC * 20) {
		if (Local.CallConfirmFlag == 0) {
			if ((Local.Status == 1) || (Local.Status == 2)
					|| (Local.Status == 5) || (Local.Status == 6)
					|| (Local.Status == 7) || (Local.Status == 8)
					|| (Local.Status == 9) || (Local.Status == 10)) {
				Talk_Call_End_Task();
				//recv_Call_End(1);
			}
			Local.OnlineFlag = 0;
		} else {
			Local.CallConfirmFlag = 0;
		}
		Local.Timer1Num = 0;
	} else if ((Local.Timer1Num % INTRPERSEC) == 0) {
		if ((Local.Status == 2) || (Local.Status == 6) || (Local.Status == 8)
				|| (Local.Status == 10) || (Local.Status == 3)) {
			memcpy(send_b, UdpPackageHead, 6);
			send_b[6] = VIDEOTALK;

			send_b[7] = ASK;
			send_b[8] = CALLCONFIRM;
			memcpy(send_b + 9, LocalCfg.Addr, 20);
			memcpy(send_b + 29, LocalCfg.IP, 4);
			memcpy(send_b + 33, Remote.Addr[0], 20);
			memcpy(send_b + 53, Remote.IP[0], 4);
			send_b[57] = (Local.OnlineNum & 0xFF000000) >> 24;
			send_b[58] = (Local.OnlineNum & 0x00FF0000) >> 16;
			send_b[59] = (Local.OnlineNum & 0x0000FF00) >> 8;
			send_b[60] = Local.OnlineNum & 0x000000FF;
			Local.OnlineNum++;
			sendlength = 62;
			sprintf(RemoteHost, "%d.%d.%d.%d", Remote.DenIP[0], Remote.DenIP[1],
					Remote.DenIP[2], Remote.DenIP[3]);
			RemotePort = RemoteVideoPort;
			/*UdpSendBuff(m_VideoSocket, RemoteHost, RemotePort, send_b,
					sendlength);*/
		}
		Local.Timer1Num++;
		TalkCtrlFunc();
	}
}
void TalkCtrlFunc(void)
{
	char strtime[30];
	int PrevStep = 0;
	int CurrStep = 0;
	//1S
	if ((Local.TimeOut % INTRPERSEC) == 0) {
		switch (Local.Status) {
            case 1:
            case 2:
                if (Local.TimeOut > CALLTIMEOUT) {
                    Talk_Call_TimeOut_Task();
                    //MsgLAN2CCCallTimeOut();
                }
                break;
            case 5:
            case 6:
                sprintf(strtime, "%02d:%02d", Local.TimeOut / INTRPERSEC / 60,
                        (Local.TimeOut / INTRPERSEC) % 60);
                if (Local.TimeOut > TALKTIMEOUT) {
                    Talk_Call_End_Task();
                    //recv_Call_End(1);
                    Local.OnlineFlag = 0;
                    if (DebugMode == 1) {
                        LOGD("talk timeout \n");
                    }
                }
                break;
            default:
                break;
        }
    }
    Local.TimeOut++;
}
