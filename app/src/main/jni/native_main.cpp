#include <jni.h>
#include <string.h>
#include <opencv2/core/core.hpp>

#include "native_main.h"
#include "native_logger.h"

#include "Application/Controller.hpp"

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1start
  (JNIEnv *, jclass clazz)
{
    Controller::getInstance()->start();
}

JNIEXPORT jboolean JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1initialize
  (JNIEnv *env, jclass clazz, jlong mAddrGray, jstring configPath)
{
    const char *strMsgPtr = env->GetStringUTFChars( configPath , 0);
    if (strMsgPtr != NULL) {
        std::string pathString(strMsgPtr);
        Controller::getInstance()->initialize(*(cv::Mat*)mAddrGray, pathString);
    } else {
        Controller::getInstance()->initialize(*(cv::Mat*)mAddrGray, NULL);
    }
    env->ReleaseStringChars(configPath, (jchar *)strMsgPtr);
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1displayFunction
  (JNIEnv *env, jclass clazz, jlong mRgbaAddr, jlong mGrayAddr)
{
	return Controller::getInstance()->displayFunction(*(cv::Mat*)mRgbaAddr, *(cv::Mat*)mGrayAddr);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1glRender
  (JNIEnv *env, jclass clazz)
{
    Controller::getInstance()->glRender();
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1glResize
  (JNIEnv *env, jclass clazz, jint width, jint height)
{
    Controller::getInstance()->glResize(width, height);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setObjectDetection
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
    Controller::getInstance()->setObjectDetection(isActive);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setTracking
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
    Controller::getInstance()->setTracking(isActive);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setOpenGL
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
    Controller::getInstance()->setOpenGL(isActive);
}
