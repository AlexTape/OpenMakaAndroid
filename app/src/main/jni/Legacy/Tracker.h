#ifndef __TRACKING_OBJ__
#define __TRACKING_OBJ__

#include <opencv2/core/core.hpp>

namespace om {
    namespace track {

        class Tracker {
        protected:
            Tracker(void);

        public:
            virtual ~Tracker(void);

            typedef enum {
                TRACKER_KLT = 1, TRACKER_ORB = 2
            } TRACKER_TYPE;

        public:
            //! create track OBJ
            /*!
             \param[in] type tracker type
             \return pointer of tracker
             */
            static Tracker *create(TRACKER_TYPE type);

            //! Start Tracking
            /*!
             \param[in] grayImg first farme in gray scale
             \param[in] pts initial object position: pts[0]:Top Left, pts[1]:Bottom Left, pts[2]:Bottom Right, pts[3]:Top Right
             */
            virtual void startTracking(const cv::Mat &grayImg,
                                       std::vector<cv::Point2f> &pts) = 0;

            //! Continue Tracking
            /*!
             \param[in] grayImg input gray scale image
             \return false if track failed
             */
            virtual bool onTracking(const cv::Mat &grayImg) = 0;

            //! Get current obj position
            /*!
             \return Homography from previous frame
             */
            virtual cv::Mat &getHomographyMat() = 0;

            virtual void setHomographyMat(const cv::Mat &newHomographyMat) = 0;
        };

    };
};
#endif
