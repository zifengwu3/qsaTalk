#include <jni.h>
#include <stdio.h>
#include <stdlib.h>

#include "log_jni.h"

#define _Included_Jni_Load
#include "jni_load.h"

#include "code_c_in_java.h"

/*
 * Class:     com_qsatalk_JniLoad
 * Method:    sendHello
 * Signature: ()Ljava/lang/String;
 */

JNIEXPORT jstring JNICALL native_sendHello(JNIEnv *env, jobject obj) {
  return ( *env )->NewStringUTF(env, "Good Hello World!");
}

/*
 * Class:     com_qsatalk_JniLoad
 * Method:    SayHelloInC
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */

//处理字符串追加
JNIEXPORT jstring JNICALL native_SayHelloInC (JNIEnv * env, jobject obj, jstring str) {
  char * p = NULL;
  jstring rtnStr;
  p = Jstring2CStr(env, str);
  LOGD("%s", p);
  char newstr[30] = "append string";
  LOGD("END");
  rtnStr = (* env)->NewStringUTF(env, strcat(p, newstr));
  return rtnStr;
}