#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <GLES/glext.h>

#define SIZE 1024
#define ControllerTAG "OpenMaka::Controller"

#ifdef __cplusplus
extern "C" {
#endif

class Controller {

    private:

        static Controller*  inst_;

        Controller(void);
        ~Controller(void);

        bool                isObjectDetection;
        bool                isTracking;
        bool                isOpenGL;

        GLuint              Name;
        GLubyte*            ImagePtr;
        double              featureFinished;

        int                 findFeatures(cv::Mat mRgbaFrame, cv::Mat mGrayFrame);

    public:

        static Controller*  getInstance();

        void                start();
        int                 initialize(cv::Mat& mGrayFrame, std::string configPath);
        int                 displayFunction(cv::Mat& mRgbaFrame, cv::Mat& mGrayFrame);
        void                glRender();
        void                glResize(int height, int width);

        void                setObjectDetection(bool isActive);
        void                setTracking(bool isActive);
        void                setOpenGL(bool isActive);

};

#ifdef __cplusplus
}
#endif

#endif // CONTROLLER_H_