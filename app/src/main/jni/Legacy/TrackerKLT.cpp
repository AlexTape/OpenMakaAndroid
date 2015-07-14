#include "TrackerKLT.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/video/tracking.hpp>
#include <stdio.h>

#include "HelperFunctions.h"

using namespace std;
using namespace cv;
using namespace om;
using namespace om::track;

TrackerKLT::TrackerKLT(void) {
    max_corners = 80;
    quality_level = 0.1;
    min_distance = 5;
}

TrackerKLT::~TrackerKLT(void) {
}

void TrackerKLT::startTracking(const Mat &grayImg, vector<Point2f> &pts) {
    Mat mask = createMask(grayImg.size(), pts);
    goodFeaturesToTrack(grayImg, corners, max_corners, quality_level,
                        min_distance, mask);
    grayImg.copyTo(prevImg);
    object_position = pts;
    float d[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};
    homographyMat = Mat(3, 3, CV_32FC1, d).clone();
    track_status.clear();
}

bool TrackerKLT::onTracking(const Mat &grayImg) {
    std::vector<cv::Point2f> next_corners;
    std::vector<float> err;

    if (corners.size() == 0) {
        unsigned int cornerCount = corners.size();
        printf("ABORT: %d\n", cornerCount);
        return false;
    }

    calcOpticalFlowPyrLK(prevImg, grayImg, corners, next_corners, track_status,
                         err);

    int tr_num = 0;
    vector<unsigned char>::iterator status_itr = track_status.begin();
    while (status_itr != track_status.end()) {
        if (*status_itr > 0) tr_num++;
        status_itr++;
    }
    if (tr_num < 6) {
        return false;
    } else {
        homographyMat = findHomography(Mat(corners), Mat(next_corners),
                                       track_status, CV_RANSAC, 5);

        if (countNonZero(homographyMat) == 0) {
            return false;
        } else {
            vector<Point2f> next_object_position = calcAffineTransformPoints(
                    object_position, homographyMat);
            if (!checkPtInsideImage(prevImg.size(), next_object_position)) {
                return false;
            }
            if (!checkRectShape(next_object_position)) {
                return false;
            }
            int ret = checkInsideArea(next_corners, next_object_position,
                                      track_status);
            if (ret < 6) {
                return false;
            }
            grayImg.copyTo(prevImg);
            corners = next_corners;
            object_position = next_object_position;
        }
    }
    return true;
}
