#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp> // for homograp

#define AnalyzerTAG "OpenMaka::Analyzer"

class Analyzer {

    private:

        static Analyzer*    inst_;

        Analyzer(void);
        ~Analyzer(void);

        bool    initialize(cv::Mat mGrayFrame);
        void    match();

        cv::Ptr<cv::FeatureDetector>        detector;
        cv::Ptr<cv::DescriptorExtractor>    extractor;
        cv::Ptr<cv::DescriptorMatcher>      matcher;

        std::vector<cv::KeyPoint>   objectKeypoints;
        std::vector<cv::KeyPoint>   sceneKeypoints;
        cv::Mat                     objectDescriptors;
        cv::Mat                     sceneDescriptors;

        bool    isInitialized;
        bool    isComputed;

    public:

        static Analyzer*    getInstance();

        bool    initialized(cv::Mat mGrayFrame);
        int     compute(cv::Mat mRgbaFrame, cv::Mat mGrayFrame);
        void    setDetector(std::string type);
        void    setExtractor(std::string type);


};
#endif // ANALYZER_H_