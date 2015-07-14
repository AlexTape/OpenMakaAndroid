#include "ControlOR.h"
#include <opencv2/nonfree/nonfree.hpp>

#include <iostream>

using namespace std;
using namespace cv;
using namespace om;
using namespace om::orns;

ControlOR::ControlOR(void) {
//	featureDetector = 0;
    detectorType = "SURF";
    descriptorType = "SURF";
    feature_detector = 0;
    descriptor_extractor = 0;
    initializeFeatureDetector();
//	visual_words.setFeatureDimention(feature_dimention);
    voteNum = 1;
    visual_words.setVoteNum(voteNum);
    image_db.setVoteNum(voteNum);
}

ControlOR::~ControlOR(void) {
    releaseFeatureDetector();
}

/////// create visual word ///////
int ControlOR::addFeaturesForVW(const Mat &src_img) {
    vector<KeyPoint> kp_vec;
//	vector<float> desc_vec;
    cv::Mat desc_vec;

    extractFeatures(src_img, kp_vec, desc_vec);
    visual_words.addFeatures(desc_vec);

//	kp_vec.clear();
//	desc_vec.clear();

    return 0;
}

int ControlOR::createVisualWords(int cluster_number) {
    visual_words.createVW(cluster_number);

    return 0;
}

int ControlOR::registImage(const cv::Mat &src_img, int img_id) {
    vector<KeyPoint> kp_vec;
//	vector<float> desc_vec;
    cv::Mat desc_vec;

    if (img_id <= 0) return -1;

    try {
        extractFeatures(src_img, kp_vec, desc_vec);

        vector<int> id_list;
        getFeatureIdVec(desc_vec, id_list);
//		vector<featureData> img_data;
//		calcFeatureData(kp_vec,desc_vec,img_id, img_data);

        int ret = image_db.registImageFeatures(img_id, src_img.size(), kp_vec,
                                               id_list);

        if (ret < 0) return -1;
    } catch (cv::Exception &e) {
        cout << "Debug17\n";
        throw e;
    } catch (std::exception &e2) {
        cout << "Debug18\n";
        throw e2;
    }

    return 0;
}

int ControlOR::removeImage(int img_id) {
    return image_db.removeImageId(img_id);
}

void ControlOR::releaseObjectDB() {
    image_db.release();
}

vector<resultInfo> ControlOR::queryImage(const Mat &src_img, int result_num) {

    vector<resultInfo> retInfo;

    vector<KeyPoint> kp_vec;
//	vector<float> desc_vec;
    cv::Mat desc_vec;

    try {
        extractFeatures(src_img, kp_vec, desc_vec);

        vector<int> id_list;
        int ret = getFeatureIdVec(desc_vec, id_list);
        if (ret < 0) return retInfo;

        retInfo = image_db.retrieveImageId(kp_vec, id_list, src_img.size(),
                                           visual_words.getVisualWordNum(), result_num);

        kp_vec.clear();
        id_list.clear();
//		desc_vec.clear();
    } catch (cv::Exception &e) {
        cout << "Debug19\n";
        throw e;
    } catch (std::exception &e2) {
        cout << "Debug20\n";
        throw e2;
    }

    return retInfo;
}

bool ControlOR::setDetectorType(const std::string &detector_type) {
    cv::Ptr<cv::FeatureDetector> tmp_detector;
    try {
        tmp_detector = FeatureDetector::create(detector_type);
        if (tmp_detector.empty()) {
            return false;
        }
    } catch (cv::Exception &e) {
        cout << "Debug21\n";
        return false;
    }
    this->feature_detector = tmp_detector;
    this->detectorType = detector_type;

    return true;
}

bool ControlOR::setDescriptorType(const std::string &descriptor_type) {
    cv::Ptr<cv::DescriptorExtractor> tmp_descriptor;
    try {
        tmp_descriptor = DescriptorExtractor::create(descriptor_type);
        if (tmp_descriptor.empty()) {
            return false;
        }
    } catch (cv::Exception &e) {
        cout << "Debug22\n";
        return false;
    }
    this->descriptor_extractor = tmp_descriptor;
    this->descriptorType = descriptor_type;

    return true;
}

int ControlOR::getFeatureIdVec(const cv::Mat &desc_vec, vector<int> &id_list) {
    if (desc_vec.empty()) {
        return -1;
    }

    try {

        // query and obtain feature point indices
        Mat indices = visual_words.querySearchDB(desc_vec);

        int size = desc_vec.rows;

        int i, j;
        for (i = 0; i < size; i++) {
            for (j = 0; j < voteNum; j++) {
                id_list.push_back(indices.at<int>(i, j));
            }
        }
    } catch (std::exception &e) {
        cout << "Debug23\n";
        throw e;
    }

    return 0;
}

bool ControlOR::saveVisualWords(const string &filename) const {
    bool ret = visual_words.save(filename);
    return ret;
}

bool ControlOR::saveVisualWordsBinary(const string &filename,
                                      const string &idxname) const {
    bool ret = visual_words.saveBinary(filename, idxname);
    return ret;
}

bool ControlOR::loadVisualWords(const string &filename) {
    try {
        bool ret = visual_words.load(filename);
        return ret;
    } catch (std::exception &e) {
        cout << "Debug24\n";
        return false;
    }
}

bool ControlOR::loadVisualWordsBinary(const string &filename,
                                      const string &idxname) {
    try {
        bool ret = visual_words.loadBinary(filename, idxname);
        return ret;
    } catch (std::exception &e) {
        cout << "Debug25\n";
        return false;
    }
}

int ControlOR::loadObjectDB(const string filename) {
    try {
        FileStorage cvfs(filename, FileStorage::READ);
        FileNode cvfn(cvfs.fs, NULL);

        FileNode cvfn1 = cvfn["ControlOR"];
        read(cvfn1);

        FileNode cvfn2 = cvfn["imageDB"];
        image_db.read(cvfs, cvfn2);

        visual_words.setVoteNum(voteNum);
        image_db.setVoteNum(voteNum);
    } catch (std::exception &e2) {
        cout << "Debug26\n";
        throw e2;
    }
    return 0;
}

void ControlOR::read(FileNode &cvfn) {
    voteNum = cvfn["voteNum"];

    string detecType = cvfn["detectorType"];
    detectorType = detecType;

    string descType = cvfn["descriptorType"];
    descriptorType = descType;

    feature_detector->create(detectorType);
    descriptor_extractor->create(descriptorType);
}

int ControlOR::saveObjectDB(const string filename) const {
    FileStorage cvfs(filename, FileStorage::WRITE);
    write(cvfs, "ControlOR");
    image_db.write(cvfs, "imageDB");
    return 0;
}

void ControlOR::write(FileStorage &fs, string name) const {
    WriteStructContext ws(fs, name, CV_NODE_MAP);
    cv::write(fs, "voteNum", voteNum);
    cv::write(fs, "detectorType", detectorType);
    cv::write(fs, "descriptorType", descriptorType);
}

/////// Feature Detector ////////

// initialize
int ControlOR::initializeFeatureDetector() {
    if (feature_detector || descriptor_extractor) releaseFeatureDetector();
    cv::initModule_nonfree();
    feature_detector = FeatureDetector::create(detectorType); // create feature detector
    descriptor_extractor = DescriptorExtractor::create(descriptorType);    // create descriptor extractor
    return 0;
}

int ControlOR::extractFeatures(const cv::Mat &src_img,
                               cv::vector<cv::KeyPoint> &kpt, cv::Mat &descriptor) const {
    // extract freak
    try {
        // keypoints detection from a query image
        feature_detector->detect(src_img, kpt);
        // descriptor extraction
        descriptor_extractor->compute(src_img, kpt, descriptor);
    } catch (cv::Exception &e) {
        cout << "Debug27\n";
        ProcessException or_e;
        or_e.setCvExceptionClass(e);
        or_e.setFunctionName("SURF()");

        throw or_e;
    }

    return 0;
}

int ControlOR::releaseFeatureDetector() {
    feature_detector.release();
    feature_detector = 0;
    descriptor_extractor.release();
    descriptor_extractor = 0;

    return 0;
}

void ControlOR::setRecogThreshold(float th) {
    image_db.setThreshold(th);
}

float ControlOR::getRecogThreshold() const {
    return image_db.getThreshold();
}
