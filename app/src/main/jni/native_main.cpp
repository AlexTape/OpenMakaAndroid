#include <jni.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <android/log.h>
#include <android_key_code.h>

#include "native_interface.h"

#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <GLES/glext.h>

#include <libpng/png.h>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include "Main/commonCvFunctions.h"
#include "ObjRecog/controlOR.h"
#include "ObjRecog/imageDB.h"
#include "Tracking/trackingOBJ.h"

using namespace std;
using namespace cv;

using namespace cvar;
using namespace cvar::orns;
using namespace cvar::tracking;

#define LOG_TAG "NATIVE_MAIN"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)

GLuint Name;
GLubyte* ImagePtr;
#define SIZE 1024
bool textured;


int seq_id = 0;	// Sequence ID of tracking
int wait_seq_id = 0; // Sequence ID at the time of non-tracking
trackingOBJ* trckOBJ = 0;	// Object tracking class
bool track_f = false;	// Tracking flag
int query_scale = 1;	// Query image reduction scale
controlOR* ctrlOR = 0;	// Specific object recognition class
int max_query_size = 320;	// Maximum query image size

Mat query_image;	// Reduced image size for image recognition query


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

        Mat frame;
        if (capture.isOpened()) { // When the camera is present
            // TODO capture here
            capture.read(frame);

        } else { // When the camera does not exist
            // No particularly to do
            LOGI("ERROR 2: camera not exist");
        }

     Mat grayImg;


        cvtColor(frame, grayImg, CV_BGR2GRAY);


                if (!track_f) {
                    try {
                        cv::resize(grayImg, query_image, query_image.size());
                        vector<resultInfo> recog_result = ctrlOR->queryImage(
                                query_image);	// Recognized by the reduced image
        //			vector<resultInfo> recog_result = ctrlOR->queryImage(grayImg);	// Recognized in the image from the camera
                        if (!recog_result.empty()) {
                            LOGI("img id: ", recog_result[0].img_id);

                            // Convert homography for reduced image for the camera image
                            Mat pose_mat_scale = recog_result[0].pose_mat.clone();
                            pose_mat_scale.row(0) *= query_scale;
                            pose_mat_scale.row(1) *= query_scale;

                            const Mat& test = grayImg;

                            vector<Point2f> test1 = scalePoints(
                                    recog_result[0].object_position,
                                    (double) query_scale);

                            trckOBJ->startTracking(test, test1);
                            //track_f = viewMDL->setRecogId(recog_result[0].img_id,
                            //        pose_mat_scale);

        //				trckOBJ.startTracking(grayImg, recog_result[0].object_position);
        //				viewMDL->setRecogId(recog_result[0].img_id, recog_result[0].pose_mat);
        //				track_f = true;
                            seq_id = 0;
                            wait_seq_id = 0;

                            // Draw Result
        //				drawLineContour(frame, trckOBJ.object_position, Scalar(255));
        //				vector<Point2f>::iterator itr = trckOBJ.corners.begin();
        //				while(itr!=trckOBJ.corners.end()){
        //					circle(frame, *itr, 3, Scalar(255,0,0));
        //					itr++;
        //				}
                        }
                    } catch (exception& e) {
                        LOGI("ERROR 3");
                    }
                } else {
                    track_f = trckOBJ->onTracking(grayImg);
                    seq_id++;
                }

	//LOGI("native_render ");
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
 	vertex[vertexIndex++] =-20.0;
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
	return;

}

void native_start(JNIEnv *env UNUSED, jclass clazz UNUSED)
{
	LOGI("native_start ");
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

void native_touch_event(JNIEnv *env,jclass clazz,jfloat x,jfloat y,jint status)
{
}


bool init = false;

int native_FindFeatures(JNIEnv *env,jclass clazz,jlong addrGray, jlong addrRgba, jstring configPath)
{

	//LOGI("native_FindFeatures  gray:%ld rgba:%ld",addrGray,addrRgba,configPath);

	Mat& frame  = *(Mat*)addrGray;

	const char *strMsgPtr = env->GetStringUTFChars( configPath , 0);
	//env->ReleaseStringChars(configPath, (jchar *)strMsgPtr);

	try {
		trckOBJ = trackingOBJ::create(trackingOBJ::TRACKER_KLT);

	} catch (std::exception& e) {
		LOGI("ERROR 1");
		throw e;
	}

	if(!init){
		Size frame_size = Size(frame.cols, frame.rows);

		cv::FileStorage cvfs(strMsgPtr, cv::FileStorage::READ);


		printf(strMsgPtr);

		// reading of visual word
		FileNode fn;
		fn = cvfs["VisualWord"];
		std::string vwfile;
		fn["visualWord"] >> vwfile;
		LOGI("visualWord = %s",vwfile.c_str());

		std::string idxfile;
		fn["index"] >> idxfile;
		if(idxfile.empty()){
			ctrlOR->loadVisualWords(vwfile);
		}
		else{
			ctrlOR->loadVisualWordsBinary(vwfile, idxfile);
		}

		// Reading of the object DB
		ctrlOR->loadObjectDB(cvfs["ObjectDB"]);

		// Reading of the maximum image size for image recognition query
		cvfs["max_query_size"] >> max_query_size;

		// Area secured by reducing the image size for the query to the appropriate size
		int frame_max_size;
		if(frame_size.width > frame_size.height){
			frame_max_size = frame_size.width;
		}
		else{
			frame_max_size = frame_size.height;
		}

		int query_scale=1;
		while((frame_max_size / query_scale) > max_query_size){
			query_scale*=2;
		}
		query_image.create(frame_size.height/query_scale, frame_size.width/query_scale, CV_8UC1);

		init = true;

		LOGI("frame_max_size = %d",frame_max_size);   //800
		LOGI("frame_size.height = %d",frame_size.height);   //480
		LOGI("frame_size.width = %d",frame_size.width);      //800
		LOGI("query_scale = %d",query_scale);                //4

		track_f = true;
		double d2[] = {1,0,0,0,1,0,0,0,1};
		Mat diagMat = Mat(3,3,CV_64FC1,d2).clone();
		trckOBJ->setHomographyMat(diagMat);

	}

	cv::resize(frame, query_image, query_image.size());

	vector<cvar::orns::resultInfo> recog_result = ctrlOR->queryImage(query_image);
	if(!recog_result.empty()){
		LOGI("Recognized id=%d,probility=%f,matchnum=%d, size=%d",
			recog_result[0].img_id,
			recog_result[0].probability,
			recog_result[0].matched_num,
			recog_result[0].object_position.size());
		return recog_result[0].img_id;
	}
	return 0;

	/*Mat& mGr  = *(Mat*)addrGray;
    Mat& mRgb = *(Mat*)addrRgba;
    vector<KeyPoint> v;

    FastFeatureDetector detector(50);
    detector.detect(mGr, v);
    for( unsigned int i = 0; i < v.size(); i++ )
    {
        const KeyPoint& kp = v[i];
        circle(mRgb, Point(kp.pt.x, kp.pt.y), 10, Scalar(255,0,0,255));
    }*/

}
