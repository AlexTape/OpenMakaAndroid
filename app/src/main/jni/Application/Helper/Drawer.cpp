#ifndef OPENMAKAENGINE_DRAWER_CPP
#define OPENMAKAENGINE_DRAWER_CPP

#include "Drawer.h"
#include "../Controller.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace om;

Drawer::Drawer(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Creating Drawer instance.." << endl;
    }
}

Drawer::~Drawer(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Drawer instance.." << endl;
    }
}

void Drawer::drawContour(cv::Mat &image, std::vector<cv::Point2f> points2d, cv::Scalar color, int thickness,
                         int lineType, int shift) {
    for (size_t i = 0; i < points2d.size(); i++) {
        cv::line(image, points2d[i], points2d[(i + 1) % points2d.size()], color, thickness, lineType, shift);
    }
}

void Drawer::drawKeypoints(cv::Mat &image, std::vector<cv::KeyPoint> keyPoints, cv::Scalar color) {
    for (unsigned int i = 0; i < keyPoints.size(); i++) {
        const cv::KeyPoint &kp = keyPoints[i];
        // TODO fix this
        cv::circle(image, cv::Point(kp.pt.x, kp.pt.y), 10, cv::Scalar(255, 0, 0, 255));
    }
}

cv::Mat Drawer::drawMatchesWindow(cv::Mat query, cv::Mat pattern, const std::vector<cv::KeyPoint> &queryKp,
                                  const std::vector<cv::KeyPoint> &trainKp, std::vector<cv::DMatch> matches,
                                  int maxMatchesDrawn) {
    cv::Mat outImg;

    if (matches.size() > maxMatchesDrawn) {
        matches.resize(maxMatchesDrawn);
    }

    cv::drawMatches
            (
                    query,
                    queryKp,
                    pattern,
                    trainKp,
                    matches,
                    outImg,
                    cv::Scalar(0, 200, 0, 255),
                    cv::Scalar::all(-1),
                    std::vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
            );

    return outImg;
}

cv::Mat Drawer::drawKeypointsWindow(cv::Mat query, cv::Mat pattern, const std::vector<cv::KeyPoint> &queryKp,
                                    const std::vector<cv::KeyPoint> &trainKp, std::vector<cv::DMatch> matches,
                                    int maxMatchesDrawn) {
    cv::Mat outImg;

    if (matches.size() > maxMatchesDrawn) {
        matches.resize(maxMatchesDrawn);
    }

    cv::drawMatches
            (
                    query,
                    queryKp,
                    pattern,
                    trainKp,
                    matches,
                    outImg,
                    cv::Scalar(0, 200, 0, 255),
                    cv::Scalar::all(-1),
                    std::vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
            );

    return outImg;
}

#endif
