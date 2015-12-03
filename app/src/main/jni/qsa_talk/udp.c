//UDP
#include <stdio.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>       //sem_t
#include <dirent.h>
#define CommonH
#include "include/common.h"

#include <jni.h>

#include "../include/log_jni.h"

//UDP
int SndBufLen = 1024 * 128;
int RcvBufLen = 1024 * 128;

extern int DebugMode;

extern int ArpSendBuff(void);

short UdpRecvFlag;
pthread_t udpdatarcvid;
pthread_t udpvideorcvid;
int InitUdpSocket(short lPort);
void CloseUdpSocket(void);
int UdpSendBuff(int m_Socket, char *RemoteHost, int RemotePort,
		unsigned char *buf, int nlength);
void CreateUdpVideoRcvThread(void);
void UdpVideoRcvThread(void);
void AddMultiGroup(int m_Socket, char *McastAddr);
void DropMultiGroup(int m_Socket, char *McastAddr);
void RefreshNetSetup(int cType);

extern sem_t audiorec2playsem;
extern sem_t videorec2playsem;

int AudioMuteFlag;
char watchRecvBuf[1024];

void Recv_Talk_Call_Task(unsigned char *recv_buf, char *cFromIP);
void Recv_Talk_Line_Use_Task(unsigned char *recv_buf, char *cFromIP);
void Recv_Talk_Call_Answer_Task(unsigned char *recv_buf, char *cFromIP);
void Recv_Talk_Call_Confirm_Task(unsigned char *recv_buf, char *cFromIP);
void Recv_Talk_Open_Lock_Task(unsigned char *recv_buf, char *cFromIP);
void Recv_Talk_Call_Start_Task(unsigned char *recv_buf, char *cFromIP);
void Recv_Talk_Call_End_Task(unsigned char *recv_buf, char *cFromIP);
void Recv_Talk_Call_UpDown_Task(unsigned char *recv_buf, char *cFromIP,
		int length);
void TalkEnd_ClearStatus(void);
void RecvForceIFrame_Func(unsigned char *recv_buf, char *cFromIP);
void ForceIFrame_Func(void);

int Init_Udp_Send_Task(void);
int Uninit_Udp_Send_Task(void);
//---------------------------------------------------------------------------
int Init_Udp_Send_Task(void) {
	int i;
	pthread_attr_t attr;

	for (i = 0; i < UDPSENDMAX; i++) {
		Multi_Udp_Buff[i].isValid = 0;
		Multi_Udp_Buff[i].SendNum = 0;
		Multi_Udp_Buff[i].DelayTime = 100;
		Multi_Udp_Buff[i].SendDelayTime = 0;
	}

	sem_init(&multi_send_sem, 0, 0);
	multi_send_flag = 1;
	pthread_mutex_init(&Local.udp_lock, NULL);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&multi_send_thread, &attr, (void *) multi_send_thread_func,
			NULL);
	pthread_attr_destroy(&attr);
	if (multi_send_thread == 0) {
		LOGD("don't create UDP send thread. \n");
		return FALSE;
	}
}

void multi_send_thread_func(void) {
	int i, j, k;
	int isAdded;
	int HaveDataSend;
	char wavFile[80];
	int CurrNo;
	short CurrPack;
	struct timeval tv1;
	int SendCapturePicFailFlag = 0;
	int FindError;

    LOGD("create multi send thread \n");
	while (multi_send_flag == 1) {
		sem_wait(&multi_send_sem);
		HaveDataSend = 1;
		while (HaveDataSend) {
			//  pthread_mutex_lock (&Local.udp_lock);
			for (i = 0; i < UDPSENDMAX; i++) {
				if (Multi_Udp_Buff[i].isValid == 1) {
					if (Multi_Udp_Buff[i].SendNum < MAXSENDNUM) {
						if (Multi_Udp_Buff[i].m_Socket == ARP_Socket) {
                            ArpSendBuff();
                        } else {
							if (Multi_Udp_Buff[i].isValid == 1) {
								if (Multi_Udp_Buff[i].SendDelayTime == 0) {
									UdpSendBuff(Multi_Udp_Buff[i].m_Socket,
											Multi_Udp_Buff[i].RemoteHost,
											Multi_Udp_Buff[i].RemotePort,
											Multi_Udp_Buff[i].buf,
											Multi_Udp_Buff[i].nlength);
								}
							}
						}
						Multi_Udp_Buff[i].SendDelayTime += 100;
						if (Multi_Udp_Buff[i].SendDelayTime
								>= Multi_Udp_Buff[i].DelayTime) {
							Multi_Udp_Buff[i].SendDelayTime = 0;
							Multi_Udp_Buff[i].SendNum++;
						}
					}
					if (Multi_Udp_Buff[i].SendNum >= MAXSENDNUM) {
						if (Multi_Udp_Buff[i].m_Socket == ARP_Socket) {
							Multi_Udp_Buff[i].isValid = 0;
							if (DebugMode == 1)
								LOGD("free ARP send finish\n");
						} else {
							switch (Multi_Udp_Buff[i].buf[6]) {
							case VIDEOTALK:
								switch (Multi_Udp_Buff[i].buf[8]) {
								case CALL:
									if (Multi_Udp_Buff[i].buf[6] == VIDEOTALK) {
										for (k = 0; k < UDPSENDMAX; k++) {
											if (Multi_Udp_Buff[k].isValid
													== 1) {
												if (Multi_Udp_Buff[k].buf[8]
														== CALL) {
													if (k != i)
														Multi_Udp_Buff[k].isValid =
																0;
												}
											}
										}
										Multi_Udp_Buff[i].isValid = 0;
										LOGD("call fail, %d\n",
												Multi_Udp_Buff[i].buf[6]);
									}
									break;
								case CALLEND:
									Multi_Udp_Buff[i].isValid = 0;
									Local.OnlineFlag = 0;
									Local.CallConfirmFlag = 0;
									LOGD("multi_send_thread_func :: TalkEnd_ClearStatus()\n");
									TalkEnd_ClearStatus();
									break;
								default:
									Multi_Udp_Buff[i].isValid = 0;
#ifdef _DEBUG
									LOGD("communicate fail 1, %d\n",
											Multi_Udp_Buff[i].buf[6]);
#endif
									break;
								}
								break;
							default:
								Multi_Udp_Buff[i].isValid = 0;
								Local.Status = 0;
								//recv_Call_End(1);
#ifdef _DEBUG
								LOGD("communicate fail 3, %d\n",
										Multi_Udp_Buff[i].buf[6]);
#endif
								break;
							}
						}
						// pthread_mutex_unlock (&Local.udp_lock);
					}
					if ((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)
							&& (Multi_Udp_Buff[i].buf[8] == CALL)) {
					}
				}
			}

			HaveDataSend = 0;
			for (i = 0; i < UDPSENDMAX; i++)
				if (Multi_Udp_Buff[i].isValid == 1) {
					HaveDataSend = 1;
					break;
				}
			//   pthread_mutex_unlock (&Local.udp_lock);
			usleep(100 * 1000);
		}
	}
}

int Uninit_Udp_Send_Task(void) {
	multi_send_flag = 0;
	usleep(40 * 1000);
	sem_destroy(&multi_send_sem);
	pthread_mutex_destroy(&Local.udp_lock);
}

int InitUdpSocket(short lPort) {
	struct sockaddr_in s_addr;
	int nZero = 0;
	int iLen;
	int m_Socket;
	int nYes;
	int ret;
	int rc;
	int ttl;

	if ((m_Socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		LOGD("Create socket error\r\n");
		return 0;
	} else {
		LOGD("create socket.\n\r");
	}

	memset(&s_addr, 0, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(lPort);
	s_addr.sin_addr.s_addr = INADDR_ANY; //inet_addr(LocalIP);//INADDR_ANY;

	iLen = sizeof(nZero); //  SO_SNDBUF
	nZero = SndBufLen; //128K
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*) &nZero,
			sizeof((char*) &nZero));
	nZero = RcvBufLen; //128K
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*) &nZero,
			sizeof((char*) &nZero));

	ttl = MULTITTL;
	rc = setsockopt(m_Socket, IPPROTO_IP, IP_MULTICAST_TTL, (char *) &ttl,
			sizeof(ttl));

	nYes = 1;
	if (setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (char *) &nYes,
			sizeof((char *) &nYes)) == -1) {
		LOGD("set broadcast error.\n\r");
		return 0;
	}

	if ((bind(m_Socket, (struct sockaddr *) &s_addr, sizeof(s_addr))) == -1) {
		LOGD("bind error");
		exit(errno);
		return 0;
	} else
		LOGD("bind address to socket.\n\r");

	if (lPort == LocalVideoPort) {
		m_VideoSocket = m_Socket;
		CreateUdpVideoRcvThread();
	}
	return 1;
}
//---------------------------------------------------------------------------
void CloseUdpSocket(void) {
	UdpRecvFlag = 0;
	close(m_DataSocket);
	close(m_VideoSocket);
}
//---------------------------------------------------------------------------
#define SMALLESTSIZE  512
int UdpSendBuff(int m_Socket, char * RemoteHost, int RemotePort,
		unsigned char * buf, int nlength) {
	struct sockaddr_in To;
	int nSize;
	To.sin_family = AF_INET;
	To.sin_port = htons(RemotePort);
	To.sin_addr.s_addr = inet_addr(RemoteHost);

#ifdef ISSENDPACKETSIZE
	if (nlength < SMALLESTSIZE)
	nlength = SMALLESTSIZE;
#endif

	nSize = sendto(m_Socket, buf, nlength, 0, (struct sockaddr*) &To,
			sizeof(struct sockaddr));
	if (buf[8] != 0x0B) {
		LOGD("&&& SEND VIDEO &&& nSize = %d, nlength = %d", nSize, nlength);
		LOGD("RemoteHost = %s, RemotePort = %d, buf[8] = %02X", RemoteHost, RemotePort, buf[8]);
	}

	return nSize;
}

//---------------------------------------------------------------------------
void CreateUdpVideoRcvThread() {
	int i, ret;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&udpvideorcvid, &attr, (void *) UdpVideoRcvThread,
			NULL);
	pthread_attr_destroy(&attr);
#ifdef _DEBUG
	LOGD("Create UDP video pthread!\n");
#endif
	if (ret != 0) {
		LOGD("Create video pthread error!\n");
		exit(1);
	}
}
//---------------------------------------------------------------------------
void AddMultiGroup(int m_Socket, char *McastAddr)
{
	struct ip_mreq mcast;
	mcast.imr_multiaddr.s_addr = inet_addr(McastAddr);
	mcast.imr_interface.s_addr = INADDR_ANY;
	if (setsockopt(m_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mcast,
			sizeof(mcast)) == -1) {
		LOGD("set multicast error.\n\r");
		return;
	}
#ifdef _DEBUG
	LOGD("AddMultiGroup \n\r");
#endif
}
//---------------------------------------------------------------------------
void DropMultiGroup(int m_Socket, char *McastAddr)
{
	struct ip_mreq mcast;
	char IP_Group[20];
	if (Local.IP_Group[0] != 0) {
		sprintf(IP_Group, "%d.%d.%d.%d", Local.IP_Group[0], Local.IP_Group[1],
				Local.IP_Group[2], Local.IP_Group[3]);
		Local.IP_Group[0] = 0;
		Local.IP_Group[1] = 0;
		Local.IP_Group[2] = 0;
		Local.IP_Group[3] = 0;
		//  memset(&mcast, 0, sizeof(struct ip_mreq));
		mcast.imr_multiaddr.s_addr = inet_addr(IP_Group);
		mcast.imr_interface.s_addr = INADDR_ANY;
		if (setsockopt(m_Socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*) &mcast,
				sizeof(mcast)) < 0) {
			LOGD("drop multicast error.\n\r");
			return;
		}
	}
}

void UdpVideoRcvThread(void)
{
	unsigned char send_b[1520];
	int sendlength;
	char FromIP[20];
	int newframeno;
	int currpackage;
	int i, j;
	int sub;
	short PackIsExist;
	short FrameIsNew;
	struct sockaddr_in c_addr;
	socklen_t addr_len;
	int len;
	int tmp;
	unsigned char buff[8096];

	int isAddrOK;

	if (DebugMode == 1)
		LOGD("This is udp video pthread.");
	UdpRecvFlag = 1;

	addr_len = sizeof(c_addr);
	while (UdpRecvFlag == 1) {
		len = recvfrom(m_VideoSocket, buff, sizeof(buff) - 1, 0,
				(struct sockaddr *) &c_addr, &addr_len);
		if (len < 0) {
			perror("recvfrom");
			continue;
		}
		buff[len] = '\0';
#ifdef _DEBUG
		sprintf(Local.DebugInfo, "&&& RECV VIDEO &&& len=%d addr=%s:%d", len,
				inet_ntoa(c_addr.sin_addr), ntohs(c_addr.sin_port));
		if ((buff[8] != 0x0B) && (buff[8] != 0x08)) {
			LOGD(Local.DebugInfo);
		}
		sprintf(Local.DebugInfo,
				"&&& RECV VIDEO &&& [6]%02X, [7]%02X, [8]%02X, [32]%02X, [33]%02X, [34]%02X, [35]%02X",
				buff[6], buff[7], buff[8], buff[32], buff[33], buff[34],
				buff[35]);
		if ((buff[8] != 0x0B) && (buff[8] != 0x08)) {
			LOGD(Local.DebugInfo);
		}
#endif
		strcpy(FromIP, inet_ntoa(c_addr.sin_addr));
		//if(DebugMode == 1)
		//   printf("FromIP is %s\n",FromIP);
		if ((buff[0] == UdpPackageHead[0]) && (buff[1] == UdpPackageHead[1])
				&& (buff[2] == UdpPackageHead[2])
				&& (buff[3] == UdpPackageHead[3])
				&& (buff[4] == UdpPackageHead[4])
				&& (buff[5] == UdpPackageHead[5])) {
			switch (buff[6]) {
			case VIDEOTALK:
			case VIDEOTALKTRANS:
				switch (buff[8]) {
				case CALL:
					if (len >= 62) {
						Recv_Talk_Call_Task(buff, FromIP);
					} else {
						if (DebugMode == 1)
							LOGD("len of call is unusual\n");
					}
					break;
				case LINEUSE:
					if (len >= 57) {
						Recv_Talk_Line_Use_Task(buff, FromIP);
					} else {
						if (DebugMode == 1)
							LOGD("len of lineuse reply is unusual\n");
					}
					break;
				case CALLANSWER:
					if (len >= 61) {
						Recv_Talk_Call_Answer_Task(buff, FromIP);
					} else {
						if (DebugMode == 1)
							LOGD("len of call reply is unusual\n");
					}
					break;
				case CALLSTART:
					LOGD("FromIP is %s\n",FromIP);
					if (len >= 57) {
						Recv_Talk_Call_Start_Task(buff, FromIP);
					} else {
						if (DebugMode == 1)
							LOGD("len of talk-start is unusual\n");
					}
					break;
				case CALLCONFIRM:
					if (len >= 61) {
//						LOGD("UdpVideoRcvThread, 4444\n");
						Recv_Talk_Call_Confirm_Task(buff, FromIP);
					} else {
						if (DebugMode == 1)
							LOGD("len of call-confirm is unusual\n");
					}
					break;
				case REMOTEOPENLOCK:
					if (len >= 57) {
						Recv_Talk_Open_Lock_Task(buff, FromIP);
					} else {
						if (DebugMode == 1)
							LOGD("len of remote open lock is unusual\n");
					}
					break;
				case CALLEND:
					if (len >= 57) {
						Recv_Talk_Call_End_Task(buff, FromIP);
					} else {
						if (DebugMode == 1)
							LOGD("len of call end is unusual\n");
					}
					break;
				case FORCEIFRAME:
//					if(len == 57 || len == 58)
					if (len >= 57) {
						RecvForceIFrame_Func(buff, FromIP);
					} else {
						if (DebugMode == 1)
							LOGD("len of force Iframe is unusual\n");
					}
					break;
				case CALLUP:
				case CALLDOWN:
					Recv_Talk_Call_UpDown_Task(buff, FromIP, len);
					break;
				}
				break;
			}
		}

		if (strcmp(buff, "exit") == 0) {
			LOGD("recvfrom888888888\n");
			UdpRecvFlag = 0;
		}
	}
}
//-----------------------------------------------------------------------
void Recv_Talk_Call_Task(unsigned char *recv_buf, char *cFromIP) {
}
//-----------------------------------------------------------------------
void Recv_Talk_Line_Use_Task(unsigned char *recv_buf, char *cFromIP) {
	int i, j;
	int isAddrOK;
	char wavFile[80];

	pthread_mutex_lock(&Local.udp_lock);
	if (recv_buf[7] == ASK)
	{
		for (i = 0; i < UDPSENDMAX; i++) {
			if (Multi_Udp_Buff[i].isValid == 1) {
				if (Multi_Udp_Buff[i].m_Socket == m_VideoSocket) {
					if (Multi_Udp_Buff[i].SendNum < MAXSENDNUM) {
						if ((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)
								|| (Multi_Udp_Buff[i].buf[6] == VIDEOTALKTRANS)) {
							if (Multi_Udp_Buff[i].buf[7] == ASK) {
								if (Multi_Udp_Buff[i].buf[8] == CALL) {
									if (strcmp(Multi_Udp_Buff[i].RemoteHost,
											cFromIP) == 0) {
										Multi_Udp_Buff[i].isValid = 0;
										if (Remote.DenNum == 1) {
											Local.Status = 0;
											//recv_Line_Use(1);
										}

										if (DebugMode == 1) {
											sprintf(Local.DebugInfo,
													"receive reply of lineuse i = %d,%d.%d.%d.%d\n",
													i, recv_buf[53],
													recv_buf[54], recv_buf[55],
													recv_buf[56]);
											LOGD(Local.DebugInfo);
										}
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	pthread_mutex_unlock(&Local.udp_lock);
}

//-----------------------------------------------------------------------
void Recv_Talk_Call_Answer_Task(unsigned char *recv_buf, char *cFromIP) {
	int i, j;
	int isAddrOK;
	uint32_t Ip_Int;
	char wavFile[80];
	char RemoteIP[20];

	sprintf(RemoteIP, "%d.%d.%d.%d", recv_buf[53], recv_buf[54], recv_buf[55],
			recv_buf[56]);

	pthread_mutex_lock(&Local.udp_lock);
	if (recv_buf[7] == ASK)
	{
		for (i = 0; i < UDPSENDMAX; i++) {
			if (Multi_Udp_Buff[i].isValid == 1) {
				if (Multi_Udp_Buff[i].m_Socket == m_VideoSocket) {
					if (Multi_Udp_Buff[i].SendNum < MAXSENDNUM) {
						if (Multi_Udp_Buff[i].buf[6] == VIDEOTALK) {
							if (Multi_Udp_Buff[i].buf[7] == ASK) {
								if (Multi_Udp_Buff[i].buf[8] == CALL) {
									if (strcmp(Multi_Udp_Buff[i].RemoteHost,
											cFromIP) == 0) {
										if (strcmp(Multi_Udp_Buff[i].RemoteIP,
												RemoteIP) == 0) {
											Multi_Udp_Buff[i].isValid = 0;
											//  printf("Remote.DenNum = %d, Remote.isDirect = %d\n", Remote.DenNum, Remote.isDirect);
											if (LocalCfg.Addr[0] == 'S') {
												Ip_Int = inet_addr(cFromIP);
												memcpy(Remote.DenIP, &Ip_Int,
														4);
											}

											//MsgLAN2CCCallRespond();
											//StartRecVideo();//(CIF_W, CIF_H);
											//cameraInit();
											//StartRecVideo(CIF_W, CIF_H);
											Local.Status = 1;
											//recv_Call_Answer(1);
											Local.CallConfirmFlag = 1;
											Local.Timer1Num = 0;
											Local.TimeOut = 0;
											Local.OnlineNum = 0;
											Local.OnlineFlag = 1;

											sprintf(Local.DebugInfo,
													"Local.Status = %d\n",
													Local.Status);
											LOGD(Local.DebugInfo);
											sprintf(Local.DebugInfo,
													"receive reply of call, i = %d, %d.%d.%d.%d\n",
													i, recv_buf[53],
													recv_buf[54], recv_buf[55],
													recv_buf[56]);
											LOGD(Local.DebugInfo);
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	pthread_mutex_unlock(&Local.udp_lock);
}
//-----------------------------------------------------------------------
void Recv_Talk_Call_Start_Task(unsigned char *recv_buf, char *cFromIP) {
	unsigned char send_b[1520];
	int sendlength;
	int RemotePort;

	sprintf(Local.DebugInfo,
			"Local.Status: %d, recv_buf[7]: %d, Remote.DenNum = %d, Remote.isDirect = %d\n",
			Local.Status, recv_buf[7], Remote.DenNum, Remote.isDirect);
	LOGD(Local.DebugInfo);

	if (((Local.Status == 1) || (Local.Status == 7) || (Local.Status == 9))
			&& (recv_buf[7] == ASK)) {
		memcpy(send_b, recv_buf, 57);
		send_b[7] = REPLY;
		sendlength = 57;
		RemotePort = RemoteVideoPort;
		UdpSendBuff(m_VideoSocket, cFromIP, RemotePort, send_b, sendlength);

		memcpy(Remote.Addr[0], recv_buf + 33, 20);
		memcpy(Remote.IP[0], recv_buf + 53, 4);
		Remote.DenNum = 1;
		if (recv_buf[6] == VIDEOTALK) {
			memcpy(Remote.DenIP, Remote.IP[0], 4);
		}

		//MsgLAN2CCCallPickUp();

		//StopPlayWavFile();
		//WaitAudioUnuse(2000);
		//StartPlayAudio();

		Local.Status = 5;
		Local.TimeOut = 0;
		//recv_Call_Start(1);

		if (DebugMode == 1) {
			LOGD("other start talk \n");
		}
	}
}

void Recv_Talk_Call_Confirm_Task(unsigned char *recv_buf, char *cFromIP) {
	int i, j;
	int isAddrOK;
	unsigned char send_b[1520];
	int sendlength;
	int RemotePort;

	if (((Local.Status == 1) || (Local.Status == 5) || (Local.Status == 7)
			|| (Local.Status == 9)) && (recv_buf[7] == ASK)) {
		memcpy(send_b, recv_buf, 61);
		send_b[7] = REPLY;
		sendlength = 61;
		RemotePort = RemoteVideoPort;
		UdpSendBuff(m_VideoSocket, cFromIP, RemotePort, send_b, sendlength);
		Local.CallConfirmFlag = 1;
	} else {
		if (((Local.Status == 2) || (Local.Status == 6) || (Local.Status == 8)
				|| (Local.Status == 10)) && (recv_buf[7] == REPLY)) {
			Local.CallConfirmFlag = 1;
		}
	}
}
//-----------------------------------------------------------------------
void Recv_Talk_Open_Lock_Task(unsigned char *recv_buf, char *cFromIP) {
	int i, j;
	int isAddrOK;
	unsigned char send_b[1520];
	int sendlength;
	int RemotePort;

	if (((Local.Status == 1) || (Local.Status == 2) || (Local.Status == 5)
			|| (Local.Status == 6)) && (recv_buf[7] == ASK)) {
		memcpy(send_b, recv_buf, 57);
		send_b[7] = REPLY;
		sendlength = 57;
		RemotePort = RemoteVideoPort;
		UdpSendBuff(m_VideoSocket, cFromIP, RemotePort, send_b, sendlength);
		//recv_Open_Lock(1);

		if (DebugMode == 1) {
			LOGD("other remote open lock\n");
		}
	}
}

void Recv_Talk_Call_End_Task(unsigned char *recv_buf, char *cFromIP) {
	int i, j;
	int isAddrOK;
	unsigned char send_b[1520];
	int sendlength;
	int RemotePort;

	if (((Local.Status == 1) || (Local.Status == 2) || (Local.Status == 5)
			|| (Local.Status == 6) || (Local.Status == 7) || (Local.Status == 8)
			|| (Local.Status == 9) || (Local.Status == 10))
			&& (recv_buf[7] == ASK)) {
        Local.OnlineFlag = 0;
        Local.CallConfirmFlag = 0;

        memcpy(send_b, recv_buf, 57);
        send_b[7] = REPLY;
        sendlength = 57;
        RemotePort = RemoteVideoPort;
        if (recv_buf[6] == VIDEOTALK) {
            RemotePort = RemoteVideoPort;
        } else {
            RemotePort = RemoteVideoServerPort;
        }
        UdpSendBuff(m_VideoSocket, cFromIP, RemotePort, send_b, sendlength);
#ifdef _MULTI_MACHINE_SUPPORT
		ExitGroup(recv_buf);
#endif
		TalkEnd_ClearStatus();
#ifdef _DEBUG
		sprintf(Local.DebugInfo, "other end talk, %s\n", cFromIP);
		LOGD(Local.DebugInfo);
#endif
	} else {
		Local.OnlineFlag = 0;
		Local.CallConfirmFlag = 0;

		pthread_mutex_lock(&Local.udp_lock);
		if (recv_buf[7] == REPLY) {
			for (i = 0; i < UDPSENDMAX; i++) {
				if (Multi_Udp_Buff[i].isValid == 1) {
					if (Multi_Udp_Buff[i].m_Socket == m_VideoSocket) {
						if (Multi_Udp_Buff[i].SendNum < MAXSENDNUM) {
							if ((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)
									|| (Multi_Udp_Buff[i].buf[6]
											== VIDEOTALKTRANS)) {
								if (Multi_Udp_Buff[i].buf[7] == ASK) {
									if (Multi_Udp_Buff[i].buf[8] == CALLEND) {
										if (strcmp(Multi_Udp_Buff[i].RemoteHost,
												cFromIP) == 0) {
											Multi_Udp_Buff[i].isValid = 0;

											TalkEnd_ClearStatus();

											if (DebugMode == 1)
												LOGD("other reply talk end\n");
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		pthread_mutex_unlock(&Local.udp_lock);
	}
}
//-----------------------------------------------------------------------
void TalkEnd_ClearStatus(void) {
#ifdef _DEBUG
	sprintf(Local.DebugInfo, "Local.Status=%d\n", Local.Status);
	LOGD(Local.DebugInfo);
#endif
	DropMultiGroup(m_VideoSocket, NULL);
	switch (Remote.Addr[0][0]) {
	case 'Z':
	case 'W':
		Remote.Addr[0][5] = '\0';
		break;
	case 'M':
		Remote.Addr[0][8] = '\0';
		break;
	case 'S':
		Remote.Addr[0][12] = '\0';
		break;
	}

	switch (Local.Status) {
	case 1:
		break;
	case 5:
		//StopRecVideo();
#ifdef	_SND_RECORD_
		//StopRecAudio();
#endif
#ifdef	_SND_PLAY_
		//StopPlayAudio();
#endif
		break;
	}

	Local.Status = 0;
	Local.OnlineFlag = 0;
	Local.CallConfirmFlag = 0;
	//recv_Call_End(1);

}

void RecvForceIFrame_Func(unsigned char *recv_buf, char *cFromIP) {
	int i;
	unsigned char send_b[1520];
	int sendlength;
	int RemotePort;

	if (recv_buf[7] == ASK) {
		memcpy(send_b, recv_buf, 57);
		send_b[7] = REPLY;
		sendlength = 57;
		RemotePort = RemoteVideoPort;
		UdpSendBuff(m_VideoSocket, cFromIP, RemotePort, send_b, sendlength);
		Local.ForceIFrame = 1;
	} else {
		pthread_mutex_lock(&Local.udp_lock);
		if (recv_buf[7] == REPLY) {
			for (i = 0; i < UDPSENDMAX; i++) {
				if (Multi_Udp_Buff[i].isValid == 1) {
					if (Multi_Udp_Buff[i].m_Socket == m_VideoSocket) {
						if (Multi_Udp_Buff[i].SendNum < MAXSENDNUM) {
							if ((Multi_Udp_Buff[i].buf[6] == VIDEOTALK)
									|| (Multi_Udp_Buff[i].buf[6]
											== VIDEOTALKTRANS)) {
								if (Multi_Udp_Buff[i].buf[7] == ASK) {
									if (Multi_Udp_Buff[i].buf[8] == FORCEIFRAME) {
										if (strcmp(Multi_Udp_Buff[i].RemoteHost,
												cFromIP) == 0) {
											Multi_Udp_Buff[i].isValid = 0;
											break;
										}
									}
								}
							}
						}
					}
				}
			}
		}
		pthread_mutex_unlock(&Local.udp_lock);
	}
}
//-----------------------------------------------------------------------
void Recv_Talk_Call_UpDown_Task(unsigned char *recv_buf, char *cFromIP,
		int length) {
	int isAddrOK;
	unsigned char send_b[1520];
	int sendlength;
	short PackIsExist;
	TempAudioNode1 * tmp_audionode;
	int isFull;
	struct talkdata1 talkdata;
	if ((Local.Status == 1) || (Local.Status == 2) || (Local.Status == 5)
			|| (Local.Status == 6) || (Local.Status == 7) || (Local.Status == 8)
			|| (Local.Status == 9) || (Local.Status == 10)) //״̬Ϊ�Խ�
			{
		//LOGD(Local.DebugInfo, "&&&&&& recv_buf[61] = %d \n", recv_buf[61]);
		switch (recv_buf[61]) {
		case 1:
#if 0
			if ((Local.Status == 5) || (Local.Status == 6)
					|| (Local.Status == 10)) {
				memcpy(&talkdata, recv_buf + 9, sizeof(talkdata));
				//       printf("talkdata.Frameno = %d, talkdata.timestamp = %d\n", talkdata.Frameno, talkdata.timestamp);
				if (temp_audio_n >= G711NUM) {
					temp_audio_n = G711NUM;
					if (DebugMode == 1) {
						LOGD("temp_audio is full");
					}
				} else {
					tmp_audionode = (TempAudioNode1 *) find_audionode(
							TempAudioNode_h, talkdata.Frameno,
							talkdata.CurrPackage);
					if (tmp_audionode == NULL) {
						isFull = creat_audionode(TempAudioNode_h, talkdata,
								recv_buf, length);
						PackIsExist = 0;
					} else {
						if (DebugMode == 1) {
							LOGD("audio pack exist");
						}
						PackIsExist = 1;
					}

					if (PackIsExist == 0) {
						TimeStamp.OldCurrAudio = TimeStamp.CurrAudio;
						TimeStamp.CurrAudio = talkdata.timestamp;

						//   temp_audio_n ++;
						temp_audio_n = length_audionode(TempAudioNode_h);
						if (temp_audio_n >= 4) {
							//VPLAYNUM/2 4֡ 128ms
//							LOGD("&&&&&& audio data recv");
							sem_post(&audiorec2playsem);
						}
					}
				}
			}
#endif           

			break;
		case 2: //I֡  352*288
		case 3: //P֡  352*288
		case 4: //I֡  720*480
		case 5: //P֡  720*480
			LOGD("video data recv\n");
			break;

		}
	}
}
//-----------------------------------------------------------------------
void ForceIFrame_Func(void)
{
	int i;
	int RemotePort;

	for (i = 0; i < UDPSENDMAX; i++) {
		if (Multi_Udp_Buff[i].isValid == 0) {
			pthread_mutex_lock(&Local.udp_lock);
			Multi_Udp_Buff[i].SendNum = 5;
			Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
			RemotePort = RemoteVideoPort;
			Multi_Udp_Buff[i].RemotePort = RemotePort;
			Multi_Udp_Buff[i].CurrOrder = 0;
			sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d",
					Remote.DenIP[0], Remote.DenIP[1], Remote.DenIP[2],
					Remote.DenIP[3]);
			memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);
			if ((Local.Status == 1) || (Local.Status == 2)
					|| (Local.Status == 5) || (Local.Status == 6)
					|| (Local.Status == 7) || (Local.Status == 8)
					|| (Local.Status == 9) || (Local.Status == 10)) {
				Multi_Udp_Buff[i].buf[6] = VIDEOTALK;
			}
			Multi_Udp_Buff[i].buf[7] = ASK;
			Multi_Udp_Buff[i].buf[8] = FORCEIFRAME;

			memcpy(Multi_Udp_Buff[i].buf + 9, LocalCfg.Addr, 20);
			memcpy(Multi_Udp_Buff[i].buf + 29, LocalCfg.IP, 4);
			memcpy(Multi_Udp_Buff[i].buf + 33, Remote.Addr[0], 20);
			memcpy(Multi_Udp_Buff[i].buf + 53, Remote.IP[0], 4);

			Multi_Udp_Buff[i].nlength = 57;
			Multi_Udp_Buff[i].DelayTime = 100;
			Multi_Udp_Buff[i].SendDelayTime = 0;
			Multi_Udp_Buff[i].isValid = 1;

			pthread_mutex_unlock(&Local.udp_lock);

			sem_post(&multi_send_sem);
			break;
		}
	}
}

void ExitGroup(unsigned char *buf)
{
	int i, j;
	int isIP;
	int RemotePort;
	char tmp_ip[20];
	if (Remote.DenNum > 1) {
		pthread_mutex_lock(&Local.udp_lock);
		for (j = 0; j < Remote.DenNum; j++) {
			Remote.Added[j] = 0;
			if (DebugMode == 1) {
				sprintf(Local.DebugInfo, "%d.%d.%d.%d  %d.%d.%d.%d\n",
						Remote.IP[j][0], Remote.IP[j][1], Remote.IP[j][2],
						Remote.IP[j][3], buf[53], buf[54], buf[55], buf[56]);
				LOGD(Local.DebugInfo);
			}

			if ((Remote.IP[j][0] == buf[53]) && (Remote.IP[j][1] == buf[54])
					&& (Remote.IP[j][2] == buf[55])
					&& (Remote.IP[j][3] == buf[56]))
				isIP = 1;
			else
				isIP = 0;

			sprintf(tmp_ip, "%d.%d.%d.%d", Remote.IP[j][0], Remote.IP[j][1],
					Remote.IP[j][2], Remote.IP[j][3]);
			for (i = 0; i < UDPSENDMAX; i++) {
				if (Multi_Udp_Buff[i].isValid == 1) {
					if ((strcmp(Multi_Udp_Buff[i].RemoteHost, tmp_ip) == 0)
							&& (Multi_Udp_Buff[i].buf[8] == LEAVEGROUP)) {
						isIP = 1;
						sprintf(Local.DebugInfo,
								"exit command of multicast group in buffer, %s\n",
								tmp_ip);
						LOGD(Local.DebugInfo);
						break;
					}
				}
			}

			if (isIP == 0) {
				for (i = 0; i < UDPSENDMAX; i++) {
					if (Multi_Udp_Buff[i].isValid == 0) {
						Multi_Udp_Buff[i].SendNum = 0;
						Multi_Udp_Buff[i].m_Socket = m_VideoSocket;
						if (Remote.isDirect == 0) {
							//ֱͨ����
							RemotePort = RemoteVideoPort;
							Multi_Udp_Buff[i].buf[6] = VIDEOTALK;
							sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d",
									Remote.IP[j][0], Remote.IP[j][1],
									Remote.IP[j][2], Remote.IP[j][3]);
						} else {
							//��ת����
							RemotePort = RemoteVideoServerPort;
							Multi_Udp_Buff[i].buf[6] = VIDEOTALKTRANS;
							sprintf(Multi_Udp_Buff[i].RemoteHost, "%d.%d.%d.%d",
									Remote.DenIP[0], Remote.DenIP[1],
									Remote.DenIP[2], Remote.DenIP[3]);
						}
						Multi_Udp_Buff[i].RemotePort = RemotePort;
						if (DebugMode == 1) {
							sprintf(Local.DebugInfo,
									"requiring exit multicast group, Multi_Udp_Buff[i].RemoteHost is %s\n",
									Multi_Udp_Buff[i].RemoteHost);
							LOGD(Local.DebugInfo);
						}
						//ͷ��
						memcpy(Multi_Udp_Buff[i].buf, UdpPackageHead, 6);

						Multi_Udp_Buff[i].buf[7] = ASK; //����
						// ������
						Multi_Udp_Buff[i].buf[8] = LEAVEGROUP;

						memcpy(Multi_Udp_Buff[i].buf + 9, LocalCfg.Addr, 20);
						memcpy(Multi_Udp_Buff[i].buf + 29, LocalCfg.IP, 4);
						memcpy(Multi_Udp_Buff[i].buf + 33, Remote.Addr[j], 20);
						memcpy(Multi_Udp_Buff[i].buf + 53, Remote.IP[j], 4);

						Multi_Udp_Buff[i].buf[57] = Remote.GroupIP[0];
						Multi_Udp_Buff[i].buf[58] = Remote.GroupIP[1];
						Multi_Udp_Buff[i].buf[59] = Remote.GroupIP[2];
						Multi_Udp_Buff[i].buf[60] = Remote.GroupIP[3];

						Multi_Udp_Buff[i].nlength = 61;
						Multi_Udp_Buff[i].DelayTime = 100; //600;  20101112 ��Ϊ 100
						Multi_Udp_Buff[i].SendDelayTime = 0; //���͵ȴ�ʱ�����  20101112
						Multi_Udp_Buff[i].isValid = 1;

						sem_post(&multi_send_sem);
						break;
					}
				}
			}
		}
		//�򿪻�����
		pthread_mutex_unlock(&Local.udp_lock);
	}
}
//-----------------------------------------------------------------------
