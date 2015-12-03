//UDP
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#define CommonH
#include "include/common.h"

#include <jni.h>
//#include "communication-jni.h"

extern int UdpSendBuff(int m_Socket, char *RemoteHost, int RemotePort,
                unsigned char *buf, int nlength);
struct videobuf1 videorecbuf;
//-----------------------------------------------------------------------
void Send_Video_Data_Task(char * p_VideoData, int size)
{
    struct timeval tv, tv1;
    uint32_t nowtime, prevtime;
    int dwSize;
    unsigned char mpeg4_out[1600];
    struct talkdata1 talkdata;
    int TotalPackage;
    int j;
    char tmphost[20];
    int kk;

    strcpy(tmphost, "192.168.10.84");
    dwSize = size;

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    prevtime = nowtime;

    if ((Local.nowvideoframeno != 1 && (Local.nowvideoframeno - 1) % 31)
            == 0) {
        if (Local.ForceIFrame == 1) {
            Local.ForceIFrame = 0;
            //forceiframe_func();
        }
    }

    Local.nowvideoframeno++;
    if (Local.nowvideoframeno >= 65536)
        Local.nowvideoframeno = 1;

    gettimeofday(&tv, NULL);
    nowtime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    gettimeofday(&tv1, NULL);

    nowtime = (tv.tv_sec - tv1.tv_sec) * 1000
        + (tv.tv_usec - tv1.tv_usec) / 1000;

    if (dwSize < 1024) {
        memset(p_VideoData + dwSize, 0x00, 1024 - dwSize);
        dwSize = 1024;
    }
    dwSize = ((dwSize >> 9) << 9);
    //printf("Video 0 quant %d blength= %d\n",ratec.rtn_quant,dwSize);
    //printf("frame=%d TIME=%d dwsize=%d\n",videorecbuf.frameno[videorecbuf.iput],nowtime,dwSize);
#if 1
    talkdata.timestamp = nowtime;
    talkdata.Frameno = Local.nowvideoframeno;
    memcpy(mpeg4_out, UdpPackageHead, 6);
    Remote.isDirect = 0;
    if ((Local.Status == 0) || (Local.Status == 1)
        || (Local.Status == 2) || (Local.Status == 5)
            || (Local.Status == 6) || (Local.Status == 7)
            || (Local.Status == 8) || (Local.Status == 9)
            || (Local.Status == 10))
    {
        if (Remote.isDirect == 1) {
            mpeg4_out[6] = VIDEOTALKTRANS;
        } else {
            mpeg4_out[6] = VIDEOTALK;
        }
    }
    if ((Local.Status == 3) || (Local.Status == 4))
    {
        if (Remote.isDirect == 1) {
            mpeg4_out[6] = VIDEOWATCHTRANS;
        } else {
            mpeg4_out[6] = VIDEOWATCH;
        }
    }

    mpeg4_out[7] = 1;
    if ((Local.Status == 0) || (Local.Status == 1)
        || (Local.Status == 3) || (Local.Status == 5)
            || (Local.Status == 7) || (Local.Status == 9))
    {
        mpeg4_out[8] = CALLUP;
        memcpy(talkdata.HostAddr, LocalCfg.Addr, 20);
        memcpy(talkdata.HostIP, LocalCfg.IP, 4);
        memcpy(talkdata.AssiAddr, Remote.Addr[0], 20);
        if ((mpeg4_out[6] == VIDEOTALKTRANS)
                || (mpeg4_out[6] == VIDEOWATCHTRANS)) {
            memcpy(talkdata.AssiIP, Remote.DenIP, 4);
        } else {
            memcpy(talkdata.AssiIP, Remote.IP[0], 4);
        }
    }

    if ((Local.Status == 2) || (Local.Status == 4) || (Local.Status == 6)
            || (Local.Status == 8) || (Local.Status == 10))  //����Ϊ���з�
    {
        mpeg4_out[8] = CALLDOWN;
        memcpy(talkdata.HostAddr, Remote.Addr[0], 20);
        if ((mpeg4_out[6] == VIDEOTALKTRANS)
                || (mpeg4_out[6] == VIDEOWATCHTRANS)) {
            memcpy(talkdata.HostIP, Remote.DenIP, 4);
        } else {
            memcpy(talkdata.HostIP, Remote.IP[0], 4);
        }
        memcpy(talkdata.AssiAddr, LocalCfg.Addr, 20);
        memcpy(talkdata.AssiIP, LocalCfg.IP, 4);
    }

    talkdata.Framelen = dwSize;
    //720*480 I֡
    talkdata.DataType = 4;

    //if (Local.Status != 0)
    {
        talkdata.PackLen = PACKDATALEN;
        if ((dwSize % talkdata.PackLen) == 0)
            TotalPackage = dwSize / talkdata.PackLen;
        else
            TotalPackage = dwSize / talkdata.PackLen + 1;
        talkdata.TotalPackage = TotalPackage;
        sprintf(RemoteHost, "%d.%d.%d.%d\0", Remote.DenIP[0],
                Remote.DenIP[1], Remote.DenIP[2], Remote.DenIP[3]);
        strcpy(RemoteHost, tmphost);
        for (j = 1; j <= TotalPackage; j++)
        {
            if (j == TotalPackage) {
                talkdata.CurrPackage = j;
                talkdata.Datalen = dwSize - (j - 1) * talkdata.PackLen;
                memcpy(mpeg4_out + 9, &talkdata, sizeof(talkdata));
                memcpy(mpeg4_out + DeltaLen,
                        p_VideoData + (j - 1) * talkdata.PackLen,
                        (dwSize - (j - 1) * talkdata.PackLen));
                UdpSendBuff(m_VideoSocket, tmphost, RemoteVideoPort,
                            mpeg4_out , DeltaLen + (dwSize-(j-1)*talkdata.PackLen));
            } else {
                talkdata.CurrPackage = j;
                talkdata.Datalen = talkdata.PackLen;
                memcpy(mpeg4_out + 9, &talkdata, sizeof(talkdata));
                memcpy(mpeg4_out + DeltaLen,
                        p_VideoData + (j - 1) * talkdata.PackLen,
                        talkdata.PackLen);
                 UdpSendBuff(m_VideoSocket, tmphost, RemoteVideoPort,
                             mpeg4_out , DeltaLen + talkdata.PackLen);
            }
            if (Local.nowvideoframeno == 2) {
                if (TotalPackage > 70) {
                    usleep(1);
                } else {
                    for (kk = 0; kk < 10000; kk++);
                }
            }
        }
    }
#endif
}
