#ifndef OPENMAKAENGINE_CONTROLLER_CPP
#define OPENMAKAENGINE_CONTROLLER_CPP

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
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <iomanip>
#include <iostream>

#include "Controller.h"
#include "Helper/Drawer.h"

#include "androidbuf.h"

using namespace std;
using namespace cv;
using namespace om;

// init static members
Controller *Controller::inst_ = NULL;
bool Controller::MODE_OBJECT_DETECTION;
bool Controller::MODE_TRACKING;
bool Controller::MODE_OPENGL;
Analyzer *Controller::analyzer;
Tracker *Controller::tracker;
Statistics *Controller::stats;
bool Controller::isInitialized;
cv::Size Controller::FRAME_SIZE;
std::string Controller::STORAGE_PATH;
std::string Controller::DEFAULT_OBJECT_IMAGE;
std::string Controller::STATISTICS_FILE;
bool Controller::MODE_STATISTICS;
bool Controller::MODE_DEBUG;
bool Controller::USE_WINDOWS;

Controller::Controller(void) {

    if (Controller::MODE_DEBUG) {
        cout << "Creating Controller instance.." << endl;
    }

    // define analyzer instance
    analyzer = Analyzer::getInstance();

    // define tracker instance
    tracker = Tracker::getInstance();

    // initialize variables
    sceneFrame = 0;

    // set acutal state
    isInitialized = false;

    // set environment triggers
    MODE_OBJECT_DETECTION = false;
    MODE_TRACKING = false;
    MODE_OPENGL = false;

    // set state variables
    STATE_OBJECT_FOUND = false;
    STATE_TRACKING_OBJECT = false;
    STATE_DISPLAY_OPENGL = false;

    // create statistics instance
    stats = new Statistics();

    // create global clock
    clock = new Timer();

    // create local timer
    timer = new Timer();

#ifdef __ANDROID__
    // make cout working in logcat
    std::cout.rdbuf(new androidbuf);
#endif

}

Controller::~Controller(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Controller instance.." << endl;
    }
    delete tracker;
    delete analyzer;
    delete sceneFrame;
    delete stats;
    delete clock;
    delete timer;

#ifdef __ANDROID__
    delete std::cout.rdbuf(0);
#endif
}

Controller *Controller::getInstance() {
    if (inst_ == NULL) {
        inst_ = new Controller();
    }
    return inst_;
}

// NEEDED
int Controller::initialize(cv::Mat &frame, std::string configPath) {

    // initializing..
    isInitialized = false;

    // grab frame size
    Controller::FRAME_SIZE = cv::Size(frame.cols, frame.rows);

    // load storage
    cv::FileStorage storage(configPath, cv::FileStorage::READ);

    // load image store path
    Controller::STORAGE_PATH = (string) storage["storagePath"];
    Controller::DEFAULT_OBJECT_IMAGE = (string) storage["defaultObjectImage"];
    Controller::STATISTICS_FILE = (string) storage["statisticsFile"];

    // load environment variables
    string statistics = (string) storage["statisticsMode"];
    Controller::MODE_STATISTICS = statistics == "true";
    string debug = (string) storage["debugMode"];
    Controller::MODE_DEBUG = debug == "true";
    string windows = (string) storage["useWindows"];
    Controller::USE_WINDOWS = windows == "true";

    // load scene frame attributes
    cv::FileNode sceneFrameNode = storage["sceneFrame"];
    SceneFrame::MAX_IMAGE_SIZE = (int) sceneFrameNode["maxImageSize"];

    // load analyzer attributes
    cv::FileNode analyzerNode = storage["analyzer"];
    Analyzer::DETECTOR = (string) analyzerNode["detector"];
    Analyzer::EXTRACTOR = (string) analyzerNode["extractor"];
    Analyzer::MATCHER = (string) analyzerNode["matcher"];
    Analyzer::MINIMUM_INLIERS = (int) analyzerNode["minimumInliers"];
    Analyzer::MINIMUM_MATCHES = (int) analyzerNode["minimumMatches"];
    Analyzer::NN_DISTANCE_RATIO = (float) analyzerNode["nnDistanceRatio"];
    Analyzer::K_GROUPS = (int) analyzerNode["kGroups"];
    Analyzer::RANSAC_REPROJECTION_THRESHOLD = (double) analyzerNode["ransacReprojectionThreshold"];

    // initialize analyzer
    analyzer->initialize();

    // load tracker attributes
    cv::FileNode trackerNode = storage["tracker"];
    Tracker::MAX_CORNERS = (int) trackerNode["maxCorners"];
    Tracker::QUALITY_LEVEL = (double) trackerNode["qualityLevel"];
    Tracker::MINIMUM_DISTANCE = (double) trackerNode["minimumDistance"];

    // add default object
    cv::Mat objectImage = cv::imread(STORAGE_PATH + DEFAULT_OBJECT_IMAGE,
                                     CV_LOAD_IMAGE_GRAYSCALE);
    analyzer->createObjectPattern(objectImage);

    // release storage
    if (storage.isOpened()) {
        storage.release();
    }

//    if (Controller::MODE_DEBUG) {
    cout << "Loading attributes.." << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "C O N T R O L L E R" << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "::STORAGE_PATH=" << Controller::STORAGE_PATH << endl;
    cout << "::FRAME_SIZE=" << Controller::FRAME_SIZE << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "S C E N E   F R A M E" << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "::MAX_IMAGE_SIZE=" << SceneFrame::MAX_IMAGE_SIZE << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "A N A L Y Z E R" << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "::DETECTOR=" << Analyzer::DETECTOR << endl;
    cout << "::EXTRACTOR=" << Analyzer::EXTRACTOR << endl;
    cout << "::MATCHER=" << Analyzer::MATCHER << endl;
    cout << "::MINIMUM_INLIERS=" << Analyzer::MINIMUM_INLIERS << endl;
    cout << "::MINIMUM_MATCHES=" << Analyzer::MINIMUM_MATCHES << endl;
    cout << "::NN_DISTANCE_RATIO=" << Analyzer::NN_DISTANCE_RATIO << endl;
    cout << "::K_GROUPS=" << Analyzer::K_GROUPS << endl;
    cout << "::RANSAC_REPROJECTION_THRESHOLD=" << Analyzer::RANSAC_REPROJECTION_THRESHOLD << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "T R A C K E R" << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "::MAX_CORNERS=" << Tracker::MAX_CORNERS << endl;
    cout << "::QUALITY_LEVEL=" << Tracker::QUALITY_LEVEL << endl;
    cout << "::MINIMUM_DISTANCE=" << Tracker::MINIMUM_DISTANCE << endl;
    cout << "-----------------------------------------------------" << endl;
    cout << "Loading done!" << endl;
//    }

    // instance is initialized
    isInitialized = true;

    // if ok
    return 1;
}

int Controller::displayFunction(cv::Mat &mRgbaFrame, cv::Mat &mGrayFrame) {


    if (Controller::MODE_STATISTICS) {
        clock->restart();
        Controller::stats->reset();
        Controller::statistics("Detector", (string) Analyzer::DETECTOR);
        Controller::statistics("Extractor", (string) Analyzer::EXTRACTOR);
    }

    int returnThis = 0;

    // if object detection is enabled..
    if (MODE_OBJECT_DETECTION) {

        // recreate object pattern if it is not existing
        if (analyzer->missingObjectPattern()) {
            if (MODE_DEBUG) {
                cout << "ObjectPattern (re-) created!" << endl;
            }
        }

        // ..and tracking disabled
        if (!MODE_TRACKING) {

            // delete old scene frame
            delete sceneFrame;

            // create new scene frame
            sceneFrame = new SceneFrame(mRgbaFrame, mGrayFrame);

            if (Controller::MODE_STATISTICS) {
                Controller::statistics("InputResolution",
                                       (string) sceneFrame->getInputResolution());
                Controller::statistics("ProcessingResolution",
                                       (string) sceneFrame->getProcessingResolution());
                timer->restart();
            }

            // analyzer processing
            STATE_OBJECT_FOUND = analyzer->process(*sceneFrame);

            if (Controller::MODE_STATISTICS) {
                Controller::statistics("AnalyzerProcess(ms)", (double) timer->getMillis());
                Controller::statistics("ObjectFound", (bool) STATE_OBJECT_FOUND);
            }

            // drawing green contours
            Drawer::drawContour(mRgbaFrame, sceneFrame->objectPosition, cv::Scalar(0, 255, 0));

        }

        // ..and tracking enabled
        if (MODE_TRACKING) {

            // if object NOT is trackable
            if (!STATE_TRACKING_OBJECT) {

                // delete old scene frame
                delete sceneFrame;

                // create new scene frame
                sceneFrame = new SceneFrame(mRgbaFrame, mGrayFrame);

                if (Controller::MODE_STATISTICS) {
                    Controller::statistics("InputResolution",
                                           (string) sceneFrame->getInputResolution());
                    Controller::statistics("ProcessingResolution",
                                           (string) sceneFrame->getProcessingResolution());
                    timer->restart();
                }

                // analyzer processing
                STATE_OBJECT_FOUND = analyzer->process(*sceneFrame);

                if (Controller::MODE_STATISTICS) {
                    Controller::statistics("AnalyzerProcess(ms)", (double) timer->getMillis());
                    Controller::statistics("ObjectFound", (bool) STATE_OBJECT_FOUND);
                }

                // if object found
                if (STATE_OBJECT_FOUND) {

                    // resize gray image
                    cv::Mat trackerFrame = sceneFrame->gray;
                    try {
                        cv::resize(mGrayFrame, trackerFrame, trackerFrame.size());
                    } catch (cv::Exception &exception) {
                        cvError(0, "TrackerFrame", "Resizing failed!", __FILE__, __LINE__);
                        cout << exception.what() << endl;
                    }

                    if (Controller::MODE_STATISTICS) {
                        Controller::statistics("InputResolution",
                                               (string) sceneFrame->getInputResolution());
                        Controller::statistics("ProcessingResolution",
                                               (string) sceneFrame->getProcessingResolution());
                        timer->restart();
                    }

                    // analyzer processing
                    bool isInImage = tracker->isObjectInsideImage(trackerFrame.size(), sceneFrame->objectPosition);

                    if (Controller::MODE_STATISTICS) {
                        Controller::statistics("isObjectInsideImage(ms)", (double) timer->getMillis());
                        Controller::statistics("isInImage", (bool) isInImage);
                    }

                    if (isInImage) {

                        if (Controller::MODE_STATISTICS) {
                            timer->restart();
                        }

                        // initialize tracking
                        tracker->initialize(trackerFrame, sceneFrame->objectPosition);

                        if (Controller::MODE_STATISTICS) {
                            Controller::statistics("TrackerInitialize(ms)", (double) timer->getMillis());
                        }

                        // can track object
                        STATE_TRACKING_OBJECT = true;

                    } else {

                        // drawing red contours
                        Drawer::drawContour(mRgbaFrame, sceneFrame->objectPosition, cv::Scalar(0, 0, 255));

                        // could not track object
                        STATE_TRACKING_OBJECT = false;

                    }

                }

            }

            // if object IS trackable
            if (STATE_TRACKING_OBJECT) {

                // resize gray image
                cv::Mat trackerFrame = sceneFrame->gray;
                try {
                    cv::resize(mGrayFrame, trackerFrame, trackerFrame.size());
                } catch (cv::Exception &exception) {
                    cvError(0, "TrackerFrame", "Resizing failed!", __FILE__, __LINE__);
                    cout << exception.what() << endl;
                }

                if (Controller::MODE_STATISTICS) {
                    timer->restart();
                }

                // processing
                STATE_TRACKING_OBJECT = tracker->process(trackerFrame);

                if (Controller::MODE_STATISTICS) {
                    Controller::statistics("TrackingProcess(ms)", (double) timer->getMillis());
                }

                // draw blue contours
                Drawer::drawContour(mRgbaFrame, tracker->objectPosition, cv::Scalar(255, 0, 0));

            }
        }

        if (Controller::MODE_STATISTICS) {
            Controller::statistics("DisplayFunction(ms)", (double) clock->getMillis());
            Controller::stats->write(STATISTICS_FILE);
        }

    }

    // return state
    if (STATE_OBJECT_FOUND || STATE_TRACKING_OBJECT) {
        returnThis = 1;
    }

    return returnThis;
}

void Controller::glRender() {
//    log(ControllerTAG, "rendering..");
//    log(ControllerTAG, "rendering.. done");
}

void Controller::glResize(int height, int width) {
//    log(ControllerTAG, "resizing..");
//    log(ControllerTAG, "resizing.. done");
}

int Controller::setDetector(std::string type) {
    int returnThis = 0;

    // save state and disable processing
    bool isActiveObjectRecognition = MODE_OBJECT_DETECTION;
    bool isActiveTracking = MODE_TRACKING;
    MODE_OBJECT_DETECTION = false;
    MODE_TRACKING = false;

    bool result = configure(type, Analyzer::EXTRACTOR, Analyzer::MATCHER);

    if (result) {
        returnThis = 1;
    }

    // return to state
    MODE_OBJECT_DETECTION = isActiveObjectRecognition;
    MODE_TRACKING = isActiveTracking;

    return returnThis;
}

int Controller::setExtractor(std::string type) {
    int returnThis = 0;

    // save state and disable processing
    bool isActiveObjectRecognition = MODE_OBJECT_DETECTION;
    bool isActiveTracking = MODE_TRACKING;
    MODE_OBJECT_DETECTION = false;
    MODE_TRACKING = false;

    bool result = Controller::configure(Analyzer::DETECTOR, type, Analyzer::MATCHER);
    if (result) {
        returnThis = 1;
    }

    // return to state
    MODE_OBJECT_DETECTION = isActiveObjectRecognition;
    MODE_TRACKING = isActiveTracking;

    return returnThis;;
}

int Controller::setMatcher(std::string type) {
    int returnThis = 0;

    // save state and disable processing
    bool isActiveObjectRecognition = MODE_OBJECT_DETECTION;
    bool isActiveTracking = MODE_TRACKING;
    MODE_OBJECT_DETECTION = false;
    MODE_TRACKING = false;

    bool result = Controller::configure(Analyzer::DETECTOR, Analyzer::EXTRACTOR, type);
    if (result) {
        returnThis = 1;
    }

    // return to state
    MODE_OBJECT_DETECTION = isActiveObjectRecognition;
    MODE_TRACKING = isActiveTracking;

    return returnThis;
}

void Controller::isModeObjectDetection(bool isActive) {
//    log(ControllerTAG, "MODE_OBJECT_DETECTION: %b", isActive);
    MODE_OBJECT_DETECTION = isActive;
}

void Controller::isModeTracking(bool isActive) {
//    log(ControllerTAG, "MODE_TRACKING: %b", isActive);
    MODE_TRACKING = isActive;
}

void Controller::isModeOpenGL(bool isActive) {
//    log(ControllerTAG, "MODE_OPENGL: %b", isActive);
    MODE_OPENGL = isActive;
}

void Controller::isModeDebug(bool isActive) {
//    log(ControllerTAG, "MODE_OPENGL: %b", isActive);
    MODE_DEBUG = isActive;
}

void Controller::isModeStatistics(bool isActive) {
//    log(ControllerTAG, "MODE_OPENGL: %b", isActive);
    MODE_STATISTICS = isActive;
}

bool Controller::createObjectPattern(cv::Mat &rgb, cv::Mat &gray) {

    // register object pattern to analyzer
    return analyzer->createObjectPattern(gray);

}

bool Controller::configure(std::string detector, std::string extractor, std::string matcher) {

    // disable analyzer
    analyzer->isInitialized = false;

    // set values
    Analyzer::DETECTOR = detector;
    Analyzer::EXTRACTOR = extractor;
    Analyzer::MATCHER = matcher;

    // update analyzer
    bool returnThis = analyzer->initialize();

    if (MODE_DEBUG && returnThis) {
        cout << "Controller initialized [Detector=" << Analyzer::DETECTOR << ", Extractor" << Analyzer::EXTRACTOR <<
        ", Matcher" << Analyzer::MATCHER << "]" << endl;
    }

    return returnThis;
}

void Controller::statistics(std::string key, int value) {
    stringstream sstr;
    sstr << value;
    stats->add(key, sstr.str());
}

void Controller::statistics(std::string key, double value) {
    stringstream sstr;
    sstr << value;
    stats->add(key, sstr.str());
}

void Controller::statistics(std::string key, long unsigned int value) {
    stringstream sstr;
    sstr << value;
    stats->add(key, sstr.str());
}

void Controller::statistics(std::string key, std::string value) {
    stats->add(key, value);
}

void Controller::statistics(std::string key, bool value) {
    stats->add(key, value ? "true" : "false");
}

int Controller::test(int test, int quantifier) {

    Mat sceneRgbImageData, sceneGrayImageData, objectRgbImage, objectGrayImage;

    sceneRgbImageData = cv::imread(STORAGE_PATH + "/images/book_frame.jpg");
    if (sceneRgbImageData.empty()) {
        std::cout << "Scene image cannot be read" << std::endl;
        return 1;
    }

    objectRgbImage = cv::imread(STORAGE_PATH + "/images/book.jpg");
    if (objectRgbImage.empty()) {
        std::cout << "Object image cannot be read" << std::endl;
        return 2;
    }

    cvtColor(sceneRgbImageData, sceneGrayImageData, CV_RGB2GRAY);
    cvtColor(objectRgbImage, objectGrayImage, CV_RGB2GRAY);

    if (!isInitialized) {
        initialize(sceneRgbImageData, STORAGE_PATH + "/config/config.xml");
    }

    // set testing mode and save actual configuration
    bool wasObjectDetection = MODE_OBJECT_DETECTION;
    bool wasTracking = MODE_TRACKING;
    bool wasStatistics = MODE_STATISTICS;
    MODE_OBJECT_DETECTION = true;
    MODE_TRACKING = false;
    MODE_STATISTICS = true;

    if (Controller::MODE_DEBUG) {
        cout << "-----------------------------------------------------" << endl;
        cout << "-----------------------------------------------------" << endl;
        cout << " T E S T I N G   S U I T E" << endl;
        cout << "-----------------------------------------------------" << endl;
    }

    // some variables to control testing routine
    bool shouldQuit = false;
    int isRun = 0;
    int doRuns = quantifier;

    // default test configuration
    bool doSIFT = false;
    bool doFAST = false;
    bool doGFTT = false;
    bool doMSER = false;
    bool doDENSE = false;
    bool doSTAR = false;
    bool doSURF = false;
    bool doBRISK = false;
    bool doORB = false;
    bool doAKAZE = false;

    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;
    cout << "------" << test << "-----" << quantifier << endl;

    // trigger tests
    switch (test) {
        case 0:
            doSIFT = true;
            doFAST = true;
            doGFTT = true;
            doMSER = true;
            doDENSE = true;
            doSTAR = true;
            doSURF = true;
            doBRISK = true;
            doORB = true;
            doAKAZE = true;
            break;
        case 1:
            doSIFT = true;
            break;
        case 2:
            doFAST = true;
            break;
        case 3:
            doGFTT = true;
            break;
        case 4:
            doMSER = true;
            break;
        case 5:
            doDENSE = true;
            break;
        case 6:
            doSTAR = true;
            break;
        case 7:
            doSURF = true;
            break;
        case 8:
            doBRISK = true;
            break;
        case 9:
            doORB = true;
            break;
        case 10:
            doAKAZE = true;
            break;
        default:
            return 0;
    }

    do {

        // start first test run
        isRun++;

        // init variables
        Mat sceneRgbImage, sceneGrayImage;
        std::vector<std::vector<string>> testConfigurations;

        if (doSIFT) {
            //*********** SIFT BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"SIFT", "SIFT", "BF_NORM_L2"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "BRIEF", "BF_NORM_L2"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "ORB", "BF_NORM_L2"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "SURF", "BF_NORM_L2"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "BRISK", "BF_NORM_L2"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "FREAK", "BF_NORM_L2"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "AKAZE", "BF_NORM_L2"});

            //*********** SIFT FLANN Tests ***********//
//            testConfigurations.push_back(std::vector<string>{"SIFT", "SIFT", "FLANN_X"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "BRIEF", "FLANN_X"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "ORB", "FLANN_X"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "SURF", "FLANN_X"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "BRISK", "FLANN_X"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "FREAK", "FLANN_X"});
//            testConfigurations.push_back(std::vector<string>{"SIFT", "AKAZE", "FLANN_X"});
        }

        if (doFAST) {
            //*********** FAST BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"FAST", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"FAST", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"FAST", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"FAST", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"FAST", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"FAST", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"FAST", "AKAZE", "BF_NORM_L2"});

            //*********** FAST FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"FAST", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"FAST", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"FAST", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"FAST", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"FAST", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"FAST", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"FAST", "AKAZE", "FLANN_X"});
        }

        if (doGFTT) {
            //*********** GFTT BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"GFTT", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "AKAZE", "BF_NORM_L2"});

            //*********** GFTT FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"GFTT", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"GFTT", "AKAZE", "FLANN_X"});
        }

        if (doMSER) {
            //*********** MSER BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"MSER", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"MSER", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"MSER", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"MSER", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"MSER", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"MSER", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"MSER", "AKAZE", "BF_NORM_L2"});

            //*********** MSER FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"MSER", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"MSER", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"MSER", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"MSER", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"MSER", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"MSER", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"MSER", "AKAZE", "FLANN_X"});
        }

        if (doDENSE) {
            //*********** DENSE BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"DENSE", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "AKAZE", "BF_NORM_L2"});

            //*********** DENSE FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"DENSE", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"DENSE", "AKAZE", "FLANN_X"});
        }

        if (doSTAR) {
            //*********** STAR BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"STAR", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"STAR", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"STAR", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"STAR", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"STAR", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"STAR", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"STAR", "AKAZE", "BF_NORM_L2"});

            //*********** STAR FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"STAR", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"STAR", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"STAR", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"STAR", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"STAR", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"STAR", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"STAR", "AKAZE", "FLANN_X"});
        }

        if (doSURF) {
            //*********** SURF BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"SURF", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"SURF", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"SURF", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"SURF", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"SURF", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"SURF", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"SURF", "AKAZE", "BF_NORM_L2"});

            //*********** SURF FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"SURF", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"SURF", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"SURF", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"SURF", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"SURF", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"SURF", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"SURF", "AKAZE", "FLANN_X"});
        }

        if (doBRISK) {
            //*********** BRISK BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"BRISK", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "AKAZE", "BF_NORM_L2"});

            //*********** BRISK FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"BRISK", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"BRISK", "AKAZE", "FLANN_X"});
        }

        if (doORB) {
            //*********** ORB BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"ORB", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"ORB", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"ORB", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"ORB", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"ORB", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"ORB", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"ORB", "AKAZE", "BF_NORM_L2"});

            //*********** ORB FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"ORB", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"ORB", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"ORB", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"ORB", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"ORB", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"ORB", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"ORB", "AKAZE", "FLANN_X"});
        }

        if (doAKAZE) {
            //*********** AKAZE BF Tests ***********//
            testConfigurations.push_back(std::vector<string>{"AKAZE", "SIFT", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "BRIEF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "ORB", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "SURF", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "BRISK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "FREAK", "BF_NORM_L2"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "AKAZE", "BF_NORM_L2"});

            //*********** AKAZE FLANN Tests ***********//
            testConfigurations.push_back(std::vector<string>{"AKAZE", "SIFT", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "BRIEF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "ORB", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "SURF", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "BRISK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "FREAK", "FLANN_X"});
            testConfigurations.push_back(std::vector<string>{"AKAZE", "AKAZE", "FLANN_X"});
        }

        for (auto &configuration : testConfigurations) {


            // clone images to clean previous drawings
            sceneRgbImage = sceneRgbImageData.clone();
            sceneGrayImage = sceneGrayImageData.clone();

            // configure controller
            configure(configuration.at(0), configuration.at(1), configuration.at(2));

            cout << "Testing.. [Detector=" << Analyzer::DETECTOR << ", Extractor=" << Analyzer::EXTRACTOR <<
            ", Matcher=" << Analyzer::MATCHER << "]" << endl;

            // (re-)create object pattern
            createObjectPattern(objectRgbImage, objectGrayImage);

            if (Controller::MODE_DEBUG) {
                cout << "::Run_" << isRun << "/" << doRuns << "::" << endl;
            }

            int result = displayFunction(sceneRgbImage, sceneGrayImage);

            if (result == 1) {
                    cout << "Result: Object found!" << endl;
                    cout << "-----------------------------------------------------" << endl;
            } else {
                    cout << "Result: Object NOT found!" << endl;
                    cout << "-----------------------------------------------------" << endl;
            }

        }

        if (isRun == doRuns) {
            shouldQuit = true;
        }

    } while (!shouldQuit);

    // restore last state
    isModeObjectDetection(wasObjectDetection);
    isModeTracking(wasTracking);
    isModeStatistics(wasStatistics);

    // success message
    if (MODE_DEBUG) {
        cout << "Tests finished successfull!" << endl;
        cout << "Results saved to statistics file.." << endl;
    }

    return 1;
}

#endif
