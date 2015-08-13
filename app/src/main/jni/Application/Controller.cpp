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

    // create global clock
    clock = new Timer();

    // create local timer
    timer = new Timer();

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

}

Controller::~Controller(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Deleting Controller instance.." << endl;
    }
    delete tracker;
    delete analyzer;
    delete sceneFrame;
    delete clock;
    delete timer;
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
    if (statistics == "true") {
        Controller::MODE_STATISTICS = true;
    } else {
        Controller::MODE_STATISTICS = false;
    }
    string debug = (string) storage["debugMode"];
    if (debug == "true") {
        Controller::MODE_DEBUG = true;
    } else {
        Controller::MODE_DEBUG = false;
    }
    string windows = (string) storage["useWindows"];
    if (windows == "true") {
        Controller::USE_WINDOWS = true;
    } else {
        Controller::USE_WINDOWS = false;
    }

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
        // Controller::statistics("Matcher", (string) Analyzer::MATCHER);
    }

    int returnThis = 0;

    // if object detection is enabled..
    if (MODE_OBJECT_DETECTION) {

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

    // TODO resize or crop here if needed

    // register object pattern to analyzer
    return analyzer->createObjectPattern(gray);

}

void Controller::configure(std::string detector, std::string extractor, std::string matcher) {

    // disable analyzer
    analyzer->isInitialized = false;

    // set values
    Analyzer::DETECTOR = detector;
    Analyzer::EXTRACTOR = extractor;
    Analyzer::MATCHER = matcher;

    // update analyzer
    analyzer->initialize();

    if (MODE_DEBUG) {
        cout << "Controller initialized [Detector=" << Analyzer::DETECTOR << ", Extractor" << Analyzer::EXTRACTOR <<
        ", Matcher" << Analyzer::MATCHER << "]" << endl;
    }

    // enable analyzer
    analyzer->isInitialized = true;

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

#endif
