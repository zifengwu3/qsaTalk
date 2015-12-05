#include <jni.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/log_jni.h"

#define _Included_Jni_Load
#include "../jni_load.h"

#include "jni_qsa_serial.h"
#include "include/common.h"

/*
 * Class:     com_qsaserial_jniserialLoad
 * Method:    init_serial_qsa
 * Signature: ()Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL native_init_serial_qsa(JNIEnv *env, jobject obj) {
    LOGD(TAG, "start open comm for 485");
    //Init_MultiCommThread(); //初始化 Comm发送线程模块
    //初始化串口
    Comm2fd = OpenComm(2, 9600, 8, 1, 'N');
    LOGD(TAG, "end open comm for 485");
    if (Comm2fd > 0) {
        return (*env)->NewStringUTF(env, "init serial success!");
    } else {
        return (*env)->NewStringUTF(env, "init serial fail!");
    }
}

