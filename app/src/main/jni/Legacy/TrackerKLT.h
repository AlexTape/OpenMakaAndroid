#ifndef __KLT_TRACKING_OBJ__
#define __KLT_TRACKING_OBJ__

#include <opencv2/core/core.hpp>
#include "Tracker.h"

namespace om {
    namespace track {

        class TrackerKLT : public Tracker {
        public:
            TrackerKLT(void);

            virtual ~TrackerKLT(void);

        public:
            cv::Mat prevImg;
            std::vector<cv::Point2f> corners;    // Tracking point
            std::vector<cv::Point2f> object_position;
            // Point of the four corners
            std::vector<unsigned char> track_status;
            // Mask of the point that you are able to track
            int max_corners;
            double quality_level;
            double min_distance;
            cv::Mat homographyMat;

        public:
            //! Start Tracking
            /*!
             \param[in] grayImg first farme in gray scale
             \param[in] pts initial object position: pts[0]:Top Left, pts[1]:Bottom Left, pts[2]:Bottom Right, pts[3]:Top Right
             */
            void startTracking(const cv::Mat &grayImg,
                               std::vector<cv::Point2f> &pts);

            //! Continue Tracking
            /*!
             \param[in] grayImg input gray scale image
             \return false if track failed
             */
            bool onTracking(const cv::Mat &grayImg);

            //! Get current obj position
            /*!
             \return Homography from previous frame
             */
            cv::Mat &getHomographyMat() {
                return homographyMat;
            }

            void setHomographyMat(const cv::Mat &newHomographyMat) {
                this->homographyMat = newHomographyMat;
            }

        };

    };
};
#endif
