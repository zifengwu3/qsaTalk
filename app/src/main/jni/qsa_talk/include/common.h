

#include <inttypes.h>
#include <signal.h>
#include <semaphore.h>       //sem_t
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "sndtools.h"
//#include "image.h"

//#define _REMOTECALLTEST  //Զ�̺��в���

//#define cfg_name "./door26cfg"
#define cfg_name "/system/door26cfg"
#define info_name "/mnt/mtd/config/info"
#define wavini_name "/mnt/sddisk/homewav/wavini"
#define movieini_name "/mnt/sddisk/homemovie/movieini"
#define mtdexe_name "/mnt/mtd/door26"
#define mtdimage_name "/mdoor26pImage"

#define FLAGTEXT "hikdsdkkkkdfdsIMAGE"


#define _SND_RECORD_
#define _SND_PLAY_

#define FALSE 0
#define TRUE 1

//#define _MULTI_MACHINE_SUPPORT  //��ֻ�֧��

#define _DEBUG           //����ģʽ

//#define _CAPTUREPIC_TO_CENTER  //����ͼƬ��������
//#define _BRUSHIDCARD_SUPPORT   //ˢ������֧��

//���Է���������ģʽ
//������Ƶ��תģʽ
//#define _TESTTRANSMODE

//#define _MESSAGE_SUPPORT  //��Ϣ����

#define HARDWAREVER "M-HW VER 3.1"    //Ӳ���汾
#define SOFTWAREVER "M-SW VER 3.2"    //����汾
#define SERIALNUM "20110917"    //��Ʒ���к�
#define WAVFILEMAX 10

#define ZOOMMAXTIME 2000   //�Ŵ���С�ӳٴ���ʱ��
#define TOUCHMAXTIME 300   //�����������ӳٴ���ʱ��

#define INTRTIME 50       //�߳�50ms
#define INTRPERSEC 20       //ÿ��20���߳�

#define NSMULTIADDR  "238.9.9.1"  //NS�鲥��ַ
#define MULTITTL   5      //�ಥTTLֵ

#define COMMMAX 1024     //���ڻ��������ֵ
#define SCRWIDTH  800
#define SCRHEIGHT  480
#define D1_W  720
#define D1_H  480
#define CIF_W 352
#define CIF_H 240
#define REFRESH  1
#define NOREFRESH 0
#define SHOW  0
#define HIDE  1
#define UMOUNT  0
#define MOUNT   1
#define HILIGHT  2
#define IMAGEUP  0
#define IMAGEDOWN  1

#define DIRECTCALLTIME  600                 //ֱ�Ӻ���ÿ��ʱ��
#define TRANSCALLTIME  800                  //��ת����ÿ��ʱ��
#define WATCHTIMEOUT  60*INTRPERSEC//30*20    //�����ʱ��
#define CALLTIMEOUT  30*INTRPERSEC     //�����ʱ��
//#define CALLTIMEOUT  300*INTRPERSEC     //�����ʱ��
#define TALKTIMEOUT  240*INTRPERSEC//30*20     //ͨ���ʱ��
#define PREPARETIMEOUT  10*INTRPERSEC     //��Ӱ����Ԥ���ʱ��
#define RECORDTIMEOUT  30*INTRPERSEC     //��Ӱ�����ʱ��


//���� ��������
#define ALARM  1
#define SENDMESSAGE   3   //������Ϣ
#define REPORTSTATUS  4   //�豸��ʱ����״̬
#define QUERYSTATUS   5   //�������Ĳ�ѯ�豸״̬
#define SENDCOMMSRV   7   //������ҵ����
#define SEARCHNOTLOGIN   9   //����δ��¼���������豸
#define WRITEADDRESS   40   //д��ַ����
#define READADDRESS    41   //����ַ����
#define WRITECOMMMENU   42   //д��ҵ����˵�
#define READCOMMMENU   43   //����ҵ����˵�
#define WRITEHELP   50   //д������Ϣ
#define READHELP    51   //��������Ϣ
#define WRITESETUP     52   //д������Ϣ
#define READSETUP      53   //��������Ϣ

#ifdef _BRUSHIDCARD_SUPPORT   //ˢ������֧��
 #define _IDCARDHAVEADDR      //ID�����ַ
 #define IDCARDMAXNUM  100000  //ID���������
 #ifdef _IDCARDHAVEADDR      //ID�����ַ
  #define CARDPERPACK  50    //ÿ����ݰ���ID���������
  #define BYTEPERSN    24    //ÿ��SN ռ���ֽ���
 #else
  #define CARDPERPACK  350  //ÿ����ݰ���ID���������
  #define BYTEPERSN    4     //ÿ��SN ռ���ֽ���
 #endif
 #define IDCARDBRUSHNUM 10000 //�������ˢ������
 
 #define WRITEIDCARD    54   //дID����Ϣ
 #define READIDCARD     55   //��ID����Ϣ
 #define BRUSHIDCARD     56   //ˢID����Ϣ�������������
#endif

#ifdef _CAPTUREPIC_TO_CENTER  //����ͼƬ��������
 #define tmp_pic_file  "/tmp/capture_pic.jpg"
 #define MAX_CAPTUREPIC_NUM   20
 #define CAPTUREPIC_SEND_START     60   //��Ԫ�ſڻ�Χǽ������ſڻ��ͺ�����Ƭ->���Ϳ�ʼ
 #define CAPTUREPIC_SEND_DATA      61   //��Ԫ�ſڻ�Χǽ������ſڻ��ͺ�����Ƭ->�������
 #define CAPTUREPIC_SEND_SUCC      62   //��Ԫ�ſڻ�Χǽ������ſڻ��ͺ�����Ƭ->���ͽ���ɹ���
 #define CAPTUREPIC_SEND_FAIL      63   //��Ԫ�ſڻ�Χǽ������ſڻ��ͺ�����Ƭ->���ͽ���ʧ�ܣ�
#endif

//�Խ�
#define VIDEOTALK      150 //��������ӶԽ�
#define VIDEOTALKTRANS 151 //��������ӶԽ���ת����
#define VIDEOWATCH     152 //��������
#define VIDEOWATCHTRANS   153 //����������ת����
#define NSORDER        154 //����������������ڹ㲥��
#define NSSERVERORDER  155 //���������(NS������)


#define ASKALLOCSERVER  160 //���������Ƶ������(���������)
#define BEGINTRANS      161 //��ʼ��Ƶ��ת(����Ƶ������)
#define ENDTRANS        162 //������Ƶ��ת(����Ƶ������)

#define DOWNLOADFILE  224    //����Ӧ�ó���
#define DOWNLOADIMAGE  225    //����ϵͳӳ��
#define STARTDOWN  1       //��ʼ����
#define DOWN       2       //����
#define DOWNFINISHONE       3  //�������һ��
#define STOPDOWN       10      //ֹͣ����
#define DOWNFINISHALL       20 //ȫ���������
#define DOWNFAIL         21 //����ʧ��  �豸������������

#define REMOTEDEBUGINFO      253   //����Զ�̵�����Ϣ

#ifdef _REMOTECALLTEST  //Զ�̺��в���
 #define REMOTETEST      200   //����Զ�̺��в���
 #define STARTTEST  1    //��ʼ
#endif

#define ERASEFLASH  31    //����ɾ��Flash
#define WRITEFLASH  32    //����дFlash
#define CHECKFLASH  33    //����У��Flash
#define ENDFLASH  34      //���дImage
#define ERRORFLASH  35      //����Imageʧ��

#define ASK              1     //�������� ����
#define REPLY            2     //�������� Ӧ��

#define CALL             1     //����
#define LINEUSE          2     //ռ��
#define QUERYFAIL        3      //ͨ��ʧ��
#define CALLANSWER       4     //����Ӧ��
#define CALLSTART        6     //��ʼͨ��

#define CALLUP           7     //ͨ�����1�����з�->���з���
#define CALLDOWN         8     //ͨ�����2�����з�->���з���
#define CALLCONFIRM      9     //ͨ������ȷ�ϣ����շ����ͣ��Ա㷢�ͷ�ȷ�����ߣ�
#define REMOTEOPENLOCK   10     //Զ�̿���
#define FORCEIFRAME      11     //ǿ��I֡����
#define ZOOMOUT          15     //�Ŵ�(720*480)
#define ZOOMIN           16     //��С(352*288)

#define PREPARERECORD    20     //��ʼ��ӰԤ�������з�->���з������з�Ӧ��
#define RECORD           21     //��ʼ��Ӱ�����з�->���з������з�Ӧ��
#define JOINGROUP        22     //�����鲥�飨���з�->���з������з�Ӧ��
#define LEAVEGROUP       23     //�˳��鲥�飨���з�->���з������з�Ӧ��
#define TURNTALK         24     //ת�ӣ����з�->���з������з�Ӧ��

#define CALLEND          30     //ͨ������

#define SAVEMAX  10     //FLASH�洢�������ֵ
#define UDPSENDMAX  50  //UDP��η��ͻ������ֵ
#define COMMSENDMAX  20  //COMM��η��ͻ������ֵ
#define DISPARTMAX  10   //��ְ��ͻ���
#define MAXCOMMSENDNUM  3
#define MAXSENDNUM  6  //����ʹ���

#define SEARCHALLEQUIP  252  //���������豸���������ģ����豸��
#define WRITEEQUIPADDR      254  //д�豸��ַ���������ģ����豸��

#define TALK_IO            15   //���м�
#define CENTER_IO            14   //�������ļ�
#define OPENLOCK_IO          92   //����     GPIO2 28

#define WATCHDOG_IO_EN       93       //Watchdog ��0 ����    ��1����   GPIO2 29
#define WATCHDOG_IO_CLEAR    9        //��ʱ����ת����WatchDog         GPIO0  9

#define LCD_LIGHT_IO               2        //PWM GPIO0  2
#define PWM_IO               3        //PWM GPIO0  3

#define _M8_WATCHDOG_START  0x82
#define _M8_WATCHDOG_END  0x83

//��ťѹ��ʱ��
#define DELAYTIME  300


#define USER_DEV_NAME  "/dev/user_apply"

#define _INIT_GPIO  0
#define _SET_GPIO_OUTPUT_HIGH  1
#define _SET_GPIO_OUTPUT_LOW   2
#define _SET_GPIO_HIGH  3
#define _SET_GPIO_LOW   4
#define _SET_GPIO_INPUT  5
#define _GET_GPIO_VALUE  6


//��Ƶ����
#define cWhite  1
#define cYellow 2
#define cCyan   3
#define cGreen  4
#define cMagenta  5
#define cRed      6
#define cBlue     7
#define cBlack    8
#define	FB_DEV	"/dev/fb0"
#define MAXPIXELS (1280*1024)  /* Maximum size of final image */
#define VIDEO_PICTURE_QUEUE_SIZE_MAX 20

#define CONFLICTARP  0x8950
#define FLCD_GET_DATA_SEP   0x46db
#define FLCD_GET_DATA       0x46dc
#define FLCD_SET_FB_NUM     0x46dd
#define FLCD_SWITCH_MODE    0x46de
#define FLCD_CLOSE_PANEL    0x46df
#define FLCD_BYPASS    0x46e0
#define FLCD_OPEN	0x46fa
#define FLCD_CLOSE	0x46fb


//20071210 ���SD�������д����״̬
#define DEVICE_SD                "/dev/cpesda"
#define CHECK_SD_STATUS  0x2222
//20080401 ����PMU���رղ��õ�ʱ��
#define CLOSE_PMU1  0x2255
#define CLOSE_PMU2  0x2256

#define NMAX (512*48*2)  //��Ƶ���λ������С
#define G711NUM  (48*512*2)/AUDIOBLK         //��Ƶ���ջ�������� δ����   10

#define VIDEOMAX 720*576
#define VNUM  3         //��Ƶ�ɼ��������С
#define VPLAYNUM  10         //��Ƶ���Ż������С         6
#define MP4VNUM  10         //��Ƶ���ջ�������� δ����   10
#define PACKDATALEN  1200   //��ݰ��С
#define MAXPACKNUM  100     //֡�����ݰ�����

#define MAXRECWAV  30   //�����ʱ��
struct TimeStamp1{
    unsigned int OldCurrVideo;     //��һ�ε�ǰ��Ƶʱ��
    unsigned int CurrVideo;
    unsigned int OldCurrAudio;     //��һ�ε�ǰ��Ƶʱ��
    unsigned int CurrAudio;
   };
//��Ƶ�ɼ�����
struct videobuf1
 {
  int iput; // ���λ�����ĵ�ǰ����λ��
  int iget; // ������ĵ�ǰȡ��λ��
  int n; // ���λ������е�Ԫ��������
  uint32_t timestamp[VNUM]; //ʱ���
  uint32_t frameno[VNUM];   //֡���
  unsigned char *buffer_y[VNUM];//[VIDEOMAX];
  unsigned char *buffer_u[VNUM];//[VIDEOMAX/4];
  unsigned char *buffer_v[VNUM];//[VIDEOMAX/4];
 };
//��Ƶ���ջ���  δ����
struct tempvideobuf1
 {
//  int iput;                     // ���λ�����ĵ�ǰ����λ��
//  int iget;                     // ������ĵ�ǰȡ��λ��
//  int n;                        // ���λ������е�Ԫ��������
  uint32_t timestamp;  //ʱ���
  uint32_t frameno;       //֡���
  short TotalPackage;     //�ܰ���
  uint8_t CurrPackage[MAXPACKNUM]; //��ǰ��   1 �ѽ���  0 δ����
  int Len;                //֡��ݳ���
  uint8_t isFull;                  //��֡�ѽ�����ȫ
  unsigned char buffer[VIDEOMAX];
  unsigned char frame_flag;             //֡��־ ��Ƶ֡ I֡ P֡
 };                            //     [MP4VNUM]
//��Ƶ���Ż���
struct videoplaybuf1
 {
  uint8_t isUse;     //��֡�ѽ���δ����,���������
  uint32_t timestamp; //ʱ���
  uint32_t frameno;   //֡���
  unsigned char *buffer;//[VIDEOMAX];
  unsigned char frame_flag;             //֡��־ ��Ƶ֡ I֡ P֡
 };
//ͬ�����Žṹ
struct _SYNC
 {
  pthread_cond_t cond;       //ͬ���߳���������
  pthread_condattr_t cond_attr;
  pthread_mutex_t lock;      //������
  pthread_mutex_t audio_rec_lock;//[VPLAYNUM];//��Ƶ¼�ƻ�����
  pthread_mutex_t audio_play_lock;//[VPLAYNUM];//��Ƶ���Ż�����
  pthread_mutex_t video_rec_lock;//[VPLAYNUM];//��Ƶ¼�ƻ�����
  pthread_mutex_t video_play_lock;//[VPLAYNUM];//��Ƶ���Ż�����
  pthread_mutex_t audio_lock;
  unsigned int count;        //����
  uint8_t isDecodeVideo;     //��Ƶ�ѽ���һ֡  �����߳�-->ͬ���߳�
  uint8_t isPlayVideo;       //��Ƶ�Ѳ���һ֡  �����߳�-->ͬ���߳�
  uint8_t isDecodeAudio;     //��Ƶ�ѽ���һ֡  �����߳�-->ͬ���߳�
  uint8_t isPlayAudio;       //��Ƶ�Ѳ���һ֡  �����߳�-->ͬ���߳�
 };

//�ӻ�����?
struct audiobuf1
 {
  int iput; // ���λ�����ĵ�ǰ����λ��
  int iget; // ������ĵ�ǰȡ��λ��
  int i_echo;  //�������λ��
  int n; // ���λ������е�Ԫ��������
  uint32_t timestamp[NMAX/AUDIOBLK]; //ʱ���
  uint32_t frameno[NMAX/AUDIOBLK];   //֡���
  unsigned char buffer[NMAX];
  #ifdef _ECHO_STATE_SUPPORT  //�������֧��
   uint32_t echo_timestamp[NMAX/AUDIOBLK]; //ʱ���
  #endif
 };
struct WavFileBuf1 {
     int isValid;
     char wname[80];
     int PlayFlag;     //0 ���β���  1 ѭ������
     };
/*struct tempbuf1
 {
  int iput; // ���λ�����ĵ�ǰ����λ��
  int iget; // ������ĵ�ǰȡ��λ��
  int n; // ���λ������е�Ԫ��������
  uint32_t timestamp[NMAX/AUDIOBLK]; //ʱ���
  uint32_t frameno[NMAX/AUDIOBLK];   //֡���
  unsigned char buffer[NMAX];
 }; */
//��Ƶ���ջ���  δ����
struct tempaudiobuf1
 {
  uint32_t timestamp;  //ʱ���
  uint32_t frameno;       //֡���
  short TotalPackage;     //�ܰ���
  uint8_t CurrPackage[MAXPACKNUM]; //��ǰ��   1 �ѽ���  0 δ����
  int Len;                //֡��ݳ���
  uint8_t isFull;                  //��֡�ѽ�����ȫ
  unsigned char *buffer;//[AUDIOBLK];
  unsigned char frame_flag;             //֡��־ ��Ƶ֡ I֡ P֡
 };                            //     [MP4VNUM]
//��Ƶ���ջ��� ����
typedef struct node3{
               struct tempaudiobuf1 Content;
               struct node3 *llink, *rlink;
}TempAudioNode1; 

//��Ƶ���Ż���
struct audioplaybuf1
 {
  uint8_t isUse;     //��֡�ѽ���δ����,���������
  uint32_t timestamp; //ʱ���
  uint32_t frameno;   //֡���
  unsigned char *buffer;//[VIDEOMAX];
 };

//��ͥ���Ի���
struct wavbuf1
 {
  int iput; // ���λ�����ĵ�ǰ����λ��
  int iget; // ������ĵ�ǰȡ��λ��
  int n; // ���λ������е�Ԫ��������
  unsigned char *buffer;
 };

struct WaveFileHeader
{
 char chRIFF[4];
 uint32_t dwRIFFLen;
 char chWAVE[4];

 char chFMT[4];
 uint32_t dwFMTLen;
 uint16_t wFormatTag;
 uint16_t nChannels;
 uint32_t nSamplesPerSec;
 uint32_t nAvgBytesPerSec;
 uint16_t nBlockAlign;
 uint16_t wBitsPerSample;

 char chFACT[4];
 uint32_t dwFACTLen;

 char chDATA[4];
 uint32_t dwDATALen;
};


typedef struct _parameters {
//  char *jpegformatstr;
  char jpegformatstr[10];
  uint32_t begin;       /* the video frame start */
  int32_t numframes;   /* -1 means: take all frames */
//  y4m_ratio_t framerate;
//  y4m_ratio_t aspect_ratio;
  int interlace;   /* will the YUV4MPEG stream be interlaced? */
  int interleave;  /* are the JPEG frames field-interleaved? */
  int verbose; /* the verbosity of the program (see mjpeg_logging.h) */

  int width;
  int height;
  int colorspace;
  int loop;
  int rescale_YUV;
} parameters_t;

/*typedef */struct fcap_frame_buff
{
    unsigned int phyAddr;
    unsigned int mmapAddr;   //length per dma buffer
    unsigned int frame_no;
};

struct Local1{
               int _Door_Disp_Type;     //��ʾ����  1 -- 800x480   2 -- 320x240
               int Status;
               int NSReplyFlag;  //NSӦ�����־
               //״̬ 0 ���� 1 ���жԽ�  2 ���жԽ�  3 ����  4 ������  5 ����ͨ��
               //6 ����ͨ��  7 ������Ӱ����Ԥ��  8 ������Ӱ����Ԥ�� 9 ������Ӱ����
               //10 ������Ӱ����   11 ������Ӱ   30 IP ���

               int LoginServer;     //�Ƿ��¼��������<<��δ��¼����¼�Ǽ�ʱ���ӵ�¼��δ��¼���ж�3��>>
                              
               int CallConfirmFlag; //���߱�־
               int Timer1Num;  //��ʱ��1����
               int OnlineFlag; //��������ȷ��
               int OnlineNum;  //����ȷ�����
               int TimeOut;    //���ӳ�ʱ,  ͨ����ʱ,  ���г�ʱ�����˽���
               int RecPicSize;  //��Ƶ��С  1  352*288   2  720*480
               int PlayPicSize;  //��Ƶ��С  1  352*288   2  720*480
               pthread_mutex_t save_lock;//������
               pthread_mutex_t udp_lock;//������
               pthread_mutex_t comm_lock;//������
               #ifdef _BRUSHIDCARD_SUPPORT   //ˢ������֧��
                pthread_mutex_t idcard_lock;//������
                pthread_mutex_t dispart_send_lock;//������
               #endif
               int isSDInsert;  //SD���Ƿ����
               int isSDProtect; //SD��д����
               int isSDMounted;  //SD��Mount�ɹ�
               int SD_Status;  //SD����װ 0 -- ��  1--��  2--д����  3--��ȫ����  4--�ǰ�ȫ����
               int DefenceDelayFlag;    //������ʱ��־
               int DefenceDelayTime;   //����
               int PassLen;            //���볤��
               int SingleFortifyFlag;    //���������ʱ��־
               int SingleFortifyTime;   //�����������
               int ForceIFrame;    //1 ǿ��I֡
               int LMovieTime;     //��Ӱ�ļ����� ��
               int CalibratePos;   //У׼������ʮ��λ�� 0 1 2 3
               int CalibrateSucc;  //У׼�ɹ�
               int CurrFbPage; //��ǰFbҳ
               char ClockText[20];  //��ǰʱ��
               int InputMax;       //������󳤶�  Χǽ��  10λ  ��Ԫ�ſڻ�  4λ
               int ModiPassInputNum; //�޸������������
               char FirstPass[20];   //��һ�����������
               unsigned char IP_Group[4];  //�鲥��ַ
               unsigned char Weather[3];   //����Ԥ��

               #ifdef _CAPTUREPIC_TO_CENTER  //����ͼƬ��������
                int RecordPic;
                int IFrameCount; //֡����
                int IFrameNo;    //���ڼ���I֡
                int Pic_Size;                
                int Pic_Width;
                int Pic_Height;
                unsigned char yuv[D1_W*D1_H];
                int HavePicRecorded;  //����Ƭ��¼��
                struct tm *recpic_tm_t; //����Ƭʱ��
                char RemoteAddr[21];  //Զ�˵�ַ
                int SendToCetnering;  //���ڴ�������
                int ConnectCentered;   //����������״̬
               #endif               

               int AddrLen;          //��ַ����  S 12  B 6 M 8 H 6

               int isHost;           //'0' ���� '1' ���� '2' ...
               int DenNum;             //Ŀ������  ����
               unsigned char DenIP[10][4];    //����IP
               char DenAddr[10][21];         //����Addr

               unsigned char IP_VideoServer[4];  //��Ƶ��������ַ

               int NetStatus;   //����״̬ 1 �Ͽ�  0 ��ͨ
               int OldNetSpeed;  //�����ٶ�

               int ReportSend;  //�豸��ʱ����״̬�ѷ���
               int RandReportTime; //�豸��ʱ����״̬���ʱ��
               int ReportTimeNum;  //��ʱ

               int LcdLightFlag; //LCD�����־
               int LcdLightTime; //ʱ��

                                 //��GPIO�߳��в�ѯ���߳��Ƿ�����
               int Key_Press_Run_Flag;
               int Save_File_Run_Flag;
               #ifdef _BRUSHIDCARD_SUPPORT   //ˢ������֧��
                int Dispart_Send_Run_Flag;
               #endif 
               int Multi_Send_Run_Flag;
               int Multi_Comm_Send_Run_Flag;

               int PlayLMovieTipFlag; //����������ʾ��־

               int ResetPlayRingFlag;  //��λAudio Play flag

               int nowvideoframeno;   //��ǰ��Ƶ֡���
               int nowaudioframeno;   //��ǰ��Ƶ֡���

               int ForceEndWatch;  //�к���ʱ��ǿ�ƹؼ���
               int ZoomInOutFlag;  //���ڷŴ���С��
               uint32_t newzoomtime;
               uint32_t oldzoomtime;
               uint32_t newtouchtime;
               uint32_t oldtouchtime;    //��һ�δ���������ʱ��

               int _TESTNSSERVER;        //���Է���������ģʽ
               int _TESTTRANS;           //������Ƶ��תģʽ

               int SearchAllNo;  //���Ļ����������豸���
               int DownProgramOK; //����Ӧ�ó������
               int download_image_flag; //����ϵͳӳ��

               char DebugInfo[1024];
               char DebugIP[20];
               int RemoteDebugInfo;  //����Զ�̵�����Ϣ

               int CaptureVideoStartFlag;  //������Ƶ��ʼ��־     20101109  xu
               int CaptureVideoStartTime;  //������Ƶ��ʼ����

               char cLockFlg;								//��������ֹ������	add by lcx
              };
struct LocalCfg1{
               char Addr[20];             //��ַ����
               unsigned char Mac_Addr[6]; //���ַ
               unsigned char IP[4];       //IP��ַ
               unsigned char IP_Mask[4];  //��������
               unsigned char IP_Gate[4];  //��ص�ַ
               unsigned char IP_NS[4];    //NS����ƽ�������������ַ
               unsigned char IP_Server[4];  //����������ַ����NS��������Ϊͬһ����
               unsigned char IP_Broadcast[4];  //�㲥��ַ

               int ReportTime;      //�豸��ʱ����״̬ʱ��

               unsigned char LockType;   //��������  0 ���� �����  1 ���� �����
               unsigned char OpenLockTime;   //����ʱ��  ����� 0 �D�D100ms  1�D�D200ms
                                             //          ����� 0 �D�D5s  1�D�D10s

               unsigned char DelayLockTime;   //��ʱ����  0 0s  1 3s  2 5s  3 10s
               unsigned char PassOpenLock;   //���뿪��
               unsigned char CardOpenLock;   //ˢ������
               unsigned char CardComm;       //ˢ��ͨ��
               unsigned char DoorDetect;       //�Ŵż��
               unsigned char RoomType;        //��������  0 ������  1  �ַ���
               unsigned char CallWaitRing;        //���л���  0 ��ͨ��  1  ��ͨ��  2  ҡ����   3  ������

               char EngineerPass[10];           //��װ����
               char OpenLockPass[10];             //��������
               char HelpInfo[200];

               unsigned char VoiceHint;         //������ʾ  0 �ر�  1  ��
               unsigned char KeyVoice;          //������    0 �ر�  1  ��
               
               int Ts_X0;                   //������
               int Ts_Y0;
               int Ts_deltaX;
               int Ts_deltaY;
              };

#ifdef _BRUSHIDCARD_SUPPORT   //ˢ������֧��
//ID����Ϣ
struct IDCardNo1{
               int Num;        //����
               unsigned char SN[IDCARDMAXNUM*BYTEPERSN]; //ID����  ���10����
               uint32_t serialnum;     //���
              };
//дID����Ϣ
struct RecvIDCardNo1{
               int isNewWriteFlag;  //��д��־
               int Num;             //������
               int PackNum;         //������
               int PackIsRecved[IDCARDMAXNUM/CARDPERPACK + 1];  //���ѽ��ձ�־
               unsigned char SN[IDCARDMAXNUM*BYTEPERSN]; //ID����  ���10����
               uint32_t serialnum;     //���
              };
//ˢID����Ϣ
struct BrushIDCard1{
               int Num;        //����
               unsigned char Info[IDCARDBRUSHNUM*11]; //ID����  ������Ϣ���1����
               //ǰ4�ֽڿ��ţ���7�ֽ�ʱ��
              };
//��дID����ݽṹ
struct iddata1
  {
   #ifdef _IDCARDHAVEADDR      //ID�����ַ
    char Addr[20];       //��ַ����
   #endif 
   unsigned int serialnum;  //���
   unsigned int  Num;          //������
   unsigned int  CurrNum;           //��ǰ��ݰ�����
   unsigned int  TotalPackage;      //�ܰ���
   unsigned int  CurrPackage;       //��ǰ����
  }__attribute__ ((packed));              
#endif
#ifdef _CAPTUREPIC_TO_CENTER  //����ͼƬ��������
 //����ͼƬ��������
 struct Capture_Pic_Center1{
                int isVilid;             //
                char RemoteAddr[21];
                int jpegsize;
                unsigned char *jpeg_pic; //��ƬJPG���
                struct tm *recpic_tm_t; //����Ƭʱ��
               };
#endif

//LCD��Ļ��ʾ
struct LCD_Display1{
                    int isFinished;   //��ʾ���
                    int ShowFlag[4];   //0  ��  1  ����
                    int PrevShowFlag[4];
                    char PrevText[4][20];  //��һ��
                    char CurrText[4][20];  //��ǰ��
                    int MaxRow;  //������ʾ����к�
                   };
struct DefenceCfg1{
               unsigned char DefenceStatus;       //����״̬
               unsigned char DefenceNum;          //����ģ�����
               unsigned char DefenceInfo[32][10]; //������Ϣ
              };
//���ڽ��ջ�����
struct commbuf1
 {
  int iput; // ���λ�����ĵ�ǰ����λ��
  int iget; // ������ĵ�ǰȡ��λ��
  int n; // ���λ������е�Ԫ��������
  unsigned char buffer[COMMMAX];
 };
//״̬��ʾ��Ϣ����
//Type
//          11 -- ��������
//          12 -- ����ͨ��
//          13 -- ����
//          14 -- �����
//          15 -- �鿴��Ӱ
//          16 -- �Խ�ͼ�񴰿�

struct Remote1{
               int DenNum;             //Ŀ������  ����+����
               unsigned char DenIP[4]; //�Է�IP����Ƶ������IP
               unsigned char GroupIP[4]; //GroupIP
               unsigned char IP[10][4];    //�Է�IP
               int Added[10];                //�Ѽ�����
               char Addr[10][21];         //�Է�Addr
               int isDirect;       //�����Ƿ�ֱͨ  0 ֱͨ  1 ��ת
           //    int isAssiDirect;       //�����Ƿ�ֱͨ  0 ֱͨ  1 ��ת
              };


//�����������ݽṹ��
struct LeaveWord1{
               uint8_t isValid;  //��Ч��δɾ���־   1
               uint8_t isReaded; //�Ѷ���־    1
               uint8_t isLocked; //���־    1
               char Time[32];    //����ʱ��    32
               uint8_t Type;     //����        1   1--����  2 --�绰
               uint32_t Sn;      //���        4
               int Length;       //����        4
              };
//��������
typedef struct leavewordnode{
               struct LeaveWord1 LWord;
               struct node *llink, *rlink;
}LeaveWordNode1;                                       //�ڴ����Ϊ444

//�洢�ļ���FLASH���� ��ݽṹ ���ڴ洢FLASH�ٶȽ��� ���߳�������
struct Save_File_Buff1{
               int isValid; //�Ƿ���Ч
               int Type;    //�洢���� 1��һ����Ϣ  2��������Ϣ  3����ҵ����  4����������
               int InfoType;   //��Ϣ����
               int InfoNo;     //��Ϣλ��
               char cFromIP[15];   //���ڶ�ID��Ӧ��
              };

//UDP����������ݷ��ͽṹ
struct Multi_Udp_Buff1{
               int isValid; //�Ƿ���Ч
               int SendNum; //��ǰ���ʹ���
               int CurrOrder;//��ǰ����״̬,VIDEOTALK VIDEOTALKTRANS VIDEOWATCH VIDEOWATCHTRANS
                             //��Ҫ���������ʱ���絥��������Ϊ0
               int m_Socket;
               char RemoteHost[20];
               char RemoteIP[20];               
               char DenIP[20]; //Ŀ���ַ
               int RemotePort;
               unsigned char buf[1500];
               int DelayTime;  //�ȴ�ʱ��
               int SendDelayTime; //���͵ȴ�ʱ�����  20101112
               int nlength;
              };
//COMM����������ݷ��ͽṹ
struct Multi_Comm_Buff1{
               int isValid; //�Ƿ���Ч
               int SendNum; //��ǰ���ʹ���
               int m_Comm;
               unsigned char buf[1500];
               int nlength;
              };
//���������
struct Call_Input1{
               int Len; //�Ƿ���Ч
               char buf[20];
              };
//ͨ����ݽṹ
struct talkdata1
  {
//   unsigned char Order;     //��������
   char HostAddr[20];       //���з���ַ
   unsigned char HostIP[4]; //���з�IP��ַ
   char AssiAddr[20];       //���з���ַ
   unsigned char AssiIP[4]; //���з�IP��ַ
   unsigned int timestamp;  //ʱ���
   unsigned short DataType;          //�������
   unsigned short Frameno;           //֡���
   unsigned int Framelen;            //֡��ݳ���
   unsigned short TotalPackage;      //�ܰ���
   unsigned short CurrPackage;       //��ǰ����
   unsigned short Datalen;           //��ݳ���
   unsigned short PackLen;       //��ݰ���󳤶�
//   char HeadSpace[9];
   }__attribute__ ((packed));
//��Ϣ��ݽṹ
struct infodata1
  {
   char Addr[20];       //��ַ����
   unsigned short Type; //����
   unsigned int  Sn;         //���
   unsigned short Length;   //��ݳ���
  }__attribute__ ((packed));

struct downfile1
  {
   char FlagText[20];     //��־�ַ�
   char FileName[20];
   unsigned int Filelen;            //�ļ���С
   unsigned short TotalPackage;      //�ܰ���
   unsigned short CurrPackage;       //��ǰ����
   unsigned short Datalen;           //��ݳ���
  }__attribute__ ((packed));  
#ifndef CommonH
#define CommonH

  extern int DebugMode;

  struct TimeStamp1 TimeStamp;  //����ʱ���벥��ʱ�䣬ͬ����
  //�������Ż���
  struct WavFileBuf1 WavFileBuf[WAVFILEMAX];
  int DeltaLen;  //��ݰ���Ч���ƫ����
  char wavFile[80];
  struct tm *curr_tm_t;
  int temp_audio_n;      //��Ƶ���ջ������
  TempAudioNode1 *TempAudioNode_h;    //��Ƶ���ջ����б�
    
  //ϵͳ��ʼ����־
  int InitSuccFlag;
  //����״̬����
  struct Local1 Local;
  struct LocalCfg1 LocalCfg;

  #ifdef _BRUSHIDCARD_SUPPORT   //ˢ������֧��
   //ID����Ϣ
   struct IDCardNo1 IDCardNo;
   //ˢID����Ϣ
   struct BrushIDCard1 BrushIDCard;
   //дID����Ϣ
   struct RecvIDCardNo1 RecvIDCardNo;
  #endif
  #ifdef _CAPTUREPIC_TO_CENTER  //����ͼƬ��������
   //����ͼƬ��������
   int Capture_Pic_Num;
   struct Capture_Pic_Center1 Capture_Pic_Center[MAX_CAPTUREPIC_NUM];
   int Capture_Total_Package;
   unsigned char Capture_Send_Flag[2000];
  #endif
    
  //����״̬
  struct DefenceCfg1 DefenceCfg;

  //Զ�˵�ַ
  struct Remote1 Remote;
  char NullAddr[21];   //���ַ�
  //���������
  struct Call_Input1 Call_Input;
  //COMM
  int Comm2fd;  //����2���
  int Comm3fd;  //����3���
  int Comm4fd;  //����4���
  //I2C
  int i2c_fd;
  //���ARP
  int ARP_Socket;
  //�����������
  int m_EthSocket;
  //UDP
  int m_DataSocket;
  int m_VideoSocket;
  int LocalDataPort;   //������UDP�˿�
  int LocalVideoPort;  //����ƵUDP�˿�
  int RemoteDataPort;
  int RemoteVideoPort;
  int RemoteVideoServerPort;  //��Ƶ����������ƵUDP�˿�
  char RemoteHost[20];
  char sPath[80];
  char currpath[80];   //�Զ���·��
  char wavPath[80];
  char UdpPackageHead[15];
  //FLASH�洢�߳�
  int save_file_flag;
  pthread_t save_file_thread;
  void save_file_thread_func(void);
  sem_t save_file_sem;
  struct Save_File_Buff1 Save_File_Buff[SAVEMAX]; //FLASH�洢�������ֵ

  #ifdef _BRUSHIDCARD_SUPPORT   //ˢ������֧��
   //��ְ����߳�    ���ID��Ӧ��ʱ
   int dispart_send_flag;
   pthread_t dispart_send_thread;
   void dispart_send_thread_func(void);
   sem_t dispart_send_sem;
   struct Save_File_Buff1 Dispart_Send_Buff[DISPARTMAX]; //��ְ�����ֵ
  #endif 

  //����������ݷ����̣߳��ն����������������ʱһ��û�յ���Ӧ�����η���
  //����UDP��Commͨ��
  int multi_send_flag;
  pthread_t multi_send_thread;
  void multi_send_thread_func(void);
  sem_t multi_send_sem;
  struct Multi_Udp_Buff1 Multi_Udp_Buff[UDPSENDMAX]; //10��UDP�������ͻ���

  //����������ݷ����̣߳��ն����������������ʱһ��û�յ���Ӧ�����η���
  //����UDP��Commͨ��
  int multi_comm_send_flag;
  pthread_t multi_comm_send_thread;
  void multi_comm_send_thread_func(void);
  sem_t multi_comm_send_sem;
  struct Multi_Comm_Buff1 Multi_Comm_Buff[COMMSENDMAX]; //10��COMM�������ͻ���
#else

  extern int DebugMode;           //����ģʽ
  extern struct TimeStamp1 TimeStamp;  //����ʱ���벥��ʱ�䣬ͬ����
  //�������Ż���
  extern struct WavFileBuf1 WavFileBuf[WAVFILEMAX];
  extern int DeltaLen;  //��ݰ���Ч���ƫ����

  extern char wavFile[80];

  extern struct tm *curr_tm_t;

  extern int temp_audio_n;      //��Ƶ���ջ������
  extern TempAudioNode1 *TempAudioNode_h;    //��Ƶ���ջ����б�
  //ϵͳ��ʼ����־
  extern int InitSuccFlag;  
  //����״̬����
  extern struct Local1 Local;
  extern struct LocalCfg1 LocalCfg;
  
  #ifdef _BRUSHIDCARD_SUPPORT   //ˢ������֧��
   //ID����Ϣ
   extern struct IDCardNo1 IDCardNo;
   //ˢID����Ϣ
   extern struct BrushIDCard1 BrushIDCard;
   //дID����Ϣ
   extern struct RecvIDCardNo1 RecvIDCardNo;
  #endif

  #ifdef _CAPTUREPIC_TO_CENTER  //����ͼƬ��������
   //����ͼƬ��������
   extern int Capture_Pic_Num;
   extern struct Capture_Pic_Center1 Capture_Pic_Center[MAX_CAPTUREPIC_NUM];
   extern int Capture_Total_Package;
   extern unsigned char Capture_Send_Flag[2000];
  #endif
    
  //����״̬
  extern struct DefenceCfg1 DefenceCfg;
  //Զ�˵�ַ
  extern struct Remote1 Remote;
  extern char NullAddr[21];   //���ַ�
  //���������
  extern struct Call_Input1 Call_Input;  
  //COMM
  extern int Comm2fd;  //����2���
  extern int Comm3fd;  //����3���
  extern int Comm4fd;  //����4���
  //I2C
  extern int i2c_fd;
  //���ARP
  extern int ARP_Socket;
  //�����������
  extern int m_EthSocket;
  //UDP
  extern int m_DataSocket;
  extern int m_VideoSocket;
  extern int LocalDataPort;   //������UDP�˿�
  extern int LocalVideoPort;  //����ƵUDP�˿�
  extern int RemoteDataPort;
  extern int RemoteVideoPort;
  extern int RemoteVideoServerPort;  //��Ƶ����������ƵUDP�˿�
  extern char RemoteHost[20];
  extern char sPath[80];
  extern char currpath[80];   //�Զ���·��
  extern char wavPath[80];
  extern char UdpPackageHead[15];
  //FLASH�洢�߳�
  extern int save_file_flag;
  extern pthread_t save_file_thread;
  extern void save_file_thread_func(void);
  extern sem_t save_file_sem;
  extern struct Save_File_Buff1 Save_File_Buff[SAVEMAX]; //FLASH�洢�������ֵ

  #ifdef _BRUSHIDCARD_SUPPORT   //ˢ������֧��
   //��ְ����߳�    ���ID��Ӧ��ʱ
   extern int dispart_send_flag;
   extern pthread_t dispart_send_thread;
   extern void dispart_send_thread_func(void);
   extern sem_t dispart_send_sem;
   extern struct Save_File_Buff1 Dispart_Send_Buff[DISPARTMAX]; //��ְ�����ֵ
  #endif 

  //����������ݷ����̣߳��ն����������������ʱһ��û�յ���Ӧ�����η���
  //����UDP��Commͨ��
  extern int multi_send_flag;
  extern pthread_t multi_send_thread;
  extern void multi_send_thread_func(void);
  extern sem_t multi_send_sem;
  extern struct Multi_Udp_Buff1 Multi_Udp_Buff[UDPSENDMAX]; //10��UDP�������ͻ���
  //����������ݷ����̣߳��ն����������������ʱһ��û�յ���Ӧ�����η���
  //����UDP��Commͨ��
  extern int multi_comm_send_flag;
  extern pthread_t multi_comm_send_thread;
  extern void multi_comm_send_thread_func(void);
  extern sem_t multi_comm_send_sem;
  extern struct Multi_Comm_Buff1 Multi_Comm_Buff[COMMSENDMAX]; //10��COMM�������ͻ���
#endif
