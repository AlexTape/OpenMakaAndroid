#include <jni.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>

#include <android/native_window.h> // requires ndk r5 or newer

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <GLES/glext.h>

#include "de_alextape_openmaka_NativeFunctions.h"
#include "native_logger.h"

using namespace std;

bool isObjectDetection = false;
bool isTracking = false;
bool isOpenGL = false;


GLuint Name;
GLubyte* ImagePtr;
#define SIZE 1024

double featureFinished = 0;

string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}



int findFeatures(cv::Mat mRgbaFrame, cv::Mat mGrayFrame)
{
    int returnThis = 0;

    vector<cv::KeyPoint> v;

    cv::FastFeatureDetector detector(50);
    //cv::ORB detector(50);
    detector.detect(mGrayFrame, v);
    for( unsigned int i = 0; i < v.size(); i++ )
    {
        const cv::KeyPoint& kp = v[i];
        cv::circle(mRgbaFrame, cv::Point(kp.pt.x, kp.pt.y), 10, cv::Scalar(255,0,0,255));
    }

//    cv::Ptr<cv::FeatureDetector> detector = cv::FeatureDetector::create("SURF");
//    detector->detect(mGrayFrame, v);

    // if opengl is off, show feature circles
//    if (!isOpenGL) {
//        for( unsigned int i = 0; i < v.size(); i++ )
//        {
//            const cv::KeyPoint& kp = v[i];
//            cv::circle(mRgbaFrame, cv::Point(kp.pt.x, kp.pt.y), 10, cv::Scalar(255,0,0,255));
//        }
//    }

//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return returnThis;
}


/* return current time in milliseconds */
static double now_ms(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1start
  (JNIEnv *, jclass clazz)
{
	log_info("","","native_Start","");

    // acquisition of viewModel
    //viewMDL = cvar::overlay::viewModel::getInstance();

	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	//glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations


	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &Name);
	glBindTexture(GL_TEXTURE_2D, Name);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	ImagePtr = new GLubyte[SIZE*SIZE*4];
	for(int i =0;i < SIZE*SIZE; i++)
	{
		ImagePtr[i*4+0] = 100;
		ImagePtr[i*4+1] =0;
		ImagePtr[i*4+2] = 0;
		ImagePtr[i*4+3] = 128;

	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SIZE, SIZE,
		0, GL_RGBA, GL_UNSIGNED_BYTE, ImagePtr);
}

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_initialize
 * Signature: (JLjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1initialize
  (JNIEnv *env, jclass clazz, jlong mAddrGray, jstring configPath)
{
    log_info("","","NATIVE INITIALIZATION..");
    bool isInitialized = false;

    const char *strMsgPtr = env->GetStringUTFChars( configPath , 0);


    // do initialization


    env->ReleaseStringChars(configPath, (jchar *)strMsgPtr);
    isInitialized = true;

    log_info("","","NATIVE INITIALIZATION.. DONE!","");
    return isInitialized;

}

/*
 * Class:     de_alextape_openmaka_NativeFunctions
 * Method:    native_displayFunction
 * Signature: (JJ)I
 */
JNIEXPORT jint JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1displayFunction
  (JNIEnv *env, jclass clazz, jlong mRgbaAddr, jlong mGrayAddr)
{
    int i =0;

	cv::Mat& mRgbaFrame  = *(cv::Mat*)mRgbaAddr;
	cv::Mat& mGrayFrame  = *(cv::Mat*)mGrayAddr;

    if (isObjectDetection) {
        int thisTime = now_ms();
        int recognizedObjectId = findFeatures(mRgbaFrame, mGrayFrame);
        featureFinished = now_ms();
        int featureRuntime = featureFinished - thisTime;
        log_info("","","FIND FEATURE RUNTIME: %d ms | Detected Feature: %d", featureRuntime, recognizedObjectId);
    }

	return i;

}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1glRender
  (JNIEnv *env, jclass clazz)
{
    if (isOpenGL) {
       int recognizedObjectId = 1;
       if (recognizedObjectId > 0) {

           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
           glLoadIdentity();									// Reset The Current Modelview cv::Matrix
           glScalef(2,2,2);
           glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
           glClearColor(0,0,0,0);


           //glDeleteTextures(1,&Name);
           glEnable(GL_TEXTURE_2D);
           glBindTexture(GL_TEXTURE_2D, Name);
           glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
           glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
           glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
           glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


           GLfloat vertex[3*2*10];
           int vertexIndex = 0;
           vertex[vertexIndex++] =-30.0;
           vertex[vertexIndex++] =-10.0;
           vertex[vertexIndex++] = 5;
           vertex[vertexIndex++] =-20.0;
           vertex[vertexIndex++] =10.0;
           vertex[vertexIndex++] =5;
           vertex[vertexIndex++] =0.0;
           vertex[vertexIndex++] =10.0;
           vertex[vertexIndex++] = 5;
           vertex[vertexIndex++] =0.0;
           vertex[vertexIndex++] =10.0;
           vertex[vertexIndex++] = 5;
           vertex[vertexIndex++] =0.0;
           vertex[vertexIndex++] =-10.0;
           vertex[vertexIndex++] = 5;
           vertex[vertexIndex++] =-20.0;
           vertex[vertexIndex++] =-10.0;
           vertex[vertexIndex++] = 5;
           glColor4f(1,1,1,1);
           glEnableClientState(GL_VERTEX_ARRAY);
           glVertexPointer(3,GL_FLOAT,0,vertex);
           glDrawArrays(GL_TRIANGLE_FAN,0,6);
           glDisableClientState(GL_VERTEX_ARRAY);
           glFlush();

       } else {

           glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffe
           glClearColor(0,0,0,0);
       }
   }

}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1glResize
  (JNIEnv *env, jclass clazz, jint width, jint height)
{
	log_info("","","native_resize ");
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection cv::Matrix
	glLoadIdentity();									// Reset The Projection cv::Matrix

	// Calculate The Aspect Ratio Of The Window
	glOrthof(width/4*-1, width/4, height/4*-1, height/4, 0, 1000);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview cv::Matrix
	glLoadIdentity();									// Reset The Modelview cv::Matrix

}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setObjectDetection
  (JNIEnv *env, jclass clazz, jboolean isActive) {
    isObjectDetection = isActive;
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setTracking
  (JNIEnv *env, jclass clazz, jboolean isActive) {
    isTracking = isActive;
}

JNIEXPORT void JNICALL Java_de_alextape_openmaka_NativeFunctions_native_1setOpenGL
  (JNIEnv *env, jclass clazz, jboolean isActive) {
    isOpenGL = isActive;
}
