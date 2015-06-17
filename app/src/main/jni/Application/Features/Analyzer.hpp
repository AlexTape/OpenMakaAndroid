#include <string>

#include <opencv2/features2d/features2d.hpp>

#define AnalyzerTAG "OpenMaka::Analyzer"

class Analyzer {

    private:

        static Analyzer*    inst_;

        Analyzer(void);
        ~Analyzer(void);

    public:

        static Analyzer*                getInstance(void);

        cv::Ptr<cv::FeatureDetector>    getDetector(std::string type);

};
