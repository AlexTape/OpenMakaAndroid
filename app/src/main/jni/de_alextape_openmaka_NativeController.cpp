#include <jni.h>
#include <string.h>
#include <opencv2/core/core.hpp>

#include "de_alextape_openmaka_NativeController.h"
#include "Application/Controller.h"

std::string convertJString(JNIEnv* env, jstring str)
{
    const jsize len = env->GetStringUTFLength(str);
    const char* strChars = env->GetStringUTFChars(str, (jboolean *)0);
    std::string Result(strChars, len);
    env->ReleaseStringUTFChars(str, strChars);
    return Result;
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1initialize
  (JNIEnv *env, jclass clazz, jlong mAddrGray, jstring storagePath)
{
    int i_ = 0;
    std::string pathString = convertJString( env, storagePath );
    i_ = om::Controller::getInstance()->initialize(*(cv::Mat*)mAddrGray, pathString);
    return i_;
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1test
        (JNIEnv *env, jclass clazz, jint test, jint quantifier)
{
    return om::Controller::getInstance()->test(test, quantifier);
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

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1configure
        (JNIEnv *env, jclass clazz, jstring detector, jstring extractor, jstring matcher)
{
    int i_ = 0;
    std::string detectorString = convertJString( env, detector );
    std::string extractorString = convertJString( env, extractor );
    std::string matcherString = convertJString( env, matcher );
    i_ = om::Controller::getInstance()->configure(detectorString, extractorString, matcherString);
    return i_;
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
