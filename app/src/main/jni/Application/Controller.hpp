#include <jni.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>

#include <android/native_window.h> // requires ndk r5 or newer

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <GLES/glext.h>

using namespace std;

#define SIZE 1024

class Controller {
    private:

    static Controller*  inst_;

    bool                isObjectDetection;
    bool                isTracking;
    bool                isOpenGL;

    GLuint              Name;
    GLubyte*            ImagePtr;
    double              featureFinished;

    string              type2str(int type);
    int                 findFeatures(cv::Mat mRgbaFrame, cv::Mat mGrayFrame);

    public:

    static Controller*  getInstance();
    static double       now_ms(void);

    void                start();
    bool                initialize(cv::Mat& mGrayFrame, const char& configPath);
    int                 displayFunction(cv::Mat& mRgbaFrame, cv::Mat& mGrayFrame);
    void                glRender();
    void                glResize(int height, int width);

    void                setObjectDetection(bool isActive);
    void                setTracking(bool isActive);
    void                setOpenGL(bool isActive);

};
