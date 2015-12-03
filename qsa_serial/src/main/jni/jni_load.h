//
// Created by l on 11/30/15.
//
#include <jni.h>

#ifndef _Included_Jni_Load
#define _Included_Jni_Load
char * Jstring2CStr(JNIEnv * env, jstring jstr);
#else
extern char * Jstring2CStr(JNIEnv * env, jstring jstr);
#endif


