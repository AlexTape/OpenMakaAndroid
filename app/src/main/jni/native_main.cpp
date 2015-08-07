#include <jni.h>
#include <string.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "native_main.h"
#include "native_logger.h"

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1start
  (JNIEnv *env, jclass clazz)
{
//    Controller::getInstance()->start();
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1initialize
  (JNIEnv *env, jclass clazz, jint width, jint height, jbyteArray data, jintArray pixels, jstring configPath)
{
    int i_ = 0;

    const char *strMsgPtr = env->GetStringUTFChars( configPath , 0);
    jbyte * pData = env->GetByteArrayElements(data, 0);
    jint * pPixels = env->GetIntArrayElements(pixels, 0);

    cv::Mat mRgba(height, width, CV_8UC1, (unsigned char*) pData);
    cv::Mat mGray(height, width, CV_8UC4, (unsigned char*) pPixels);

    cvtColor(mRgba, mGray, CV_GRAY2RGBA);

    if (strMsgPtr != NULL) {
        std::string pathString(strMsgPtr);
//        i_ = Controller::getInstance()->initialize(mGray, pathString);
    }

    env->ReleaseStringChars(configPath, (jchar *)strMsgPtr);
    env->ReleaseByteArrayElements(data, pData, 0);
    env->ReleaseIntArrayElements(pixels, pPixels, 0);
    return i_;
}

JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeController_native_1displayFunction
  (JNIEnv *env, jclass clazz, jint width, jint height, jbyteArray data, jintArray pixels)
{
    int i_ = 0;

    jbyte * pData = env->GetByteArrayElements(data, 0);
    jint * pPixels = env->GetIntArrayElements(pixels, 0);

    cv::Mat mRgba(height, width, CV_8UC1, (unsigned char*) pData);
    cv::Mat mGray(height, width, CV_8UC4, (unsigned char*) pPixels);

    cvtColor(mRgba, mGray, CV_GRAY2RGBA);

//    i_ = Controller::getInstance()->displayFunction(mRgba, mGray);

    env->ReleaseByteArrayElements(data, pData, 0);
    env->ReleaseIntArrayElements(pixels, pPixels, 0);

	return i_;
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1glRender
  (JNIEnv *env, jclass clazz)
{
//    Controller::getInstance()->glRender();
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1glResize
  (JNIEnv *env, jclass clazz, jint width, jint height)
{
//    Controller::getInstance()->glResize(width, height);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1touchEvent
  (JNIEnv *env, jclass, jfloat x, jfloat y, jint action)
{
//    Controller::getInstance()->touchEvent(x, y, action);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1keyEvent
  (JNIEnv *env, jclass, jint keyCode, jint event)
{
//    Controller::getInstance()->keyEvent(keyCode, event);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1setObjectDetection
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
//    Controller::getInstance()->setObjectDetection(isActive);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1setTracking
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
//    Controller::getInstance()->setTracking(isActive);
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeController_native_1setOpenGL
  (JNIEnv *env, jclass clazz, jboolean isActive)
{
//    Controller::getInstance()->setOpenGL(isActive);
}
