#ifndef __CONTROL_OR__
#define __CONTROL_OR__

#include <opencv2/features2d/features2d.hpp>

#include "VisualWords.h"
#include "ImageDB.h"
#include "ControlException.h"

namespace om {

    namespace orns {

        class ControlOR {
        public:
            ControlOR(void);

            ~ControlOR(void);

            // Visual Words + Feature Matching functions
            int addFeaturesForVW(const cv::Mat &src_img);

            int releaseFeaturesForVW();

            int createVisualWords(int cluster_number = 0);

            bool loadVisualWords(const std::string &filename);

            bool loadVisualWordsBinary(const std::string &filename,
                                       const std::string &idxname);

            bool saveVisualWords(const std::string &filename) const;

            bool saveVisualWordsBinary(const std::string &filename,
                                       const std::string &idxname) const;

            int releaseVisualWords();

            void setRecogThreshold(float th);

            float getRecogThreshold() const;

            std::string getDetectorType() const {
                return detectorType;
            };

            bool setDetectorType(const std::string &detector_type);

            std::string getDescriptorType() const {
                return descriptorType;
            };

            bool setDescriptorType(const std::string &descriptor_type);

            // ObjectDB functions
            int loadObjectDB(const std::string filename);

            void read(cv::FileNode &fn);

            int saveObjectDB(const std::string filename) const;

            void write(cv::FileStorage &fs, std::string name) const;

            void releaseObjectDB();

            // operateFunctions
            int registImage(const cv::Mat &src_img, int img_id);

            int removeImage(int image_id);

            std::vector<resultInfo> queryImage(const cv::Mat &src_img,
                                               int result_num = 1);

            imageDB image_db;

        private:
            // internal function
//	int getFeatureIdVec(const std::vector<float>& desc_vec, std::vector<int>& id_list);
            int getFeatureIdVec(const cv::Mat &desc_vec,
                                std::vector<int> &id_list);
//	int calcFeatureData(std::vector<cv::KeyPoint> kp_vec, std::vector<float> desc_vec, int img_id, std::vector<featureData>& img_data);

// feature detection
            int initializeFeatureDetector(); // initialize feature detector, and obtain number of descriptor dimention
            int extractFeatures(const cv::Mat &src_img,
                                std::vector<cv::KeyPoint> &kpt,
                                cv::Mat &descriptor) const;

//	int extractFeatures(const cv::Mat& src_img, std::vector<cv::KeyPoint>& kpt, std::vector<float>& descriptor);
            int releaseFeatureDetector();

//	void* featureDetector;
            std::string detectorType;    // feature detector name
            std::string descriptorType;    // feature descriptor name
//	std::string matcherType;	// feature matcher name
            cv::Ptr<cv::FeatureDetector> feature_detector;
            // feature detector
            cv::Ptr<cv::DescriptorExtractor> descriptor_extractor;
            // descriptor extractor
//	cv::Ptr<cv::DescriptorMatcher>	descriptor_matcher;	// descriptor matcher
//	int feature_dimention;	// dimention of descriptor
            int voteNum;    // number of knn & vote (to keypoint)
            VisualWords visual_words;
        };
    };
};
#endif
