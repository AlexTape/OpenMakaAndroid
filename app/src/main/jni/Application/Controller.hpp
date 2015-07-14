#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifdef __ANDROID__
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <GLES/glext.h>
#endif

#define SIZE 1024
#define ControllerTAG "OpenMaka::Controller"

#ifdef __cplusplus
extern "C" {
#endif

#include "../Legacy/ControlOR.h"
#include "../Legacy/ImageDB.h"
#include "../Legacy/TrackerKLT.h"

class Controller {

private:

    static Controller *inst_;

    Controller(void);


    bool isObjectDetection;
    bool isTracking;
    bool isOpenGL;

    double featureFinished;
    clock_t lastRelease;

    int queryScale;
    int recognizedObjectId;

    int findFeatures(cv::Mat mRgbaFrame, cv::Mat mGrayFrame);

    int seq_id; // Sequence ID of track
    int wait_seq_id; // Sequence ID at the time of non-track
    int max_query_size; // Maximum query image size

    om::orns::ControlOR ctrlOR;
    cv::Mat query_image;

    std::vector<om::orns::resultInfo> recog_result;
    om::track::TrackerKLT *trckOBJ;    // Object track class

public:

    ~Controller(void);

    static Controller *getInstance();

    void start();

    int initialize(cv::Mat &mGrayFrame, std::string configPath);

    int displayFunction(cv::Mat &mRgbaFrame, cv::Mat &mGrayFrame);

    void glRender();

    void glResize(int height, int width);

    void setObjectDetection(bool isActive);

    void setTracking(bool isActive);

    void setOpenGL(bool isActive);

};

#ifdef __cplusplus
}
#endif

#endif // CONTROLLER_H_
