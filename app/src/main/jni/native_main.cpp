#include <jni.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>

#include <android/log.h>
#include <android/native_window.h> // requires ndk r5 or newer

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <libpng/png.h>

#include "android_key_code.h"
#include "native_interface.h"
#include "Main/commonCvFunctions.h"
#include "ObjRecog/controlOR.h"
#include "ObjRecog/imageDB.h"

#include "Main/utilFunctions.h"

#include "Tracking/kltTrackingOBJ.h"
#include "Tracking/trackingOBJ.h"

using namespace std;

#define LOG_TAG "NATIVE_MAIN"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

GLuint Name;
GLubyte* ImagePtr;
#define SIZE 1024
bool textured;

cvar::tracking::trackingOBJ* trckOBJ = 0;	// Object tracking class
//cvar::overlay::viewModel *viewMDL;	// OpenGL image display class (singleton)

bool isObjectDetection = false;
bool isTracking = false;
bool isOpenGL = false;

int seq_id = 0;	// Sequence ID of tracking
int wait_seq_id = 0; // Sequence ID at the time of non-tracking
int query_scale = 1.0;	// Query image reduction scale
bool track_f = false;	// Tracking flag

int recognizedObjectId = 0;

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

void native_gl_resize(JNIEnv *env UNUSED, jclass clazz UNUSED, jint width, jint height)
{
	LOGI("native_resize ");
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	glOrthof(width/4*-1, width/4, height/4*-1, height/4, 0, 1000);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

}

void native_gl_render(JNIEnv *env UNUSED, jclass clazz UNUSED)
{
    if (recognizedObjectId > 0) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
        glLoadIdentity();									// Reset The Current Modelview Matrix
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

void native_start(JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	LOGI("native_start ");

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

	textured = true;

}

void native_key_event(JNIEnv *env,jclass clazz,jint key,jint status)
{
	LOGI("native_key_event  key:%d action:%d",key,status);
	if(status == 0)
	switch(key)
	{
	case kKeyCodeBack:
		exit(0);
		break;
	case kKeyCode0:
	case kKeyCode1:
	case kKeyCode2:
	case kKeyCode3:
	case kKeyCode4:
		break;
	case kKeyCodeSpace:
		break;
	case kKeyCodeR:
		break;
	case 82:
		if(textured)
		{
			LOGI("Release Texture %d!!!",Name);
			glDeleteTextures(1, &Name);
			textured = false;
		}else
		{

			glGenTextures(1, &Name);
			LOGI("Create Texture %d!!!",Name);
			glBindTexture(GL_TEXTURE_2D, Name);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SIZE, SIZE,
			0, GL_RGBA, GL_UNSIGNED_BYTE, ImagePtr);
			textured = true;

		}
		break;

	case kKeyCodeDpadLeft:
		glGenTextures(1, &Name);
		glBindTexture(GL_TEXTURE_2D, Name);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SIZE, SIZE,
			0, GL_RGBA, GL_UNSIGNED_BYTE, ImagePtr);
		break;
	case kKeyCodeDpadRight:
		glDeleteTextures(1, &Name);
		break;
	default:
		break;
	}

}

cvar::orns::controlOR	ctrlOR;
cv::Mat query_image;

int findFeatures(cv::Mat addrGray, cv::Mat addrRgba)
{

	cv::Mat& mGray  = addrGray;
    cv::Mat& mRgba  = addrRgba;

	int returnThis = 0;

    if (!track_f) {
        try {

        // essential to do this
        cv::resize(mGray, query_image, query_image.size());

        vector<cvar::orns::resultInfo> recog_result = ctrlOR.queryImage(query_image);
        if(!recog_result.empty()){
            LOGI("Recognized id=%d,probility=%f,matchnum=%d, size=%d",
                recog_result[0].img_id,
                recog_result[0].probability,
                recog_result[0].matched_num,
                recog_result[0].object_position.size());
            returnThis = recog_result[0].img_id;
        }

	    // Convert homography for reduced image for the camera image
	    // CV_8UC1 to CV_32FC1
	    LOGI("QUERY_SCALE: %d",query_scale);
//                    cv::Mat pose_mat_scale = recog_result[0].pose_mat;
//                    recog_result[0].pose_mat.row(0) *= query_scale;
//                    recog_result[0].pose_mat.row(1) *= query_scale;
//
//                    const cv::Mat& test = mGray;

//        std::vector<cv::Point2f> objPos = recog_result[0].object_position;
//
//        std::vector<cv::Point2f> test1 = cvar::scalePoints(
//                objPos,
//                (double) query_scale);

        //trckOBJ->startTracking(test, test1);
        //track_f = viewMDL->setRecogId(recog_result[0].img_id,
        //        pose_mat_scale);

        seq_id = 0;
        wait_seq_id = 0;

        // Draw Result
        // drawLineContour(mRgba, trckOBJ.object_position, cv::Scalar(255));
        // vector<Point2f>::iterator itr = trckOBJ.corners.begin();
        // while(itr!=trckOBJ.corners.end()){
        //     circle(frame, *itr, 3, Scalar(255,0,0));
        //     itr++;
        // }
        } catch (exception& e) {
                    LOGI("BUG TRACKING 1");
        }

    } else {

        //track_f = trckOBJ->onTracking(mGray);
        seq_id++;
    }

    if(track_f) {

//        const std::vector<cv::Point2f> objPosition = trckOBJ->getObjectPosition();
//        const std::vector<cv::Point2f> objCorners = trckOBJ->getCorners();
//        const std::vector<unsigned char> objTrackStatus = trckOBJ->getTrackStatus();
//
//        cvar::drawLineContour(mRgba, objPosition, cv::Scalar(255));
//        cvar::drawPoints(mRgba, objCorners, objTrackStatus, cv::Scalar(255));
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //viewMDL->drawScene(mRgba);

	return returnThis;
}


void native_touch_event(JNIEnv *env,jclass clazz,jfloat x,jfloat y,jint status)
{
    //LOGI("touch_event: %d %d %d", x,y,status);
}

double featureFinished = 0;

/* return current time in milliseconds */
static double now_ms(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
}

int native_displayFunction(JNIEnv *env,jclass clazz,jlong mRgbaAddr, jlong mGrayAddr) {

    int i =0;

	cv::Mat& mRgbaFrame  = *(cv::Mat*)mRgbaAddr;
	cv::Mat& mGrayFrame  = *(cv::Mat*)mGrayAddr;

    double thisTime = 0;
    thisTime = now_ms();

    // TODO reactivate obj detection switch for feature detection
    //if (isObjectDetection) {
    if (true) {
        // TODO manipulate findFeature Period
        double period = 10000;
        double deltaTime = thisTime - featureFinished;
        if (deltaTime >= period) {
            recognizedObjectId = findFeatures(mRgbaFrame, mGrayFrame);
            featureFinished = now_ms();
            double featureRuntime = featureFinished - thisTime;
            LOGI("FIND FEATURE RUNTIME: %d ms", featureRuntime);
        }

        // if opengl is off, show feature circles
        if (!isOpenGL) {
            vector<cv::KeyPoint> v;

            cv::FastFeatureDetector detector(50);
            detector.detect(mGrayFrame, v);

            for( unsigned int i = 0; i < v.size(); i++ )
            {
                const cv::KeyPoint& kp = v[i];
                cv::circle(mRgbaFrame, cv::Point(kp.pt.x, kp.pt.y), 10, cv::Scalar(255,0,0,255));
            }
        }
    }

	return i;
}

void native_initTracking()
{
//    try {
//        cvar::tracking::trackingOBJ::TRACKER_TYPE trackingType = cvar::tracking::trackingOBJ::TRACKER_KLT;
//        trckOBJ = cvar::tracking::trackingOBJ::create(trackingType);
//    } catch (std::exception &e) {
//        LOGI("initTracking Exception");
//        throw e;
//    }
//
//    track_f = true;
//    double d2[] = {1,0,0,0,1,0,0,0,1};
//    cv::Mat diagMat = cv::Mat(3,3,CV_64FC1,d2).clone();
//    trckOBJ->setHomographyMat(diagMat);
    //viewMDL->setRecogId(4,diagMat);
}



bool native_initialize(JNIEnv *env,jclass clazz,jlong addrFrame, jstring configPath) {

    LOGI("NATIVE INITIALIZATION..");
    bool isInitialized = false;

    const char *strMsgPtr = env->GetStringUTFChars( configPath , 0);
    //env->ReleaseStringChars(configPath, (jchar *)strMsgPtr);

    cv::Mat& frame  = *(cv::Mat*)addrFrame;
    cv::Size frame_size = cv::Size(frame.cols, frame.rows);

    cv::FileStorage cvfs(strMsgPtr, cv::FileStorage::READ);

    // reading of visual word
    cv::FileNode fn;
    fn = cvfs["VisualWord"];
    std::string vwfile;
    fn["visualWord"] >> vwfile;
    LOGI("visualWord = %s",vwfile.c_str());

    std::string idxfile;
    fn["index"] >> idxfile;
    if(idxfile.empty()){
        ctrlOR.loadVisualWords(vwfile);
    } else{
        ctrlOR.loadVisualWordsBinary(vwfile, idxfile);
    }

    // Reading of the object DB
    ctrlOR.loadObjectDB(cvfs["ObjectDB"]);

    // Reading of the maximum image size for image recognition query
    int max_query_size = 320;
    cvfs["max_query_size"] >> max_query_size;

    // Area secured by reducing the image size for the query to the appropriate size
    int frame_max_size;
    if(frame_size.width > frame_size.height){
        frame_max_size = frame_size.width;
    } else{
        frame_max_size = frame_size.height;
    }

    query_scale = 1;
    while((frame_max_size / query_scale) > max_query_size){
        query_scale*=2;
    }
    query_image.create(frame_size.height/query_scale, frame_size.width/query_scale, CV_8UC1);

    LOGI("frame_max_size = %d",frame_max_size);         //800
    LOGI("frame_size.height = %d",frame_size.height);   //480
    LOGI("frame_size.width = %d",frame_size.width);     //800
    LOGI("query_scale = %d",query_scale);               //4

    native_initTracking();

    LOGI("NATIVE INITIALIZATION.. DONE!");
    return !isInitialized;

}

void native_setObjectDetection(JNIEnv *env,jclass clazz,jboolean isActive) {
    isObjectDetection = isActive;
}

void native_setTracking(JNIEnv *env,jclass clazz,jboolean isActive) {
    isTracking = isActive;
}

void native_setOpenGL(JNIEnv *env,jclass clazz,jboolean isActive) {
    isOpenGL = isActive;
}
