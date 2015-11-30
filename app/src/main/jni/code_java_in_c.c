#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log_jni.h"
#include "code_java_in_c.h"

#define JNICALLBACK_CLASS "com/qsatalk/jniLoad"

/*
 * Java和JNI函数绑定
 * */
static JNINativeMethod callback_method_table[] = {
        {"helloFromJava", "()V", NULL},
        {"printString", "(Ljava/lang/String;)V", NULL},
        {"Add", "(II)I", NULL},
};

JNIEXPORT void JNICALL Java_com_qsatalk_jniLoad_callCcode
        (JNIEnv * env , jobject obj) {

    // 获取到class对象
    jclass clazz = (*env)->FindClass(env, JNICALLBACK_CLASS);
    if (clazz == 0) {
        LOGD("not find class!");
    } else {
        LOGD("find class");
    }
    //第三个参数 和第四个参数 是方法的签名,第三个参数是方法名  , 第四个参数是根据返回值和参数生成的
    //获取到class要调用的方法
    jmethodID methodID = (*env)->GetMethodID(
            env, clazz,
            callback_method_table[0].name,
            callback_method_table[0].signature);
    if (methodID == 0) {
        LOGD("not find method!");
    } else {
        LOGD("find method");
    }
    //调用这个方法
    (*env)->CallVoidMethod(env, obj, methodID);
}

JNIEXPORT void JNICALL Java_com_qsatalk_jniLoad_callCcode1
        (JNIEnv * env, jobject obj)
{
    // 获取到class对象
    jclass clazz = (*env)->FindClass(env, JNICALLBACK_CLASS);
    if (clazz == 0) {
        LOGD("not find class!");
    } else {
        LOGD("find class");
    }
    //第三个参数 和第四个参数 是方法的签名,第三个参数是方法名  , 第四个参数是根据返回值和参数生成的
    //获取到class要调用的方法
    jmethodID methodID = (*env)->GetMethodID(
            env, clazz,
            callback_method_table[1].name,
            callback_method_table[1].signature);
    if (methodID == 0) {
        LOGD("not find method!");
    } else {
        LOGD("find method");
    }

    //调用这个方法
    (*env)->CallVoidMethod(env, obj, methodID, (*env)->NewStringUTF(env,"HHHHH-haha"));
}

JNIEXPORT void JNICALL Java_com_qsatalk_jniLoad_callCcode2
        (JNIEnv * env , jobject obj) {

    // 获取到class对象
    jclass clazz = (*env)->FindClass(env, JNICALLBACK_CLASS);
    if (clazz == 0) {
        LOGD("not find class!");
    } else {
        LOGD("find class");
    }
    //第三个参数 和第四个参数 是方法的签名,第三个参数是方法名  , 第四个参数是根据返回值和参数生成的
    //获取到class要调用的方法
    jmethodID methodID = (*env)->GetMethodID(
            env, clazz,
            callback_method_table[2].name,
            callback_method_table[2].signature);
    if (methodID == 0) {
        LOGD("not find method!");
    } else {
        LOGD("find method");
    }
    //调用这个方法
    (*env)->CallIntMethod(env, obj, methodID, 34, 59);
}
