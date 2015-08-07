#ifndef OPENMAKAENGINE_SCENEFRAME_H
#define OPENMAKAENGINE_SCENEFRAME_H

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

namespace om {

    class SceneFrame {

    public:

        SceneFrame(cv::Mat &rgbInputFrame, cv::Mat &grayInputFrame, int scale);

        virtual ~SceneFrame(void);

        cv::Mat rgb;
        cv::Mat gray;
        cv::Mat homography;

        float scale;

        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        std::vector<cv::DMatch> matches;

        std::vector<cv::Point2f> objectPosition;

    };

}

#endif
