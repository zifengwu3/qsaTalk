#ifndef SNDTOOLS_H
#define SNDTOOLS_H

#include <linux/soundcard.h>

#define AUDIODSP  1
#define AUDIODSP1 2

#define FMT8BITS 8 
#define FMT16BITS 16

#define FMT8K 8000
#define FMT16K 16000
#define FMT22K 22000
#define FMT44K 44000

#define MONO 0
#define STERO 1

#define AUDIORECBLK  128
#define AUDIOPLAYBLK  128

//#define AUDIOBLK 512   //ÿ֡32ms
#define AUDIOBLK 128//128   //ÿ֡8ms
#define TAIL 1280//1600//1280
#define AFRAMETIME (AUDIOBLK/16)   //ÿ֡ms

//#define _ECHO_CANCEL_AUDIO

//#define _SOFTWARE_REC_MAGNIFY
#define _SOFTWARE_REC_MULTIPLE   1
//#define _SOFTWARE_PLAY_MAGNIFY
#define _SOFTWARE_MULTIPLE   1
//#define _RECORD_PCM_FILE
//#define _READ_PCM_FILE

extern int AudioRecIsStart;
extern int AudioPlayIsStart;

extern int devrecfd;
extern int devplayfd;

//Open sound device, return 1 if open success
//else return 0
int OpenSnd(int nWhich);     //1 record  2 play

//Close sound device
int CloseSnd(int nWhich);

//Set record or playback format, return 1 if success
//else return 0
int SetFormat(int nWhich, int bits, int hz, int chn);

//Record
int Record(char *buf, int size);

//Playback
int Play(char *buf, int size);

#endif //ifndef SNDTOOLS_H
