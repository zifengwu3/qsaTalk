#include <jni.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/log_jni.h"

#define _Included_Jni_Load
#include "../jni_load.h"

#include "qsa_serial.h"

/*
 * Class:     com_qsaserial_jniserialLoad
 * Method:    init_serial_qsa
 * Signature: ()Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL native_init_serial_qsa(JNIEnv *env, jobject obj) {
  return ( *env )->NewStringUTF(env, "init serial success");
}

