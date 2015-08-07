#ifndef OPENMAKAENGINE_TRACKER_CPP
#define OPENMAKAENGINE_TRACKER_CPP

//! Big Thanks to Takuya Minagawa
//! source @https://github.com/takmin/OpenCV-Marker-less-AR
//! forked @https://github.com/AlexTape/OpenCV-Marker-less-AR

#include "Tracker.h"
#include "../Controller.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace std;
using namespace om;

// init static members
Tracker *Tracker::inst_ = NULL;
int Tracker::MAX_CORNERS;
double Tracker::QUALITY_LEVEL;
double Tracker::MINIMUM_DISTANCE;

Tracker *Tracker::getInstance() {
    if (inst_ == NULL) {
        inst_ = new Tracker();
    }
    return inst_;
}

Tracker::Tracker(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Creating Tracker instance.." << endl;
    }
}

Tracker::~Tracker(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Tracker instance.." << endl;
    }
}

void Tracker::initialize(const cv::Mat &frame, vector<cv::Point2f> &actualObjectPosition) {

    cv::Mat mask = createMask(frame.size(), actualObjectPosition);

    goodFeaturesToTrack(frame, corners, MAX_CORNERS, QUALITY_LEVEL,
                        MINIMUM_DISTANCE, mask);

    frame.copyTo(lastImage);

    objectPosition = actualObjectPosition;

    float d[] = {1, 0, 0, 0, 1, 0, 0, 0, 1};

    homography = cv::Mat(3, 3, CV_32FC1, d).clone();

    vstatus.clear();
}

bool Tracker::process(const cv::Mat &sceneImage) {

    std::vector<cv::Point2f> nextCorners;
    std::vector<float> verror;

    if (corners.size() == 0) {
        cout << "Corners size == 0" << endl;
        return false;
    }

    // vstatus gives the total number of found features
    // verror will contian known errors
    cv::calcOpticalFlowPyrLK(lastImage, sceneImage, corners, nextCorners, vstatus,
                             verror);


    int tr_num = 0;
    vector<unsigned char>::iterator status_itr = vstatus.begin();
    while (status_itr != vstatus.end()) {
        if (*status_itr > 0) tr_num++;
        status_itr++;
    }

//    vector<cv::Point2f> rightPoints;
//    vector<int> rightPointsToFindBackIndex;
//
//    for (unsigned int i = 0; i<vstatus.size(); i++) {
//        float test = verror[i];
//        if (vstatus[i] && verror[i] > 0) {
//            // keep original point index in optical flow array for further use
//            rightPointsToFindBackIndex.push_back(i);
//            rightPoints.push_back(nextCorners[i]);
//        } else {
//            vstatus[i] = 0;
//        }
//    }
//
//    int a = rightPoints.size();
//    int b = tr_num;


//    if (rightPoints.size() < 6) {
//        return false;

    if (tr_num < 6) {
        return false;

    } else {

        homography = findHomography(cv::Mat(corners), cv::Mat(nextCorners),
                                    vstatus, CV_RANSAC, 5);

        if (countNonZero(homography) == 0) {

            return false;

        } else {

            vector<cv::Point2f> nextObjectPosition = calcAffineTransformPoints(
                    objectPosition, homography);

            if (!isObjectInsideImage(lastImage.size(), nextObjectPosition)) {
                return false;
            }

            if (!isRectangle(nextObjectPosition)) {
                return false;
            }

            int ret = isInsideArea(nextCorners, nextObjectPosition,
                                   vstatus);
            if (ret < 6) {
                return false;
            }

            sceneImage.copyTo(lastImage);

            corners = nextCorners;

            objectPosition = nextObjectPosition;
        }
    }
    return true;
}

int Tracker::isInsideArea(vector<cv::Point2f> &points, vector<cv::Point2f> &corner_pts,
                          vector<unsigned char> &status) {

    CV_Assert(corner_pts.size() == 4);
    CV_Assert(points.size() == status.size());

    // ax+by+c=0
    float a[4];
    float b[4];
    float c[4];

    a[0] = corner_pts[3].y - corner_pts[0].y;
    a[1] = corner_pts[2].y - corner_pts[1].y;
    a[2] = corner_pts[1].y - corner_pts[0].y;
    a[3] = corner_pts[2].y - corner_pts[3].y;

    b[0] = corner_pts[0].x - corner_pts[3].x;
    b[1] = corner_pts[1].x - corner_pts[2].x;
    b[2] = corner_pts[0].x - corner_pts[1].x;
    b[3] = corner_pts[3].x - corner_pts[2].x;

    c[0] = corner_pts[0].y * corner_pts[3].x
           - corner_pts[3].y * corner_pts[0].x;
    c[1] = corner_pts[1].y * corner_pts[2].x
           - corner_pts[2].y * corner_pts[1].x;
    c[2] = corner_pts[0].y * corner_pts[1].x
           - corner_pts[1].y * corner_pts[0].x;
    c[3] = corner_pts[3].y * corner_pts[2].x
           - corner_pts[2].y * corner_pts[3].x;

    float max_x, min_x, max_y, min_y;
    max_x = corner_pts[0].x;
    min_x = corner_pts[0].x;
    max_y = corner_pts[0].y;
    min_y = corner_pts[0].y;

    int i;
    for (i = 1; i < 4; i++) {
        if (corner_pts[i].x > max_x) max_x = corner_pts[i].x;
        if (corner_pts[i].x < min_x) min_x = corner_pts[i].x;
        if (corner_pts[i].y > max_y) max_y = corner_pts[i].y;
        if (corner_pts[i].y < min_y) min_y = corner_pts[i].y;
    }

    float val[4];
    int size = points.size();
    int count = 0;
    for (int j = 0; j < size; j++) {
        if (status[j] > 0) {
            for (i = 0; i < 4; i++) {
                val[i] = a[i] * points[j].x + b[i] * points[j].y + c[i];
            }
            if (val[0] * val[1] <= 0 && val[2] * val[3] <= 0) {
                count++;
            } else {
                status[j] = 0;
            }
        }
    }

    return count;
}

bool Tracker::isRectangle(vector<cv::Point2f> &rectanglePoints) {

    // check the validity of transformed rectangle shape
    // the sign of outer products of each edge vector must be the same
    bool returnThis = true;

    if (rectanglePoints.size() == 4) {

        float vector[4][2];
        int i;

        vector[0][0] = rectanglePoints[1].x - rectanglePoints[0].x;
        vector[0][1] = rectanglePoints[1].y - rectanglePoints[0].y;
        vector[1][0] = rectanglePoints[2].x - rectanglePoints[1].x;
        vector[1][1] = rectanglePoints[2].y - rectanglePoints[1].y;
        vector[2][0] = rectanglePoints[3].x - rectanglePoints[2].x;
        vector[2][1] = rectanglePoints[3].y - rectanglePoints[2].y;
        vector[3][0] = rectanglePoints[0].x - rectanglePoints[3].x;
        vector[3][1] = rectanglePoints[0].y - rectanglePoints[3].y;

        int multiplicator;
        float product = vector[3][0] * vector[0][1] - vector[3][1] * vector[0][0];
        if (product > 0) {
            multiplicator = 1;
        } else {
            multiplicator = -1;
        }

        for (i = 0; i < 3; i++) {
            product = vector[i][0] * vector[i + 1][1] - vector[i][1] * vector[i + 1][0];
            if (product * multiplicator <= 0) {
                returnThis = false;
                break;
            }
        }

    } else {
        returnThis = false;
    }

    return returnThis;
}

bool Tracker::isObjectInsideImage(cv::Size img_size, vector<cv::Point2f> &pts) {

    // test is all points are inside the image area
    vector<cv::Point2f>::iterator itr = pts.begin();
    while (itr != pts.end()) {
        if (itr->x < 0 || itr->x >= img_size.width || itr->y < 0
            || itr->y >= img_size.height) {
            return false;
        } else {
            itr++;
        }
    }
    return true;
}

cv::Mat Tracker::transPointVecToMatHom(vector<cv::Point2f> &pt_vec) {

    // Convert Point2f structure vector to Mat type in the homogeneous coordinates
    int size = pt_vec.size();
    cv::Mat retMat(3, size, CV_64FC1);

    for (int i = 0; i < size; i++) {
        retMat.at<double>(0, i) = (double) (pt_vec[i].x);
        retMat.at<double>(1, i) = (double) (pt_vec[i].y);
        retMat.at<double>(2, i) = 1.0;
    }

    return retMat;
}

vector<cv::Point2f> Tracker::calcAffineTransformPoints(vector<cv::Point2f> &pts_vec,
                                                       cv::Mat &transMat) {
    vector<cv::Point2f> ret_vec;
    if (pts_vec.empty()) return ret_vec;

    cv::Mat src_mat = transPointVecToMatHom(pts_vec);

    cv::Mat dest_mat = transMat * src_mat;

    cv::Point2f pt;
    int cols = dest_mat.cols;
    for (int i = 0; i < cols; i++) {
        pt.x = (float) (dest_mat.at<double>(0, i)
                        / dest_mat.at<double>(2, i));
        pt.y = (float) (dest_mat.at<double>(1, i)
                        / dest_mat.at<double>(2, i));
        ret_vec.push_back(pt);
    }

    return ret_vec;
}

cv::Mat Tracker::createMask(cv::Size img_size, vector<cv::Point2f> &pts) {

    // create feature mask
    cv::Mat mask(img_size, CV_8UC1);

    int zero = 0;
    mask = zero;

    // ax + by + c = 0
    float a[4];
    float b[4];
    float c[4];

    a[0] = pts[3].y - pts[0].y;
    a[1] = pts[2].y - pts[1].y;
    a[2] = pts[1].y - pts[0].y;
    a[3] = pts[2].y - pts[3].y;

    b[0] = pts[0].x - pts[3].x;
    b[1] = pts[1].x - pts[2].x;
    b[2] = pts[0].x - pts[1].x;
    b[3] = pts[3].x - pts[2].x;

    c[0] = pts[0].y * pts[3].x - pts[3].y * pts[0].x;
    c[1] = pts[1].y * pts[2].x - pts[2].y * pts[1].x;
    c[2] = pts[0].y * pts[1].x - pts[1].y * pts[0].x;
    c[3] = pts[3].y * pts[2].x - pts[2].y * pts[3].x;

    float max_x, min_x, max_y, min_y;
    max_x = 0;
    min_x = img_size.width;
    max_y = 0;
    min_y = img_size.height;

    int i;
    for (i = 0; i < 4; i++) {
        if (pts[i].x > max_x) max_x = pts[i].x;
        if (pts[i].x < min_x) min_x = pts[i].x;
        if (pts[i].y > max_y) max_y = pts[i].y;
        if (pts[i].y < min_y) min_y = pts[i].y;
    }
    if (max_x >= img_size.width) max_x = img_size.width - 1;
    if (max_y >= img_size.height) max_y = img_size.height - 1;
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;

    unsigned char *ptr = mask.data;
    int x, y;
    int offset;
    float val[4];
    for (y = min_y; y <= max_y; y++) {
        offset = y * img_size.width;
        for (x = min_x; x <= max_x; x++) {
            for (i = 0; i < 4; i++) {
                val[i] = a[i] * x + b[i] * y + c[i];
            }
            if (val[0] * val[1] <= 0 && val[2] * val[3] <= 0)
                *(ptr + offset
                  + x) = 255;
        }
    }

    return mask;
}

#endif
