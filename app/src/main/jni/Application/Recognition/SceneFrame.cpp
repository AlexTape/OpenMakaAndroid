#ifndef OPENMAKAENGINE_SCENEFRAME_CPP
#define OPENMAKAENGINE_SCENEFRAME_CPP

#include "SceneFrame.h"
#include "../Controller.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <iostream>

using namespace std;
using namespace om;

SceneFrame::SceneFrame(cv::Mat &rgbInputFrame, cv::Mat &grayInputFrame, int thisScale) {

    if (Controller::MODE_DEBUG) {
        cout << "Creating SceneFrame instance.." << endl;
    }

    rgb = rgbInputFrame;
    gray = grayInputFrame;
    scale = thisScale;
    objectPosition = vector<cv::Point2f>(4);

}

SceneFrame::~SceneFrame(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting SceneFrame instance.." << endl;
    }
}

#endif
