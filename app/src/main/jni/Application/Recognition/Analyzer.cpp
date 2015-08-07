#ifndef OPENMAKAENGINE_ANALYZER_CPP
#define OPENMAKAENGINE_ANALYZER_CPP

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <iomanip>

#include "Analyzer.h"
#include "../Controller.h"
#include "../Helper/Drawer.h"
#include "../akaze/akaze_features.h"

using namespace std;
using namespace om;

// init static members
Analyzer *Analyzer::inst_ = NULL;
string Analyzer::DETECTOR;
string Analyzer::EXTRACTOR;
string Analyzer::MATCHER;
int Analyzer::MINIMUM_INLIERS;
int Analyzer::MINIMUM_MATCHES;
float Analyzer::NN_DISTANCE_RATIO;
int Analyzer::K_GROUPS;
double Analyzer::RANSAC_REPROJECTION_THRESHOLD;

Analyzer::Analyzer(void) {

    if (Controller::MODE_DEBUG) {
        cout << "Creating Analyzer instance.." << endl;
    }

    // init nonfree module for SURF support
    cv::initModule_nonfree();

    // setup measurements
    clock = new Timer();
    timer = new Timer();

    // preinit vars to avoid segmentation faults
    activeObjectPattern = 0;

    // set variables
    isInitialized = false;
}

Analyzer::~Analyzer(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Analyzer instance.." << endl;
    }
    releaseOpenCV();
    delete activeObjectPattern;
    delete clock;
    delete timer;
}

Analyzer *Analyzer::getInstance() {
    if (inst_ == NULL) {

        // create singleton instance
        inst_ = new Analyzer();

        // init attributes
        inst_->isInitialized = false;

    }
    return inst_;
}

void Analyzer::initDetector(std::string &type) {
    if (type == "DENSE") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::DenseFeatureDetector());
    }
    else if (type == "FAST") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::FastFeatureDetector());
    }
    else if (type == "GFTT") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::GFTTDetector());
    }
    else if (type == "MSER") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::MSER());
    }
    else if (type == "SIFT") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::SIFT(400, 5, 0.04, 25, 2.6));
    }
    else if (type == "STAR") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::StarFeatureDetector());
    }
    else if (type == "SURF") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::SURF(600.0));
    }
    else if (type == "BRISK") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::BRISK(60, 4, 1.0f));
    }
    else if (type == "ORB") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::ORB(1000));
    }
    else if (type == "AKAZE") {
        detector = cv::Ptr<cv::FeatureDetector>(new cv::AKAZE());
    }
}

void Analyzer::initExtractor(std::string &type) {
    if (type == "SIFT") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::SIFT());
    }
    else if (type == "BRIEF") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::BriefDescriptorExtractor());
    }
    else if (type == "ORB") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::ORB());
    }
    else if (type == "SURF") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::SURF(600.0));
    }
    else if (type == "BRISK") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::BRISK());
    }
    else if (type == "FREAK") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::FREAK(false, false));
    }
    else if (type == "AKAZE") {
        extractor = cv::Ptr<cv::DescriptorExtractor>(new cv::AKAZE());
    }
}

void Analyzer::initMatcher(std::string &type) {
    if (type == "BF_NORM_HAMMING") {
        // NOTE: OpenCV Error: Assertion failed (K == 1 && update == 0 && mask.empty()) in batchDistance
        // https://github.com/MasteringOpenCV/code/issues/5
        matcher = cv::Ptr<cv::DescriptorMatcher>(new cv::BFMatcher(cv::NORM_HAMMING, false));
        IS_BRUTEFORCE_MATCHER = true;
    }
    else if (type == "FLANN_LSF") {
        indexParams = new cv::flann::LshIndexParams(12, 20, 2);
        searchParams = new cv::flann::SearchParams();
        matcher = cv::Ptr<cv::DescriptorMatcher>(new cv::FlannBasedMatcher(indexParams, searchParams));
        IS_BRUTEFORCE_MATCHER = false;
    }
    else if (type == "FLANN_KD") {
        indexParams = new cv::flann::KDTreeIndexParams();
        searchParams = new cv::flann::SearchParams();
        matcher = cv::Ptr<cv::DescriptorMatcher>(new cv::FlannBasedMatcher(indexParams, searchParams));
        IS_BRUTEFORCE_MATCHER = false;
    }
}

bool Analyzer::analyze(cv::Mat &gray, std::vector<cv::KeyPoint> &keypoints, cv::Mat &descriptors) {

    bool returnThis = true;

    try {

        // if there is no image data
        if (Controller::MODE_DEBUG) {
            assert(!gray.empty());
        }
        if (gray.empty()) {
            cvError(0, "Analyzer", "Input image empty!", __FILE__, __LINE__);
        }

        // detect keypoints
        keypoints.clear();
        if (Controller::MODE_DEBUG) {
            assert(keypoints.empty());
        }
        detector->detect(gray, keypoints);

        // no keypoints detected!
        if (keypoints.empty()) {
            cvError(0, "Analyzer", "Detection keypoints empty!", __FILE__, __LINE__);
        }

        // compute descriptors
        descriptors.release();
        if (Controller::MODE_DEBUG) {
            assert(descriptors.empty());
        }
        extractor->compute(gray, keypoints, descriptors);

        // note: keypoints for which a descriptor cannot be computed are removed
        if (keypoints.empty()) {
            cvError(0, "Analyzer", "Compute keypoints empty!", __FILE__, __LINE__);
        }

    } catch (cv::Exception &exception) {
        if (Controller::MODE_DEBUG) {
            // NOTE: e.g. dark images can contain 0 features!
            cout << exception.what() << endl;
        }
        returnThis = false;
    }

    return returnThis;
}

bool Analyzer::analyseSceneFrame(SceneFrame &sceneFrame) {

    // validate input data
    if (sceneFrame.gray.empty()) {
        if (Controller::MODE_DEBUG) {
            cvError(0, "Analyzer::process", "Scene image empty!", __FILE__, __LINE__);
        }
        return false;
    }
    if (activeObjectPattern->image.empty()) {
        if (Controller::MODE_DEBUG) {
            cvError(0, "Analyzer::process", "Object image empty!", __FILE__, __LINE__);
        }
        return false;
    }

    analyze(sceneFrame.gray, sceneFrame.keypoints, sceneFrame.descriptors);

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("SceneKeypoints", (int) sceneFrame.keypoints.size());
        Controller::statistics("ObjectKeypoints", (int) activeObjectPattern->keypoints.size());
    }

    if (Controller::MODE_DEBUG) {
        cout << "Keypoints(Scene/Object)=" << sceneFrame.keypoints.size() << "/" <<
        activeObjectPattern->keypoints.size() <<
        endl;
    }

    if (activeObjectPattern->descriptors.empty()) {
        if (Controller::MODE_DEBUG) {
            cout << "object descriptors empty" << endl;
        }
        return false;
    }
    if (sceneFrame.descriptors.empty()) {
        if (Controller::MODE_DEBUG) {
            cout << "scene descriptors empty" << endl;
        }
        return false;
    }
}

void Analyzer::matchBinaryDescriptors(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                                      std::vector<cv::Point2f> &goodSceneKeypoints) {

    if (Controller::MODE_DEBUG) {
        cout << "Binary descriptors detected..." << endl;
    }

    timer->restart();

    cv::Mat resultIndex;
    cv::Mat distanceIndex;
    std::vector<std::vector<cv::DMatch> > matches;
    std::vector<int> objectKeypointIndex, sceneKeypointIndex;

    if (IS_BRUTEFORCE_MATCHER) {

        // train first
        cv::BFMatcher matcher(cv::NORM_HAMMING, false);
//                matcher.radiusMatch(activeObjectPattern->descriptors, sceneFrame.descriptors, matches, 1.0 );
        matcher.knnMatch(activeObjectPattern->descriptors, sceneFrame.descriptors, matches, K_GROUPS);

        for (unsigned int i = 0; i < matches.size(); ++i) {

            if (Controller::MODE_DEBUG) {
                //cout << "DistanceIndex=" << i << "; distance1=" << matches.at(i).at(0).queryIdx << "; distance2=" <<
                //matches.at(i).at(0).distance << ";" << endl;
            }

            unsigned int queryId = abs(matches.at(i).at(0).queryIdx);
            goodTrainKeypoints.push_back(activeObjectPattern->keypoints.at(queryId).pt);
            objectKeypointIndex.push_back(matches.at(i).at(0).queryIdx);

            unsigned int trainId = abs(matches.at(i).at(0).trainIdx);
            goodSceneKeypoints.push_back(sceneFrame.keypoints.at(trainId).pt);
            sceneKeypointIndex.push_back(matches.at(i).at(0).trainIdx);

        }

    } else {

        // Create Flann LSH index
        cv::flann::Index flannIndex(sceneFrame.descriptors, cv::flann::LshIndexParams(12, 20, 2),
                                    cvflann::FLANN_DIST_HAMMING);

        if (Controller::MODE_DEBUG) {
            cout << "Flann LSH Index created.." << endl;
        }

        resultIndex = cv::Mat(activeObjectPattern->descriptors.rows, K_GROUPS, CV_32SC1); // Results index
        // TODO Distance resultIndex are CV_32FC1 ?!?!? NOTE OpenCV doc is not clear about that...
        distanceIndex = cv::Mat(activeObjectPattern->descriptors.rows, K_GROUPS,
                                CV_32FC1);

        flannIndex.knnSearch(activeObjectPattern->descriptors, resultIndex, distanceIndex, K_GROUPS,
                             cv::flann::SearchParams());

        for (int i = 0; i < activeObjectPattern->descriptors.rows; ++i) {

            if (Controller::MODE_DEBUG) {
                //cout << "DistanceIndex=" << i << "; distance1=" << distanceIndex.at<float>(i, 0) << "; distance2=" <<
                //distanceIndex.at<float>(i, 1) << ";" << endl;
            }

            // TODO check if needed!
            if (isnan(distanceIndex.at<float>(i, 0)) || isnan(distanceIndex.at<float>(i, 1))) {
                continue;
            }

            goodTrainKeypoints.push_back(activeObjectPattern->keypoints.at(i).pt);
            objectKeypointIndex.push_back(i);

            unsigned int resultIndexId = abs(resultIndex.at<int>(i, 0));
            goodSceneKeypoints.push_back(sceneFrame.keypoints.at(resultIndexId).pt);
            sceneKeypointIndex.push_back(resultIndex.at<int>(i, 0));
        }
    }

    if (Controller::MODE_DEBUG) {
        cout << "matchBinaryDescriptors=" << timer->getMillis() << endl;
    }

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("MatchBinaryDescriptors", (double) timer->getMillis());
    }
}

void Analyzer::matchFloatDescriptors(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                                     std::vector<cv::Point2f> &goodSceneKeypoints) {

    if (Controller::MODE_DEBUG) {
        cout << "Float descriptors detected..." << endl;
    }

    timer->restart();

    cv::Mat resultIndex;
    cv::Mat distanceIndex;
    std::vector<std::vector<cv::DMatch> > matches;

    // Find correspondences by NNDR (Nearest Neighbor Distance Ratio)
    // Check if this descriptor matches with those of the objects
    std::vector<int> objectKeypointIndex, sceneKeypointIndex; // Used for homography

    if (IS_BRUTEFORCE_MATCHER) {
        cv::BFMatcher matcher(cv::NORM_L2);
        matcher.knnMatch(activeObjectPattern->descriptors, sceneFrame.descriptors, matches, K_GROUPS);

        for (unsigned int i = 0; i < matches.size(); ++i) {

            if (Controller::MODE_DEBUG) {
                //cout << "DistanceIndex=" << i << "; distance1=" << matches.at(i).at(0).queryIdx << "; distance2=" <<
                //matches.at(i).at(0).distance << ";" << endl;
            }

            if (matches.at(i).at(0).distance <= NN_DISTANCE_RATIO * matches.at(i).at(1).distance) {

                unsigned int queryId = abs(matches.at(i).at(0).queryIdx);
                goodTrainKeypoints.push_back(activeObjectPattern->keypoints.at(queryId).pt);
                objectKeypointIndex.push_back(matches.at(i).at(0).queryIdx);

                unsigned int trainId = abs(matches.at(i).at(0).trainIdx);
                goodSceneKeypoints.push_back(sceneFrame.keypoints.at(trainId).pt);
                sceneKeypointIndex.push_back(matches.at(i).at(0).trainIdx);
            }
        }
    } else {

        // Create Flann KDTree index
        cv::flann::Index flannIndex(sceneFrame.descriptors, cv::flann::KDTreeIndexParams(),
                                    cvflann::FLANN_DIST_EUCLIDEAN);

        if (Controller::MODE_DEBUG) {
            cout << "Flann KDTree created.." << endl;
        }

        resultIndex = cv::Mat(activeObjectPattern->descriptors.rows, K_GROUPS, CV_32SC1); // Results index
        distanceIndex = cv::Mat(activeObjectPattern->descriptors.rows, K_GROUPS,
                                CV_32FC1); // Distance resultIndex are CV_32FC1

        // search (nearest neighbor)
        flannIndex.knnSearch(activeObjectPattern->descriptors, resultIndex, distanceIndex, K_GROUPS,
                             cv::flann::SearchParams());

        for (int i = 0; i < activeObjectPattern->descriptors.rows; ++i) {

            if (Controller::MODE_DEBUG) {
                //cout << "DistanceIndex=" << i << "; distance1=" << distanceIndex.at<float>(i, 0) << "; distance2=" <<
                //distanceIndex.at<float>(i, 1) << ";" << endl;
            }

            // TODO check if needed!
            if (isnan(distanceIndex.at<float>(i, 0)) || isnan(distanceIndex.at<float>(i, 1))) {
                continue;
            }

            if (distanceIndex.at<float>(i, 0) <= NN_DISTANCE_RATIO * distanceIndex.at<float>(i, 1)) {
                goodTrainKeypoints.push_back(activeObjectPattern->keypoints.at(i).pt);
                objectKeypointIndex.push_back(i);

                unsigned int resultIndexId = abs(resultIndex.at<int>(i, 0));
                goodSceneKeypoints.push_back(sceneFrame.keypoints.at(resultIndexId).pt);
                sceneKeypointIndex.push_back(resultIndex.at<int>(i, 0));
            }
        }

    }

    if (Controller::MODE_DEBUG) {
        cout << "matchFloatDescriptors=" << timer->getMillis() << endl;
    }

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("MatchFloatDescriptors(ms)", (double) timer->getMillis());
    }

}

bool Analyzer::process(SceneFrame &sceneFrame) {

    // calc processing time for this method
    clock->restart();

    bool foundObject = false;
    IS_BRUTEFORCE_MATCHER = true;

    if (isInitialized) {

        timer->restart();
        // analyse features and descriptors for frame
        analyseSceneFrame(sceneFrame);

        if (Controller::MODE_DEBUG) {
            cout << "processSceneFrame=" << timer->getMillis() << endl;
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("AnalyseSceneFrame", (double) timer->getMillis());
        }

        std::vector<cv::Point2f> goodTrainKeypoints;
        std::vector<cv::Point2f> goodSceneKeypoints;

        // check preconditions
        // there have to descriptors! (catch black screens etc)
        if (activeObjectPattern->descriptors.rows <= 0) {
            return false;
        }
        if (sceneFrame.descriptors.rows <= 0) {
            return false;
        }

        if (activeObjectPattern->descriptors.type() == CV_8U) {
            matchBinaryDescriptors(sceneFrame, goodTrainKeypoints, goodSceneKeypoints);
        }

        if (activeObjectPattern->descriptors.type() == CV_32F) {
            matchFloatDescriptors(sceneFrame, goodTrainKeypoints, goodSceneKeypoints);
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("GoodTrainKeypoints", (int) goodTrainKeypoints.size());
            Controller::statistics("GoodSceneKeypoints", (int) goodSceneKeypoints.size());
        }

        if (goodSceneKeypoints.size() != goodTrainKeypoints.size()) {
            if (Controller::MODE_DEBUG) {
                cvError(0, "MatchFinder", "goodScene and goodObject keypoints size NOT equal!", __FILE__, __LINE__);
            }
            return false;
        }

        int inliers = 0;

        // NOTE: minimum 4 points needed to calc homography
        if (goodSceneKeypoints.size() >= MINIMUM_MATCHES) {
            if (goodSceneKeypoints.size() == goodTrainKeypoints.size()) {
                timer->restart();
                inliers = calcInliers(sceneFrame, goodTrainKeypoints, goodSceneKeypoints);

                if (Controller::MODE_DEBUG) {
                    cout << "calcInliers=" << timer->getMillis() << endl;
                }

                if (Controller::MODE_STATISTICS) {
                    Controller::statistics("InliersCalc(ms)", (double) timer->getMillis());
                }

            } else {

                    if (Controller::MODE_DEBUG) {
                        cout << "GoodSceneKeypoints != GoodTranKeypoints" << endl;
                    }

            }
        } else {

            if (Controller::MODE_DEBUG) {
                cout << "Not enough keypoint matches (" << goodTrainKeypoints.size() << "/" << MINIMUM_MATCHES <<
                ") for homography.." << endl;
            }

        }

        foundObject = (inliers >= MINIMUM_INLIERS);

        // find perspective and draw rectangle
        if (foundObject) {

            timer->restart();
            perspectiveTransform(activeObjectPattern->points2d, sceneFrame.objectPosition, sceneFrame.homography);

            if (Controller::MODE_DEBUG) {
                cout << "perspectiveTransform=" << timer->getMillis() << endl;
            }

            if (Controller::MODE_STATISTICS) {
                Controller::statistics("PerspectiveTransform(ms)", (double) timer->getMillis());
            }

            // drawing contours
            Drawer::drawContour(sceneFrame.gray, sceneFrame.objectPosition, cv::Scalar(0, 255, 0));


    if (Controller::USE_WINDOWS) {
        //-- Show detected matches
//        cv::Mat Drawer::drawMatchesWindow(cv::Mat query, cv::Mat pattern, const std::vector<cv::KeyPoint> &queryKp,
//                                           const std::vector<cv::KeyPoint> &trainKp, std::vector<cv::DMatch> matches,
//                                           int maxMatchesDrawn) {

        // open custom windows
        imshow(DETECTOR + "-" + EXTRACTOR + "-" + MATCHER, sceneFrame.gray);
    }

        } else {

            if (Controller::MODE_DEBUG) {
                cout << "Not enough inliers (" << inliers << "/" << MINIMUM_INLIERS << ") calculating perspective" <<
                endl;
            }

        }

        if (Controller::MODE_DEBUG) {
            cout << "Analyzer::process=" << clock->getMillis() << endl;
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("AnalyzerProcess(ms)", (double) clock->getMillis());
        }

    } else {
        if (Controller::MODE_DEBUG) {
            cvError(0, "Analyzer", "Analyzer not initialized!", __FILE__, __LINE__);
        }
    }

if (Controller::USE_WINDOWS) {
    // add text to window(s)
    if (foundObject) {
        cv::putText(sceneFrame.gray, " Found Object: true" + foundObject, cv::Point(10, 15), CV_FONT_HERSHEY_PLAIN, 1,
                    CV_RGB(0, 200, 0));
    } else {
        cv::putText(sceneFrame.gray, " Found Object: false" + foundObject, cv::Point(10, 15), CV_FONT_HERSHEY_PLAIN, 1,
                    CV_RGB(0, 200, 0));
    }

    // open custom windows
    imshow(DETECTOR + "-" + EXTRACTOR + "-" + MATCHER, sceneFrame.gray);
}

    return foundObject;
}

int Analyzer::calcInliers(SceneFrame &sceneFrame, std::vector<cv::Point2f> &goodTrainKeypoints,
                          std::vector<cv::Point2f> &goodSceneKeypoints) {

    std::vector<uchar> outlierMask;  // Used for homography

    sceneFrame.homography = findHomography(goodTrainKeypoints,
                                           goodSceneKeypoints,
                                           cv::RANSAC,
                                           RANSAC_REPROJECTION_THRESHOLD,
                                           outlierMask);

    int inliers = 0, outliers = 0;
    for (unsigned int k = 0; k < goodTrainKeypoints.size(); ++k) {
        if (outlierMask.at(k)) {
            ++inliers;
        }
        else {
            ++outliers;
        }
    }

    if (Controller::MODE_DEBUG) {
        cout << "calcInliers: Inliers=" << inliers << "; Outliers=" << outliers << ";" << endl;
    }

    if (Controller::MODE_STATISTICS) {
        Controller::statistics("Inliers", (int) inliers);
        Controller::statistics("Outliers", (int) outliers);
    }

    return inliers;
}

void Analyzer::match(SceneFrame &query) {
    assert(!query.descriptors.empty());
//    query.matches.clear();
    matcher->match(query.descriptors, query.matches);
}

void Analyzer::train(const cv::Mat &descriptors) {

    // clear old training data
    matcher->clear();

    // TODO register pattern to list and extends descriptor vector to hold multiple patterns?
    // create a descriptor vector to store descriptor data
    std::vector<cv::Mat> descriptorList(1);
    descriptorList[0] = descriptors.clone();

    // promote descriptor list to matcher instance
    matcher->add(descriptorList);

    // train matcher
    matcher->train();
}

// TODO überarbeiten
bool Analyzer::refineMatches
        (SceneFrame &query, ObjectPattern &pattern) {
    const int minNumberMatchesAllowed = 8;

    // TODO make matching working..
    cout << "RefineMatches [START]: SceneFrame Keypoints: " << std::setw(4) << query.keypoints.size() <<
    "; SceneFrame Matches: " << std::setw(4) <<
    query.matches.size() << endl;

    std::vector<cv::KeyPoint> trainKeypoints = pattern.keypoints;

    if (query.matches.size() < minNumberMatchesAllowed)
        return false;

    // Prepare data for cv::findHomography
    std::vector<cv::Point2f> srcPoints(query.matches.size());
    std::vector<cv::Point2f> dstPoints(query.matches.size());

    for (size_t i = 0; i < query.matches.size(); i++) {

        cout << pattern.keypoints[i].response << endl;

    }

    for (size_t i = 0; i < query.matches.size(); i++) {

        srcPoints[i] = pattern.keypoints[query.matches[i].trainIdx].pt;
        dstPoints[i] = query.keypoints[query.matches[i].queryIdx].pt;
    }

    cout << srcPoints.size() << "::" << dstPoints.size() << endl;

    // Find homography matrix and get inliers mask
    std::vector<unsigned char> inliersMask(srcPoints.size());
    cv::Mat homography = cv::findHomography(srcPoints,
                                            dstPoints,
                                            CV_FM_RANSAC,
                                            3.0,
                                            inliersMask);

    // TODO update homo here
    //trackerObject.homography = &homography;

    std::vector<cv::DMatch> inliers;
    for (size_t i = 0; i < inliersMask.size(); i++) {
        if (inliersMask[i]) {
            inliers.push_back(query.matches[i]);
        }
    }

    query.matches.swap(inliers);

    cout << "RefineMatches [ END ]: SceneFrame Keypoints: " << std::setw(4) << query.keypoints.size() <<
    "; SceneFrame Matches: " << std::setw(4) <<
    query.matches.size() << endl;

    return query.matches.size() > minNumberMatchesAllowed;

}

bool Analyzer::initialize() {
    if (!isInitialized) {

        // clear detector/extractor instances if needed
        if (Analyzer::detector || Analyzer::extractor) {
            Analyzer::releaseOpenCV();
        }

        // init detector/extractor
        if (!Analyzer::DETECTOR.empty() && !Analyzer::EXTRACTOR.empty() && !Analyzer::MATCHER.empty()) {
            Analyzer::initDetector(DETECTOR);
            Analyzer::initExtractor(EXTRACTOR);
            Analyzer::initMatcher(MATCHER);
        } else {
            return false;
        }

        // done
        isInitialized = true;
    }
    return isInitialized;
}

bool Analyzer::releaseOpenCV() {
    Analyzer::detector.release();
    Analyzer::extractor.release();
    Analyzer::matcher.release();
    return true;
}

bool Analyzer::createObjectPattern(cv::Mat &gray) {

    bool returnThis = false;

    if (!gray.empty()) {
        delete activeObjectPattern;
        activeObjectPattern = new ObjectPattern(gray);
        if (activeObjectPattern && isInitialized) {
            returnThis = analyze(activeObjectPattern->image, activeObjectPattern->keypoints,
                                 activeObjectPattern->descriptors);
        }
    } else {
        if (Controller::MODE_DEBUG) {
            cvError(0, "Analyzer::createObjectPattern", "Image empty!", __FILE__, __LINE__);
        }
    }

    return returnThis;
}

#endif