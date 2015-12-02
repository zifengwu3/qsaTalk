#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include <assert.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
//#include <linux/videodev.h>

#include "include/common.h"

#include "../include/log_jni.h"

int DebugMode = 0;           //����ģʽ
pthread_mutex_t audio_open_lock;
pthread_mutex_t audio_close_lock;
pthread_mutex_t audio_lock;

extern int InitArpSocket(void);
extern void AddMultiGroup(int m_Socket, char *McastAddr);
extern void InitRecVideo(void);
//---------------------------------------------------------------------------
void P_Debug(char *DebugInfo); //������Ϣ
int qsa_init_main_task(int argc, char *argv[]);
void qsa_init_audio_task(void);
void qsa_init_udp_task(void);
//---------------------------------------------------------------------------
void P_Debug(char *DebugInfo) //������Ϣ
{
	;
}
//---------------------------------------------------------------------------
int qsa_init_main_task(int argc, char *argv[]) {
	pthread_attr_t attr;
	int ch;
	int programrun; //�������б�־
	int i, j;
	uint32_t Ip_Int;

	DebugMode = 1;

	TimeStamp.OldCurrVideo = 0; //��һ�ε�ǰ��Ƶʱ��
	TimeStamp.CurrVideo = 0;
	TimeStamp.OldCurrAudio = 0; //��һ�ε�ǰ��Ƶʱ��
	TimeStamp.CurrAudio = 0;

	RemoteVideoPort = 8302;
	strcpy(RemoteHost, "192.168.0.88");
	LocalVideoPort = 8302; //����ƵUDP�˿�

	//ReadCfgFile();
	GetCfg();
	DeltaLen = 9 + sizeof(struct talkdata1); //��ݰ���Ч���ƫ����

	strcpy(UdpPackageHead, "QIUSHI");
	for (i = 0; i < 20; i++)
		NullAddr[i] = '0';
	NullAddr[20] = '\0';

	Local.Status = 0; //״̬Ϊ����
	Local.RecPicSize = 1; //Ĭ����Ƶ��СΪ352*288
	Local.PlayPicSize = 1; //Ĭ����Ƶ��СΪ352*288

	Ip_Int = inet_addr("192.168.0.5");
	memcpy(Remote.IP, &Ip_Int, 4);
	memcpy(Remote.Addr[0], NullAddr, 20);
	memcpy(Remote.Addr[0], "S00010101010", 12);

	//��ʼ��ARP Socket
	InitArpSocket();

	qsa_init_audio_task();
	qsa_init_udp_task();

	Init_Timer(); //��ʼ����ʱ��

	return (0);
}

void qsa_init_audio_task(void) {

	//��ʼ������ͷ���
	if (TempAudioNode_h == NULL) {
		TempAudioNode_h = (TempAudioNode1 *) init_audionode();
	}

	InitAudioParam(); //��ʼ����Ƶ����
	InitRecVideo();
}

void qsa_uninit_task(void) {

	Uninit_Timer(); //�ͷŶ�ʱ��

	//UDP����������ݷ����߳�
	Uninit_Udp_Send_Task();

	//�ر�ARP
	CloseArpSocket();
	//�ر�UDP�������߳�
	CloseUdpSocket();

}

void qsa_init_udp_task(void) {

	if (InitUdpSocket(LocalVideoPort) == 0) {
		LOGD("can't create video socket.\n\r");
	}

	//����������ݷ����̣߳��ն����������������ʱһ��û�յ���Ӧ�����η���
	//����UDP��Commͨ��
	Init_Udp_Send_Task();

	SendFreeArp();
}
//---------------------------------------------------------------------------

void GetCfg(void) {
	FILE *cfg_fd;
	int bytes_write;
	int bytes_read;
	uint32_t Ip_Int;
	int ReadOk;
	DIR *dirp;
	char SystemOrder[100];
	int i, j;

	//��ַ����
	memcpy(LocalCfg.Addr, NullAddr, 20);
	memcpy(LocalCfg.Addr, "M00010100000", 12);
	//�豸��ʱ����״̬ʱ��
	LocalCfg.ReportTime = 10;
}

//�������ļ�
void ReadCfgFile(void) {
	FILE *cfg_fd;
	int bytes_write;
	int bytes_read;
	uint32_t Ip_Int;
	int ReadOk;
	DIR *dirp;
	char SystemOrder[100];
	int i, j;
	//���������ļ�Ŀ¼�Ƿ����
	if ((dirp = opendir("/data/app")) == NULL) {
		if (mkdir("/data/app", 1) < 0)
			P_Debug("error to mkdir /mnt/mtd/config\n");
	} else {
		closedir(dirp);
	}

	//unlink(cfg_name);
	ReadOk = 0;
	while (ReadOk == 0) {
		//�������ļ�
		//  if(access(cfg_name, R_OK|W_OK) == NULL)
		if ((cfg_fd = fopen(cfg_name, "rb")) == NULL) {
			P_Debug("config file is not exist! create new config file \n");
			// if((cfg_fd = open(cfg_name, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) == -1)
			if ((cfg_fd = fopen(cfg_name, "wb")) == NULL)
				P_Debug("don't create config file!\n");
			else {
				//��ַ����
				memcpy(LocalCfg.Addr, NullAddr, 20);
				memcpy(LocalCfg.Addr, "M00010100000", 12);

				bytes_write = fwrite(&LocalCfg, sizeof(LocalCfg), 1, cfg_fd);
				fclose(cfg_fd);
			}
		} else {
			ReadOk = 1;
			bytes_read = fread(&LocalCfg, sizeof(LocalCfg), 1, cfg_fd);
			if (bytes_read != 1) {
				P_Debug(
						"read config file fail, recreate default config file \n");
				ReadOk = 0;
				fclose(cfg_fd);
				unlink(cfg_name);
			}
		}
	}
	//  exit(0);
}
//---------------------------------------------------------------------------
//д���������ļ�
void WriteCfgFile(void) {
	FILE *cfg_fd;
	int j;
	uint8_t isValid;
	if ((cfg_fd = fopen(cfg_name, "wb")) == NULL)
		P_Debug("don't create config file \n");
	else {
		//��д���������ļ�
		fwrite(&LocalCfg, sizeof(LocalCfg), 1, cfg_fd);
		fclose(cfg_fd);
		P_Debug("save local set succeed \n");
	}
}
