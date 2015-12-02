//
// Created by l on 11/27/15.
//

#ifndef ANDROIDJNI_LIB_LOGGER_H
#define ANDROIDJNI_LIB_LOGGER_H

#include <jni.h>
#include <android/log.h>

/**
 * 定义log标签
 */
#define TAG "jni_qsatalk"

/**
 * 定义info信息
 */
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

/**
 * 定义debug信息
 */
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)

/**
 * 定义error信息
 */
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

#endif //ANDROIDJNI_LIB_LOGGER_H
