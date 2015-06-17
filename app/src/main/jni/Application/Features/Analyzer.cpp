#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp> // for homography

#include "Analyzer.hpp"

Analyzer* Analyzer::inst_ = NULL;

Analyzer::Analyzer(void)
{
}

Analyzer::~Analyzer(void)
{
}

Analyzer* Analyzer::getInstance()
{
    if (inst_ == NULL) {
        inst_ = new Analyzer();
    }
    return inst_;
}

cv::Ptr<cv::FeatureDetector> Analyzer::getDetector(std::string type)
{
            //cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::DenseFeatureDetector());
            //cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::FastFeatureDetector());
            //cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::GFTTDetector());
            //cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::MSER());
            //cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::ORB());
            //cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::SIFT());
            //cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::StarFeatureDetector());
            //cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::SURF(600.0));
            cv::Ptr<cv::FeatureDetector> detector = cv::Ptr<cv::FeatureDetector>(new cv::BRISK());

            return detector;
}
