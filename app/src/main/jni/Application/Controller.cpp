#include <stdlib.h>
#include <stdio.h>
#include <vector>

#ifdef __ANDROID__
#include <android/log.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "Controller.hpp"

#include "Features/Analyzer.hpp"
#include "../Legacy/HelperFunctions.h"
#include "../native_logger.h"

using namespace std;

#define SIZE 1024

Controller *Controller::inst_ = NULL;

Controller::Controller(void) {
    isObjectDetection = false;
    isTracking = false;
    isOpenGL = false;
    featureFinished = 1.0;
    queryScale = 1;
    seq_id = 0;
    recognizedObjectId = 0;
    wait_seq_id = 0;
    max_query_size = 320;
    trckOBJ = 0;
    lastRelease = 0;
}

Controller::~Controller(void) {
}

Controller *Controller::getInstance() {
    if (inst_ == NULL) {
        inst_ = new Controller();
    }
    return inst_;
}

int Controller::findFeatures(cv::Mat mRgbaFrame, cv::Mat mGrayFrame) {
    int returnThis = 0;

    Analyzer *analyzer = Analyzer::getInstance();

    analyzer->setDetector("");
    analyzer->setExtractor("");

    if (analyzer->initialized(mGrayFrame)) {

        analyzer->compute(mRgbaFrame, mGrayFrame);
    }


    return returnThis;
}

void Controller::start() {
    log("start");
}

int Controller::initialize(cv::Mat &mGrayFrame, std::string configPath) {
    int isInitialized = 0;

    trckOBJ = new om::track::TrackerKLT();

    cv::FileStorage cvfs(configPath, cv::FileStorage::READ);
    cv::Size frame_size = cv::Size(mGrayFrame.cols, mGrayFrame.rows);
    cv::FileNode fn;

    // reading of visual word
    fn = cvfs["VisualWord"];
    std::string vwfile;
    fn["visualWord"] >> vwfile;
    //log("visualWord = %s",vwfile.c_str());

    std::string idxfile;
    fn["index"] >> idxfile;
    if (idxfile.empty()) {
        ctrlOR.loadVisualWords(vwfile);
    } else {
        ctrlOR.loadVisualWordsBinary(vwfile, idxfile);
    }

    // Reading of the object DB
    ctrlOR.loadObjectDB(cvfs["ObjectDB"]);

    // Reading of the maximum image size for image recognition query
    int max_query_size = 320;
    cvfs["max_query_size"] >> max_query_size;

    // Area secured by reducing the image size for the query to the appropriate size
    int frame_max_size;
    if (frame_size.width > frame_size.height) {
        frame_max_size = frame_size.width;
    } else {
        frame_max_size = frame_size.height;
    }

    while ((frame_max_size / queryScale) > max_query_size) {
        queryScale *= 2;
    }
    query_image.create(frame_size.height / queryScale,
                       frame_size.width / queryScale, CV_8UC1);

    isInitialized = 1;

    return isInitialized;

}

clock_t lastRelease;

int Controller::displayFunction(cv::Mat &mRgbaFrame, cv::Mat &mGrayFrame) {

    int i = 0;
    if (isObjectDetection) {
        if (!isTracking) {


            const clock_t timestamp = clock();
            printf("BEGIN: %d\n", timestamp);

            // essential to do this
            cv::resize(mGrayFrame, query_image, query_image.size());

            recog_result = ctrlOR.queryImage(query_image);
            if (!recog_result.empty()) {
                printf("Recognized id=%d,probility=%f,matchnum=%d, size=%d\n",
                       recog_result[0].img_id, recog_result[0].probability,
                       recog_result[0].matched_num,
                       recog_result[0].object_position.size());

                std::vector<cv::Point2f> objPositionUnscaled = recog_result[0]
                        .object_position;
                std::vector<cv::Point2f> objPosition = om::scalePoints(
                        objPositionUnscaled, (double) queryScale);

                trckOBJ->startTracking(mGrayFrame, objPosition);
                isTracking = true;
                seq_id = 0;
                wait_seq_id = 0;

                cv::line(mRgbaFrame, objPosition.at((int) 0),
                         objPosition.at((int) 1), cv::Scalar(255), 3, 8, 0);
                cv::line(mRgbaFrame, objPosition.at((int) 1),
                         objPosition.at((int) 2), cv::Scalar(255), 3, 8, 0);
                cv::line(mRgbaFrame, objPosition.at((int) 2),
                         objPosition.at((int) 3), cv::Scalar(255), 3, 8, 0);
                cv::line(mRgbaFrame, objPosition.at((int) 3),
                         objPosition.at((int) 0), cv::Scalar(255), 3, 8, 0);

                recognizedObjectId = recog_result[0].img_id;

                lastRelease = clock();
                double runtime = float(lastRelease - timestamp) / CLOCKS_PER_SEC;

                printf("DESTINATION: %d\n", runtime);
                isObjectDetection = false;
            }


// if opengl is off, show feature circles
//            if (!isOpenGL) {
//                std::vector<std::KeyPoint> v;
//
//                FastFeatureDetector detector(50);
//                detector.detect(mGrayFrame, v);
//
//                for( unsigned int i = 0; i < v.size(); i++ )
//                {
//                    const KeyPoint& kp = v[i];
//                    circle(mRgbaFrame, Point(kp.pt.x, kp.pt.y), 10, Scalar(255,0,0,255));
//                }
//            }
//    if (isObjectDetection) {
//        //int thisTime = Helper::now_ms();
//        int recognizedObjectId = findFeatures(mRgbaFrame, mGrayFrame);
//        //featureFinished = Helper::now_ms();
//        //int featureRuntime = featureFinished - thisTime;
//        //log(DTAG, "FIND FEATURE RUNTIME: %d ms Detected Feature: %d", featureRuntime, recognizedObjectId);
//    }
//        log(ControllerTAG, "display.. done");

        } else {

        isTracking = trckOBJ->onTracking(mGrayFrame);

        // Draw Result
        std::vector<cv::Point2f> points = trckOBJ->object_position;

        cv::line(mRgbaFrame, trckOBJ->object_position[0],
                 trckOBJ->object_position[1], cv::Scalar(255), 3, 8, 0);
        cv::line(mRgbaFrame, trckOBJ->object_position[1],
                 trckOBJ->object_position[2], cv::Scalar(255), 3, 8, 0);
        cv::line(mRgbaFrame, trckOBJ->object_position[2],
                 trckOBJ->object_position[3], cv::Scalar(255), 3, 8, 0);
        cv::line(mRgbaFrame, trckOBJ->object_position[3],
                 trckOBJ->object_position[0], cv::Scalar(255), 3, 8, 0);

        seq_id++;

        }
    }

    return i;
}

void Controller::glRender() {
    log("rendering..");
    log("rendering.. done");
}

void Controller::glResize(int height, int width) {
    log("resizing..");
    log("resizing.. done");
}

void Controller::setObjectDetection(bool isActive) {
    log("setObjectDetection: %b", isActive);
    isObjectDetection = isActive;
}

void Controller::setTracking(bool isActive) {
    log("setTracking: %b", isActive);
    isTracking = isActive;
}

void Controller::setOpenGL(bool isActive) {
    log("setOpenGL: %b", isActive);
    isOpenGL = isActive;
}
