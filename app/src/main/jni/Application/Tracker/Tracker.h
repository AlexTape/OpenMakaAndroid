#ifndef OPENMAKAENGINE_TRACKER_H
#define OPENMAKAENGINE_TRACKER_H

//! Big Thanks to Takuya Minagawa
//! source @https://github.com/takmin/OpenCV-Marker-less-AR
//! forked @https://github.com/AlexTape/OpenCV-Marker-less-AR

#include <opencv2/core/core.hpp>

namespace om {

    class Tracker {

    private:

        Tracker(void);

        static Tracker *inst_;

    public:

        virtual ~Tracker(void);

        cv::Mat lastImage;
        std::vector<cv::Point2f> corners;
        std::vector<cv::Point2f> objectPosition;
        std::vector<unsigned char> vstatus;

        static int MAX_CORNERS;
        static double QUALITY_LEVEL;
        static double MINIMUM_DISTANCE;

        cv::Mat homography;

        cv::Mat createMask(cv::Size img_size, std::vector<cv::Point2f> &pts);

        std::vector<cv::Point2f> calcAffineTransformPoints(std::vector<cv::Point2f> &pts_vec,
                                                           cv::Mat &transMat);

        bool isObjectInsideImage(cv::Size img_size, std::vector<cv::Point2f> &pts);

        bool isRectangle(std::vector<cv::Point2f> &rectanglePoints);

        int isInsideArea(std::vector<cv::Point2f> &points, std::vector<cv::Point2f> &corner_pts,
                         std::vector<unsigned char> &status);

        cv::Mat transPointVecToMatHom(std::vector<cv::Point2f> &pt_vec);

        void initialize(const cv::Mat &frame,
                        std::vector<cv::Point2f> &actualObjectPosition);

        bool process(const cv::Mat &sceneImage);

        static Tracker *getInstance();
    };

};

#endif
