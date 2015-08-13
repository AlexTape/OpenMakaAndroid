#include <jni.h>
#include <string.h>
#include <opencv2/core/core.hpp>

#include "de_alextape_openmaka_NativeController.h"

#include "Application/Controller.h"

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1initialize
  (JNIEnv *env, jclass clazz, jlong mAddrGray, jstring configPath)
{
    int i_ = 0;
    const char *strMsgPtr = env->GetStringUTFChars( configPath , 0);
    if (strMsgPtr != NULL) {
        std::string pathString(strMsgPtr);
        i_ = om::Controller::getInstance()->initialize(*(cv::Mat*)mAddrGray, pathString);
    }
    env->ReleaseStringChars(configPath, (jchar *)strMsgPtr);
    return i_;
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1displayFunction
  (JNIEnv *env, jclass clazz, jlong mRgbaAddr, jlong mGrayAddr)
{
	return om::Controller::getInstance()->displayFunction(*(cv::Mat*)mRgbaAddr, *(cv::Mat*)mGrayAddr);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1glRender
  (JNIEnv *env, jclass clazz)
{
    om::Controller::getInstance()->glRender();
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1glResize
  (JNIEnv *env, jclass clazz, jint width, jint height)
{
    om::Controller::getInstance()->glResize(width, height);
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1setDetector
        (JNIEnv *env, jclass, jstring type)
{
    return om::Controller::setDetector(type);
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1setExtractor
        (JNIEnv *env, jclass, jstring type)
{
    return om::Controller::setExtractor(type);
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1setMatcher
        (JNIEnv *env, jclass, jstring type)
{
    return om::Controller::setMatcher(type);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1setModeObjectDetection
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
    om::Controller::getInstance()->isModeObjectDetection(isActive);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1setModeTracking
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
    om::Controller::getInstance()->isModeTracking(isActive);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1setModeOpenGL
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
    om::Controller::getInstance()->isModeOpenGL(isActive);
}
