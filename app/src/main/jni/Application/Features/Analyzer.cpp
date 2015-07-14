#define ANALYZER_CPP_

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Analyzer.hpp"

Analyzer *Analyzer::inst_ = NULL;

Analyzer::Analyzer(void) {
    isComputed = false;
    isInitialized = false;
}

Analyzer::~Analyzer(void) {
}

Analyzer *Analyzer::getInstance() {
    if (inst_ == NULL) {
        cv::initModule_nonfree();
        inst_ = new Analyzer();
        inst_->isInitialized = false;
        inst_->isComputed = false;
    }
    return inst_;
}

void Analyzer::setDetector(std::string type) {
    //inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::DenseFeatureDetector());
    inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::FastFeatureDetector());
    //inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::GFTTDetector());
    //inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::MSER());
    //inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::ORB());
    //inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::SIFT());
    //inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::StarFeatureDetector());
    //inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::SURF(600.0));
    //inst_->detector = cv::Ptr<cv::FeatureDetector>(new cv::BRISK());
}

void Analyzer::setExtractor(std::string type) {
    //inst_->extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::SIFT());
    //inst_->extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::BriefDescriptorExtractor());
    //inst_->extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::ORB());
    //inst_->extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::SURF(600.0));
    //inst_->extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::BRISK());
    inst_->extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::FREAK());
}

bool Analyzer::initialize(cv::Mat mGrayFrame) {
    cv::Size graySize = mGrayFrame.size();
    int rows = graySize.height;
    int cols = graySize.width;

    cv::Mat subject;

    // TODO make dynamic linking
    subject = cv::imread("/storage/emulated/0/Android/data/de.alextape.openmaka/files/marker/miku.jpg", 0);

//    cv::Mat resizedSubject;
//    cv::resize(subject, resizedSubject, graySize);

    inst_->detector->detect(subject, inst_->objectKeypoints);
    inst_->extractor->compute(mGrayFrame, inst_->objectKeypoints, inst_->objectDescriptors);

    return true;
}

bool Analyzer::initialized(cv::Mat mGrayFrame) {
    if (!inst_->isInitialized) {

        Analyzer::initialize(mGrayFrame);
        inst_->isInitialized = true;
    }
    return inst_->isInitialized;
}

void Analyzer::match() {

//    OpenMaka::Timer time;
//    time.start();
//
//    cv::Mat results;
//    cv::Mat dists;
//    std::vector<std::vector<cv::DMatch> > matches;
//    int k=2; // find the 2 nearest neighbors
//    bool useBFMatcher = false; // SET TO TRUE TO USE BRUTE FORCE MATCHER
//    if(objectDescriptors.type()==CV_8U)
//    {
//        // Binary descriptors detected (from ORB, Brief, BRISK, FREAK)
//        log_info(AnalyzerTAG,"Binary descriptors detected...");
//        if(useBFMatcher)
//        {
//            cv::BFMatcher matcher(cv::NORM_HAMMING); // use cv::NORM_HAMMING2 for ORB descriptor with WTA_K == 3 or 4 (see ORB constructor)
//            matcher.knnMatch(objectDescriptors, sceneDescriptors, matches, k);
//        }
//        else
//        {
//            // Create Flann LSH index
//            cv::flann::Index flannIndex(sceneDescriptors, cv::flann::LshIndexParams(12, 20, 2), cvflann::FLANN_DIST_HAMMING);
//            log("Time creating FLANN LSH index = %d ms\n", time.restart());
//
//            // search (nearest neighbor)
//            flannIndex.knnSearch(objectDescriptors, results, dists, k, cv::flann::SearchParams() );
//        }
//    }
//    else
//    {
//        // assume it is CV_32F
//        log_info(AnalyzerTAG,"Float descriptors detected...");
//        if(useBFMatcher)
//        {
//            cv::BFMatcher matcher(cv::NORM_L2);
//            matcher.knnMatch(objectDescriptors, sceneDescriptors, matches, k);
//        }
//        else
//        {
//            // Create Flann KDTree index
//            cv::flann::Index flannIndex(sceneDescriptors, cv::flann::KDTreeIndexParams(), cvflann::FLANN_DIST_EUCLIDEAN);
//            log("Time creating FLANN KDTree index = %d ms\n", time.restart());
//
//            // search (nearest neighbor)
//            flannIndex.knnSearch(objectDescriptors, results, dists, k, cv::flann::SearchParams() );
//        }
//    }
//    log("Time nearest neighbor search = %d ms\n", time.restart());
//
//    // Conversion to CV_32F if needed
//    if(dists.type() == CV_32S)
//    {
//        cv::Mat temp;
//        dists.convertTo(temp, CV_32F);
//        dists = temp;
//    }
}

int Analyzer::compute(cv::Mat mRgbaFrame, cv::Mat mGrayFrame) {

    int returnThis = 0;

//    if (!inst_->isComputed) {

    inst_->detector->detect(mGrayFrame, inst_->sceneKeypoints);
//        inst_->extractor->compute(mGrayFrame, inst_->sceneKeypoints, inst_->sceneDescriptors);

//        inst_->match();

    inst_->isComputed = true;

//    }

    for (unsigned int i = 0; i < inst_->sceneKeypoints.size(); i++) {
        const cv::KeyPoint &kp = inst_->sceneKeypoints[i];
        cv::circle(mRgbaFrame, cv::Point(kp.pt.x, kp.pt.y), 10, cv::Scalar(255, 0, 0, 255));
    }

    return returnThis;
}
