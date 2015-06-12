#include <jni.h>

#ifndef LOGGER_H
#define LOGGER_H

#include <strings.h>
#include <android/log.h>

#define TAG "OpenMaka::Native"

#define LOGV(TAG,...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, "VERBOSE: " ,__VA_ARGS__)
#define LOGD(TAG,...) __android_log_print(ANDROID_LOG_DEBUG  , TAG, "DEBUG: "   ,__VA_ARGS__)
#define LOGI(TAG,...) __android_log_print(ANDROID_LOG_INFO   , TAG, "INFO: "    ,__VA_ARGS__)
#define LOGW(TAG,...) __android_log_print(ANDROID_LOG_WARN   , TAG, "WARN: "    ,__VA_ARGS__)
#define LOGE(TAG,...) __android_log_print(ANDROID_LOG_ERROR  , TAG, "ERROR: "   , __VA_ARGS__)

#endif
