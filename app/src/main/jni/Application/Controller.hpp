#include <string>

#include <opencv2/core/core.hpp>

#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES/gl.h>
#include <GLES/glext.h>

#define SIZE 1024
#define ControllerTAG "OpenMaka::Controller"

class Controller {

    private:

        static Controller*  inst_;

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
        bool                initialize(cv::Mat& mGrayFrame, const char& configPath);
        int                 displayFunction(cv::Mat& mRgbaFrame, cv::Mat& mGrayFrame);
        void                glRender();
        void                glResize(int height, int width);

        void                setObjectDetection(bool isActive);
        void                setTracking(bool isActive);
        void                setOpenGL(bool isActive);

};
