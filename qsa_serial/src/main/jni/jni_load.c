//
// Created by l on 11/27/15.
//

#include "include/log_jni.h"
#include "string.h"
#define _Included_com_qsaserial
#include "jni_qsa_serial.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

/*
 * 获取数组大小
 * */
#define NELEM(x) ((int)(sizeof(x) / sizeof((x)[0])))

/*
 * 设置string处理的编码
 * */
#define ENCODE_GB2312 "GB2312"
#define ENCODE_UTF_16 "UTF-16"
#define ENCODE_UTF_8 "UTF-8"

/*
 * 指定要注册的类，对应完整的java类名
 * */
#define JNIREG_CLASS "com/qsaserial/jniserialLoad"

JNIEXPORT jint JNI_OnLoad(JavaVM * vm, void * reserved) {
    JNIEnv * env = NULL;
    jint result = (-1);
    if ((*vm)->GetEnv(vm, (void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return result;
    }

    register_ndk_load(env);
    return JNI_VERSION_1_6;
}

/*
 * 注册native方法到java中
 * */
static int registerNativeMethods(JNIEnv *env, const char * className,
        JNINativeMethod * gMethods, int numMethods) {
    jclass clazz;
    clazz = (* env)->FindClass(env, className);
    if (clazz == NULL) {
        return JNI_FALSE;
    }

    if ((* env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
        return  JNI_FALSE;
    }
    return  JNI_TRUE;
}

/*
 * Java和JNI函数绑定
 * */
static JNINativeMethod method_table[] = {
        {"init_serial_qsa", "()Ljava/lang/String;", (void *)native_init_serial_qsa},
};

/*
 * 调用注册方法
 * */
int register_ndk_load(JNIEnv * env) {
    return registerNativeMethods(env, JNIREG_CLASS, method_table, NELEM(method_table));
}

/*
 * java中的jstring, 转化为c的一个字符数组
 * */
char * Jstring2CStr(JNIEnv * env, jstring jstr)
{
    char * rtn = NULL;

    jclass cls_string = (*env)->FindClass(env, "java/lang/String");
    jstring str_encode = (*env)->NewStringUTF(env, ENCODE_UTF_8);
    jmethodID mid = (*env)->GetMethodID(env, cls_string, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray)(*env)->CallObjectMethod(env, jstr, mid, str_encode);
    jsize len = (*env)->GetArrayLength(env, barr);
    jbyte * ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
    if (len > 0) {
        rtn = (char *)malloc(len + 1);
        memcpy(rtn, ba, len);
        rtn[len] = '\0';
    }
    (*env)->ReleaseByteArrayElements(env, barr, ba, 0);  //释放内存

    return rtn;
}


