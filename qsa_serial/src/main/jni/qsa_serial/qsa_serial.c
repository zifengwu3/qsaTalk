#include     <stdio.h>      
#include     <stdlib.h>     
#include     <unistd.h>     
#include     <fcntl.h>      
#include     <termios.h>    
#include     <errno.h>      

#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <sys/time.h>   

#include "include/common.h"

#define FALSE  -1
#define TRUE   0

int OpenDev(char *Dev);
int set_speed(int fd, int speed);  
int set_Parity(int fd,int databits,int stopbits,int parity);
int OpenComm(int CommPort,int BautSpeed,int databits,int stopbits,int parity);
int CommSendBuff(int fd, unsigned char buf[1024],int nlength);
void CloseComm();

#if 0
int Init_MultiCommThread(void); //初始化 Comm发送线程模块
int Uninit_MultiCommThread(void); //释放 Comm发送线程模块

short CommRecvFlag;
pthread_t comm2_rcvid;
void CreateComm2_RcvThread(int fd);
void Comm2_RcvThread(int fd);  
void print_haier_comm_data(char const *sbuf, int length);
#endif
/********************************************************************************/
int OpenDev(char *Dev)
{
	int	fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY
	if (FALSE == fd)
	{
		perror("Can't Open Serial Port");
		return FALSE;
	}
	else
    {
        return fd;
    }
}

/**
 *@brief
 *@param  fd     
 *@param  speed 
 *@return  void
 */
int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
	B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300, 38400,  
	19200,  9600, 4800, 2400, 1200,  300, };
int set_speed(int fd, int speed)
{
	int  i;
	int  status;
	struct termios Opt;
	tcgetattr(fd, &Opt);

	for (i=0; i<(sizeof(speed_arr)/sizeof(int)); i++)
	{
		if (speed == name_arr[i])
		{
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);

			if(status != 0)
			{
				perror("tcsetattr fd1");
				return FALSE;
			}
			tcflush(fd,TCIOFLUSH);
		}
	}
	return TRUE;
}

/**
 *@brief   
 *@param  fd     
 *@param  databits 
 *@param  stopbits
 *@param  parity
 */
int set_Parity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;

	if (tcgetattr(fd, &options) != 0)
	{
		perror("SetupSerial 1");
		return(FALSE);
	}

	options.c_cflag &= (~CSIZE);

	switch (databits) 
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unsupported data size\n"); return (FALSE);
	}

	switch (parity)
	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */ 
			break;  
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB); 
			options.c_iflag |= INPCK;             /* Disnable parity checking */ 
			break;  
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;     /* Enable parity */    
			options.c_cflag &= ~PARODD;        
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S':
		case 's':  /*as no parity*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;break;  
		default:
			fprintf(stderr,"Unsupported parity\n");    
			return (FALSE);  
	}  

	switch (stopbits)
	{
		case 1:    
			options.c_cflag &= ~CSTOPB;  
			break;  
		case 2:    
			options.c_cflag |= CSTOPB;  
			break;
		default:    
			fprintf(stderr,"Unsupported stop bits\n");  
			return (FALSE); 
	}

	/* Set input parity option */ 
	if (parity != 'n')
	{
		options.c_iflag |= INPCK;
	}

	options.c_cflag |= CLOCAL | CREAD;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	options.c_oflag &= ~OPOST;
	options.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME] = 10; 
	options.c_cc[VMIN] = 0; 

	if(tcsetattr(fd,TCSANOW,&options) != 0)
	{
		perror("SetupSerial 3");
		return (FALSE);
	}

	return (TRUE);
}

int OpenComm(int CommPort,int BautSpeed,int databits,int stopbits,int parity)
{
	char dev[20]  = "/dev/ttyS7";
	int Commfd;

	Commfd = OpenDev(dev);

	if (Commfd == FALSE)
    {
        return FALSE;
    }

	if (set_speed(Commfd,BautSpeed) == FALSE)
	{
		printf("Set Baut Error\n");
		return FALSE;
	}

	if (set_Parity(Commfd,databits,stopbits,parity) == FALSE)
	{
		printf("Set Parity Error\n");
		return FALSE;
	}

	switch (CommPort)
	{
		case 2:
			//CreateComm2_RcvThread(Commfd);
			break;
		default:
			break;
	}

	return Commfd;
}


#if 0
#define COMM_DEBUG 0
void CreateComm2_RcvThread(int fd)
{
	int ret;

	CommRecvFlag = 1;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ret = pthread_create(&comm2_rcvid, &attr, (void *)Comm2_RcvThread, (int *)fd);
	pthread_attr_destroy(&attr);

	printf ("Create comm2 pthread!\n");
	if(ret != 0) 
	  printf ("Create comm2 pthread error!\n");
}

void Comm2_RcvThread(int fd)  //Comm2
{
	struct timeval tv;
	uint32_t prev_comm_time;
	uint32_t nowtime;
	int len;
	int i;
	char buff[512];
	char validbuff[512];
	static int validlen;
	unsigned char m_crc;
	unsigned short m_crc16;
	struct commbuf1 commbuf;

#ifdef _DEBUG
	printf("This is comm2 pthread.\n");
#endif

	gettimeofday(&tv, NULL);
	prev_comm_time = tv.tv_sec *1000 + tv.tv_usec/1000;
	commbuf.stats = 0;
	validlen = 0;

	while (CommRecvFlag == 1)
	{
		if (Local.interface_init_flag == 1)
		{
			while ((len = read(fd, buff, 512)) > 0)
			{
				gettimeofday(&tv, NULL);
				nowtime = tv.tv_sec *1000 + tv.tv_usec/1000;
#if COMM_DEBUG
				printf("prev-now:(%d)\n",nowtime-prev_comm_time);
#endif
				if ((nowtime - prev_comm_time) >= 200)
				{
					commbuf.iget = 0;
					commbuf.iput = 0;
					commbuf.n = 0;
					commbuf.stats = 0;
					validlen = 0;
				}

				prev_comm_time = nowtime;
#if COMM_DEBUG
				printf("%d:comm2(%d,%d,%d) Len %d :\n", __LINE__, commbuf.iget, commbuf.iput, commbuf.n, len);
				for (i=0; i<len; i++)
                {
                    printf("%02X ", buff[i]);
                }
				printf("\n");
#endif

				prev_comm_time = nowtime;
				memcpy(commbuf.buffer + commbuf.iput, buff, len);

				commbuf.n += len;
#if COMM_DEBUG
				printf("%d:comm2(%d) n.len=%d,validlen=%d\n", __LINE__, commbuf.stats, commbuf.n, validlen);
#endif
				/*判断是否是正常协议*/	
				if ((commbuf.stats == 1)
							|| (commbuf.buffer[commbuf.iput] == CMD_SPI_QSA_HEAD)
#if defined(_COMM_MSM4IN1_SUPPORT)
							|| (commbuf.buffer[commbuf.iput] == CMD_AIR_MODEL_HEAD)
#if defined(_COMM_DLHB_SUPPORT)
							|| ((commbuf.buffer[commbuf.iput] == CMD_AIR_DLHB_MODEL_HEAD)
							&& (commbuf.buffer[commbuf.iput + 1] == CMD_AIR_DLHB_MODEL_CODE)
                            && (commbuf.buffer[commbuf.iput + 2] == CMD_AIR_DLHB_MODEL_LEN))
#endif
#endif
							|| (commbuf.buffer[commbuf.iput] == CMD_SPI_QSA_PORDUCTION)) {
					/*判断是否是协议头，是否带长度参数,是否是第一包*/
					if ((((commbuf.buffer[commbuf.iput] == CMD_SPI_QSA_HEAD)
#if defined(_COMM_MSM4IN1_SUPPORT)
                                    || (commbuf.buffer[commbuf.iput] == CMD_AIR_MODEL_HEAD)
#if defined(_COMM_DLHB_SUPPORT)
                                    || ((commbuf.buffer[commbuf.iput] == CMD_AIR_DLHB_MODEL_HEAD)
                                        && (commbuf.buffer[commbuf.iput + 1] == CMD_AIR_DLHB_MODEL_CODE)
                                        && (commbuf.buffer[commbuf.iput + 2] == CMD_AIR_DLHB_MODEL_LEN))
#endif
#endif
                                    || (commbuf.buffer[commbuf.iput] == CMD_SPI_QSA_PORDUCTION))
                                && (!commbuf.stats)) || ((!validlen) && commbuf.stats)) {
                        if (commbuf.n == 1) {
                            /*进入连续读取状态*/
                            commbuf.stats = 1;
                        } else if (commbuf.n >= 2) {
                            /*计算协议长度*/
                            if (commbuf.stats == 0) {
                                commbuf.stats = 1;
                            }
                            if (validlen == 0) {
#if defined(_COMM_MSM4IN1_SUPPORT)
                                if (commbuf.buffer[commbuf.iget+1] == CMD_AIR_MODEL_CODE) {
                                    validlen = 14;
#if defined(_COMM_DLHB_SUPPORT)
                                } else if ((commbuf.buffer[commbuf.iput] == CMD_AIR_DLHB_MODEL_HEAD)
                                        && (commbuf.buffer[commbuf.iput + 1] == CMD_AIR_DLHB_MODEL_CODE)
                                        && (commbuf.buffer[commbuf.iput + 2] == CMD_AIR_DLHB_MODEL_LEN)) {
                                    validlen = 15;
#endif
                                } else
#endif
                                {
                                    validlen = commbuf.buffer[commbuf.iget+1] + 3;
                                }
                            }
                        }
                    }

					commbuf.iput += len;
					if (commbuf.iput >= COMMMAX) {
                        commbuf.iput = 0;
                    }

					if (commbuf.n >= validlen && validlen) {
						commbuf.stats = 0;
#ifdef _DEBUG
						printf("%d:validlen[%d],comm2(%d,%d,%d) Len %d :\n", \
                                __LINE__, validlen, commbuf.iget, commbuf.iput, commbuf.n, len);
						for (i=0; i<commbuf.iput; i++) {
                            printf("%02X ",commbuf.buffer[i]);
                        }
						printf("\n");
#endif
						memcpy(validbuff, commbuf.buffer + commbuf.iget, validlen);
						/*从总数据中取出需要的数据*/
						commbuf.iget += validlen;
						if (commbuf.iget >= COMMMAX) {
                            commbuf.iget = 0;
                        }
						commbuf.n -= validlen;

						if ((validbuff[0] == CMD_SPI_QSA_HEAD) \
                                || (validbuff[0] == CMD_SPI_QSA_PORDUCTION)) {
							m_crc = 0;
							for (i=0; i<(validlen-1); i++) {
                                m_crc += validbuff[i];
                            }
							m_crc &= 0xFF;
							if (m_crc == validbuff[validlen-1]) {
#ifdef _DEBUG
								printf("crc success\n");
#endif
								RecvCommDataFunc((unsigned char *)validbuff, validlen-1);
                            } else {
                                printf("crc error\n");
                            }
#if defined(_COMM_MSM4IN1_SUPPORT)
#if defined(_COMM_DLHB_SUPPORT)
                        } else if ((validbuff[0] == CMD_AIR_DLHB_MODEL_HEAD)
                                        && (validbuff[1] == CMD_AIR_DLHB_MODEL_CODE)) {
                            m_crc16 = comm_crc16(validbuff, 13);
                            if ((validbuff[validlen - 2] == (m_crc16 & 0xFF))
                                && (validbuff[validlen - 1] == ((m_crc16>>8) & 0xFF))) {
#ifdef _DEBUG
								printf("DLHB crc16 success\n");
#endif
                                put_air_dlhb_data(validbuff, &curr_air_data);
                                print_air_dlhb_info(&curr_air_data);
                                compare_air_dlhb_info(&curr_air_data, &prev_air_data);
                                memcpy(&prev_air_data, &curr_air_data, sizeof(AIR_MODEL_INFO));
                            } else {
                                printf("DLHB crc16 error\n");
                            }
#endif
						} else {
                            m_crc = 0;
							for (i=0; i<(validlen-1); i++) {
                                m_crc += validbuff[i];
                            }
							m_crc &= 0xFF;
							if (m_crc == validbuff[validlen-1]) {
#ifdef _DEBUG
								printf("FA crc success\n");
#endif
                                put_air_data(validbuff, &curr_air_data);
                                print_air_info(&curr_air_data);
                                compare_air_info(&curr_air_data, &prev_air_data);
                                memcpy(&prev_air_data, &curr_air_data, sizeof(AIR_MODEL_INFO));
                            } else {
                                printf("FA crc error\n");
                            }
#endif
                        }
					}
#ifdef _CONTROL_SUPPORT
                } else if ((commbuf.stats == 2) \
                        || (commbuf.buffer[commbuf.iput] == CMD_SPI_HAIER_STX)) {
					/*判断是否是协议头，是否带长度参数,是否是第一包*/
					if ((commbuf.buffer[commbuf.iput] == CMD_SPI_HAIER_STX) \
								&& (!commbuf.stats)) {
						/*进入连续读取状态*/
						commbuf.stats = 2;
						/*计算协议长度*/
						validlen = 32;
					}

					commbuf.iput += len;
					if (commbuf.iput >= COMMMAX) {
                        commbuf.iput = 0;
                    }

					if ((commbuf.n >= validlen) && (validlen)) {
						commbuf.stats = 0;
#ifdef _DEBUG
						printf("commbuf.n=%d\n",commbuf.n);
                        print_haier_comm_data((char *)(commbuf.buffer), validlen);
#endif
						memcpy(validbuff, commbuf.buffer + commbuf.iget, validlen);

						/*从总数据中取出需要的数据*/
						commbuf.iget += validlen;
						if (commbuf.iget >= COMMMAX) {
                            commbuf.iget = 0;
                        }
						commbuf.n -= validlen;

						/*协议头和协议尾是正确的*/
						if ((validbuff[0] == CMD_SPI_HAIER_STX) \
                                && (validbuff[31] == CMD_SPI_HAIER_ETX)) {
							//Xor校验和
							m_crc = 0;
							for (i=11; i<14+validbuff[13]; i++) {
                                m_crc = m_crc^validbuff[i];
                            }

							if (m_crc == validbuff[validbuff[13] + 14]) {
								//时回复状态的命令和长度
								switch (validbuff[8])
								{
									case CMD_UHOME_DEV_STAT://设备状态
									case CMD_UHOME_EXE_CHK://状态查询返回
									case CMD_UHOME_EXE_OPER:
                                        {
#if COMM_DEBUG
                                            printf("Order_Value: %02X:::%02X\n",validbuff[8],validbuff[11]);
#endif
                                            switch (validbuff[11]) 
                                            {
                                                /* dev type */
                                                case COMM_LIGHT:
                                                    {
                                                        RecvCommLightFunc(validbuff, 32);
                                                    }
                                                    break;
                                                case COMM_CURTAIN:
                                                    {
                                                        RecvCommCurtainFunc(validbuff, 32);
                                                    }
                                                    break;
                                                case COMM_PADSWITCH:
                                                    {
                                                        RecvCommPadLightFunc(validbuff,32);
                                                    }
                                                    break;
                                                case COMM_PADCURTAIN:
                                                    {
                                                        RecvCommPadCurtainFunc(validbuff,32);
                                                    }
                                                    break;
                                                case COMM_PADSOCKET:
                                                    {
                                                        RecvCommPadSocketFunc(validbuff,32);
                                                    }
                                                    break;
#ifndef _PART_TWO_SUPPORT
                                                case COMM_PADFEN:
                                                    {
                                                        RecvCommPadWindFunc(validbuff, 32);
                                                    }
                                                    break;
#endif
#if defined(HBS_SMART_LOCK_SUPPORT)
                                                case COMM_LOCK:
                                                    {
                                                        recv_smart_lock_stats(validbuff, 32);
                                                    }
                                                    break;
#endif
                                                default:
                                                    {
                                                        com_show_status(validbuff,32);
                                                    }
                                                    break;
                                            }
                                        }
										break;
									case CMD_UHOME_DEV_REG://设备注册
                                        {
#if COMM_DEBUG
                                            P_Debug("Catch Device Status");
#endif
                                            if (validbuff[13] == 0x03 || validbuff[13] == 0x06) 
                                            {
                                                uHome_Recv_Register_Info_Func((unsigned char *)validbuff);
                                            }
                                        }
										break;
									case CMD_UHOME_OK_ACK:
                                        {
                                            switch (validbuff[11])	//dev type
                                            {
                                                case COMM_LIGHT:
                                                    {
#if COMM_DEBUG
                                                        P_Debug("Catch 30-light ACK");
#endif
                                                        RecvCommLightFunc(validbuff, 32);
                                                    }
                                                    break;
                                                case COMM_CURTAIN:
                                                    {
#if COMM_DEBUG
                                                        P_Debug("Catch 485-Curtain ACK");
#endif
                                                        RecvCommCurtainFunc(validbuff, 32);
                                                    }
                                                    break;
                                                default:
                                                    break;
                                            }
                                        }
										break;
                                    case CMD_UHOME_DEV_ALARM:
                                    case CMD_UHOME_SYNC_DATA:
                                        {
#if defined(HBS_SMART_LOCK_SUPPORT)
                                            if (validbuff[11] == COMM_LOCK)
                                            {
                                                recv_smart_lock_stats(validbuff, 32);
                                            }
#endif
                                        }
                                        break;
									default:
										break;
								}

								if ((validbuff[11] == COMM_CURTAIN) && (validbuff[14] == CMD_UHOME_DEV_STAT)) {
#if COMM_DEBUG
									P_Debug("Catch 485-Curtain ACK");
#endif
									RecvCommCurtainFunc(validbuff, 32);
								}
#if COMM_DEBUG
                            } else {
                                P_Debug("XOR ERROR");
#endif
                            }
						}
					}
#endif
				}
			}
		}
	}
}

void CloseComm()
{
	CommRecvFlag = 0;
	usleep(40*1000);
	pthread_cancel(comm2_rcvid);
	close(Comm2fd);
}

int CommSendBuff(int fd,unsigned char buf[1024],int nlength)
{
	int nByte; 
	int j;

	nByte = write(fd, buf ,nlength);
#ifdef _DEBUG
    printf("\n");
	printf ("[%d]Send:",nByte);
	for (j=0; j<nlength; j++)
    {
        printf(" %02X",buf[j]);
    }
	printf("\n");
#endif
	return nByte;
}

int Init_MultiCommThread(void) //初始化 Comm发送线程模块
{
	int i;
	pthread_attr_t attr;
	//将COMM发送缓冲置为无效
	for (i=0; i<COMMSENDMAX; i++)
	{
		Multi_Comm_Buff[i].isValid = 0;
		Multi_Comm_Buff[i].SendNum = 0;
		Multi_Comm_Buff[i].m_Comm = Comm2fd;
	}
	//主动命令数据发送线程：终端主动发送命令，如延时一段没收到回应，则多次发送
	//用于UDP和Comm通信
	sem_init(&multi_comm_send_sem,0,0);
	multi_comm_send_flag = 1;
	//创建互斥锁
	pthread_mutex_init (&Local.comm_lock, NULL);
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&multi_comm_send_thread,&attr,(void *)multi_comm_send_thread_func,NULL);
	pthread_attr_destroy(&attr);
	if ( multi_comm_send_thread == 0 ) 
    {
		printf("Fail Create Comm Send Thread\n");
		return -1;
	}
    return 0;
}

int Uninit_MultiCommThread(void) //释放 Comm发送线程模块
{
	//COMM主动命令数据发送线程
	multi_comm_send_flag = 0;
	usleep(40*1000);
	pthread_cancel(multi_comm_send_thread);
	sem_destroy(&multi_comm_send_sem);
	pthread_mutex_destroy(&Local.comm_lock);  //删除互斥锁
    return 0;
}

void multi_comm_send_thread_func(void)
{
	int i;
	int HaveDataSend;
#ifdef _DEBUG
	printf("Success Create Comm Send Thread\n");
#endif
	while (multi_comm_send_flag == 1)
	{
		sem_wait(&multi_comm_send_sem);
		if (Local.Multi_Comm_Send_Run_Flag == 0)
        {
            Local.Multi_Comm_Send_Run_Flag = 1;
        }
		else
		{
			HaveDataSend = 1;
			while (HaveDataSend) 
            {
				for (i=0; i<COMMSENDMAX; i++) 
                {
					if (Multi_Comm_Buff[i].isValid == 1) 
                    {
						if (Multi_Comm_Buff[i].SendNum  < MAXSENDNUM) 
                        {
							if (Multi_Comm_Buff[i].isValid == 1) 
                            {
								//COMM发送
								CommSendBuff(Multi_Comm_Buff[i].m_Comm,
										     Multi_Comm_Buff[i].buf, 
											 Multi_Comm_Buff[i].nlength);

								if (PopLoadNum == 119 || PopLoadNum == 131) 
                                {
									if (Local.IsLoadingFlag) 
                                    {
										Local.LoadTimeOut = 0;
									}
								}

								if (Multi_Comm_Buff[i].SendNum > 0) 
                                {
									usleep((Multi_Comm_Buff[i].DelayTime+100)*1000);
								}
							}
							Multi_Comm_Buff[i].SendNum++;
							break;
						}
					}
				}

				if ((Multi_Comm_Buff[i].isValid == 1) && (Multi_Comm_Buff[i].SendNum >= MAXSENDNUM)) 
                {
					//锁定互斥锁
					pthread_mutex_lock (&Local.comm_lock);
					switch (Multi_Comm_Buff[i].buf[0])
					{
						case CMD_SPI_QSA_PORDUCTION:
                            {
                                Multi_Comm_Buff[i].isValid = 0;
#ifdef _DEBUG
                                printf("%d: COMM Send Fail, 0x%02X:: 0x%02X\n", \
                                        __LINE__, Multi_Comm_Buff[i].buf[0],Multi_Comm_Buff[i].buf[2]);
#endif
                            }
							break;
						case CMD_SPI_QSA_HEAD:
                            {
                                Multi_Comm_Buff[i].isValid = 0;
#ifdef _DEBUG
                                printf("%d: COMM Send Fail, 0x%02X:: 0x%02X\n", \
                                        __LINE__, Multi_Comm_Buff[i].buf[0],Multi_Comm_Buff[i].buf[2]);
#endif
#ifdef _BUS_ALARM_SUPPORT
                                switch(Multi_Comm_Buff[i].buf[2])
                                {
                                    case CMD_SPI_BUS_CHK_DEV:
#ifdef _DEBUG
                                        printf("Alarm Model isn't Recvice\n");
#endif
                                        SimulationBusAlarmFunc(Multi_Comm_Buff[i].buf);
                                        break;
                                    case CMD_SPI_SCH_VER:
#ifdef _DEBUG
                                        printf("Check Device Version isn't Recvice\n");
#endif
                                        break;
                                    default:
                                        break;
                                }
#endif
                            }
							break;
#ifdef _CONTROL_SUPPORT
						case CMD_SPI_HAIER_STX:
                            {
                                Multi_Comm_Buff[i].isValid = 0;
#ifdef _DEBUG
                                printf("%d: COMM Send Fail, 0x%02X:: 0x%02X\n", __LINE__,Multi_Comm_Buff[i].buf[0],Multi_Comm_Buff[i].buf[8]);
#endif
                            }
							break;
#endif
						default://为其它命令，本次通信结束
                            {
                                Multi_Comm_Buff[i].isValid = 0;
#ifdef _DEBUG
                                printf("%d: COMM Send Fail:0x%02X\n", __LINE__,Multi_Comm_Buff[i].buf[0]);
#endif
                            }
							break;
					}
					//打开互斥锁
					pthread_mutex_unlock (&Local.comm_lock);
				}

				//判断数据是否全部发送完，若是，线程终止
				HaveDataSend = 0;
				for (i=0; i<COMMSENDMAX; i++)
				{
					if (Multi_Comm_Buff[i].isValid == 1)
					{
						HaveDataSend = 1;
						break;
					}
				}
			}
		}
	}
}

void print_haier_comm_data(char const *sbuf, int length)
{
    int i;
    int start = 0;
    int end = 0;
    int addlength = 0;

    if ((sbuf[0] == CMD_SPI_HAIER_STX) \
            && (sbuf[length-1] == CMD_SPI_HAIER_ETX))
    {
        if (length >= HAIER_ORDER_LENGTH)
        {
            addlength = 1;
            end += addlength;
            printf("Stx   (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            start = end;

            addlength = 6;
            end += addlength;
            printf("Addr  (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            start = end;

            addlength = 2;
            end += addlength;
            printf("Cmd   (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            start = end;

            addlength = 2;
            end += addlength;
            printf("Prea  (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            start = end;

            addlength = 1;
            end += addlength;
            printf("Type  (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            start = end;

            addlength = 1;
            end += addlength;
            printf("Loca  (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            start = end;

            addlength = 1;
            end += addlength;
            printf("Len   (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            start = end;

            addlength = 1;
            end += addlength;
            printf("S_cmd (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            start = end;

            addlength = 16;
            end += addlength;
            printf("Data  (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                if (((i-start)%20 == 0) && ((i-start) != 0)) 
                {
                    printf("\n");
                    printf("             ");
                }
                printf("%02X ",sbuf[i]);
            }
            printf("\n");

            start = length - 1;
            addlength = 1;
            end += addlength;
            printf("Etx   (%02d)  :", addlength);
            for (i=start; i<end; i++) 
            {
                printf("%02X ",sbuf[i]);
            }
            printf("\n");
            printf("\n");
        }
    }
}
#endif
