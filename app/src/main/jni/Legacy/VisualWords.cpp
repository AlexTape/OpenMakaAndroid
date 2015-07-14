#include "VisualWords.h"
#include "HelperFunctions.h"
#include "ControlException.h"
#include <opencv2/flann/flann.hpp>
#include "opencv2/nonfree/nonfree.hpp"
#include <fstream>

using namespace std;
using namespace cv;
using namespace cv::flann;
using namespace om;
using namespace om::orns;

VisualWords::VisualWords(void) {
//	searchDB = 0;
    this->matcherType = "FlannBased";
    this->descriptor_matcher = DescriptorMatcher::create(matcherType);
    voteNum = 1;
    radius = 0.2;
}

VisualWords::~VisualWords(void) {
    release();
}

void VisualWords::release() {
    descriptor_matcher->clear();
}

bool VisualWords::isReady() {
    if (descriptor_matcher.empty() || descriptor_matcher->empty()) {
        return false;
    }
    return true;
}

int VisualWords::getVisualWordNum() const {
    int num = 0;
    vector<Mat> mat_vec = descriptor_matcher->getTrainDescriptors();
    vector<Mat>::iterator itr, it_end;
    it_end = mat_vec.end();
    for (itr = mat_vec.begin(); itr != it_end; itr++) {
        num += itr->rows;
    }
    return num;
}

void VisualWords::setVoteNum(int vote_num) {
    voteNum = vote_num;
}

//int VisualWords::addFeatures(vector<float>& feature)
void VisualWords::addFeatures(const cv::Mat &feature) {
    vector<Mat> feat_vec;
    feat_vec.push_back(feature);
    descriptor_matcher->add(feat_vec);
}

void VisualWords::createVW(int cluster_num) {
    try {
        // if cluster_num <= 0 then feature vector index is created without clustering. 
        if (cluster_num > 0) {
            ///////// kemans clustering to create visual words ///////////////
            vector<Mat> feature_vec = descriptor_matcher->getTrainDescriptors();
            if (!feature_vec.empty() && feature_vec[0].type() == CV_32FC1) {
                Mat featureMat;
                convertFeatureMat(feature_vec, featureMat);

                Mat label(featureMat.rows, 1, CV_32SC1);
                Mat centroid(cluster_num, featureMat.cols, featureMat.type());

                kmeans(featureMat, cluster_num, label,
                       TermCriteria(TermCriteria::MAX_ITER, 10, 0.5), 2,
                       KMEANS_PP_CENTERS, centroid);

                descriptor_matcher->clear();
                descriptor_matcher->add(centroid);
            }
        }
        descriptor_matcher->train();
    } catch (cv::Exception &e) {
        cout << "Debug41\n";
        ProcessException ce;
        ce.setFunctionName("VisualWords::createVW");
        ce.setCvExceptionClass(e);
        throw ce;
    } catch (std::exception &e2) {
        cout << "Debug42\n";
        throw e2;
    }
}

void VisualWords::convertFeatureMat(const vector<cv::Mat> &feature,
                                    cv::Mat &featureMat) {
    if (feature.empty()) {
        cout << "Debug43\n";
        ParameterException e("Empty Input Feature");
        e.setFunctionName("VisualWords::convertFeatureMat()");
        throw e;
    }

    //int row = feature.size() / feature_dim;
    int row = 0;
    int feature_dim = feature[0].cols;
    vector<cv::Mat>::const_iterator itr;
    for (itr = feature.begin(); itr != feature.end(); itr++) {
        row += itr->rows;
        if (feature_dim != itr->cols) {
            cout << "Debug44\n";
            throw ParameterException("Illegal Size of Mat");
        }
    }
    featureMat.create(row, feature_dim, feature[0].type());

    unsigned char *feature_pt = featureMat.data;
    int num_bytes = featureMat.elemSize();
    int feature_size = feature.size();
    int total;
    for (int i = 0; i < feature_size; i++) {
        total = feature[i].total();
        memcpy(feature_pt, feature[i].data, total * num_bytes);
        feature_pt += total;
    }
}

///////// load & save /////////////
bool VisualWords::saveIndex(const string &filename) const {
    try {
        cv::FileStorage fs(filename, FileStorage::WRITE);
        writeIndex(fs, "VW_Index");
        return true;
    } catch (std::exception &e) {
        cout << "Debug45\n";
//		std::cerr << e.what() << std::endl;
        return false;
    }
}

void VisualWords::writeIndex(cv::FileStorage &FS,
                             const std::string &name) const {
    try {
        cv::WriteStructContext ws(FS, name, CV_NODE_MAP);
        cv::write(FS, "matcherType", matcherType);
        descriptor_matcher->write(FS);
    } catch (cv::Exception &e) {
        cout << "Debug46\n";
        throw e;
    }
}

bool VisualWords::loadIndex(const string &filename) {
    try {
        cv::FileStorage fs(filename, FileStorage::READ);
        readIndex(fs["VW_Index"]);
        return true;
    } catch (std::exception &e) {
        cout << "Debug47\n";
//		std::cerr << e.what() << std::endl;
        return false;
    }
}

void VisualWords::readIndex(const cv::FileNode &FN) {
    try {
        cv::read(FN["matcherType"], this->matcherType, "");
        this->descriptor_matcher = DescriptorMatcher::create(matcherType);
        descriptor_matcher->read(FN);
    } catch (cv::Exception &e) {
        cout << "Debug48\n";
        throw e;
    }
}

bool VisualWords::save(const string &filename) const {
    try {
        FileStorage FS(filename, FileStorage::WRITE);
        this->writeIndex(FS, "index");
        this->write(FS, "VisualWords");
        return true;
    } catch (std::exception &e) {
        cout << "Debug49\n";
        return false;
    }
}

bool VisualWords::saveBinary(const string &filename,
                             const string &idx_filename) const {
    try {
        // save index data;
        this->saveIndex(idx_filename);
        save_vw_binary(filename);
        return true;
    } catch (std::exception &e) {
        cout << "Debug50\n";
        return false;
    }
}

bool VisualWords::load(const string &filename) {
    try {
        FileStorage cvfs(filename, FileStorage::READ);
        this->readIndex(cvfs["index"]);
        this->read(cvfs["VisualWords"]);
        return true;
    } catch (cv::Exception &e) {
        cout << "Debug51\n";
        ProcessException orce;
        orce.setFunctionName("VisualWords::load()");
        orce.setCvExceptionClass(e);
        return false;
    } catch (std::exception &e2) {
        cout << "Debug52\n";
        return false;
    }
}

bool VisualWords::loadBinary(const string &filename,
                             const string &idx_filename) {
    try {
        bool ret;
        ret = loadIndex(idx_filename);
        if (!ret) {
            return ret;
        }

        ret = load_vw_binary(filename);
        return ret;
    } catch (cv::Exception &e) {
        cout << "Debug53\n";
        ProcessException orce;
        orce.setFunctionName("VisualWords::load()");
        orce.setCvExceptionClass(e);
        return false;
    } catch (std::exception &e2) {
        cout << "Debug54\n";
        return false;
    }
}

bool VisualWords::save_vw_binary(const string &filename) const {
    ofstream ofs(filename.c_str(), std::ios::binary);
    if (!ofs.is_open()) {
        cout << "Debug55\n";
//		throw new ParameterException("failed to open " + filename);
        return false;
    }
//	int type = DBdata.type();
    ofs.write((const char *) "vw", 2);

    // TODO ERROR HERE
    //ofs.write((const char*) (&version), sizeof(int));
    ofs.write((const char *) (&radius), sizeof(radius));
    vector<Mat> train_desc = descriptor_matcher->getTrainDescriptors();
    int num = train_desc.size();
    ofs.write((const char *) (&num), sizeof(num));
    vector<Mat>::iterator itr;
    for (itr = train_desc.begin(); itr != train_desc.end(); itr++) {
        writeMatBinary(ofs, *itr);
    }

    return true;
}

bool VisualWords::load_vw_binary(const string &filename) {
    ifstream ifs(filename.c_str(), std::ios::binary);
    if (!ifs.is_open()) {
        cout << "Debug56\n";
//		throw new ParameterException("failed to open " + filename);
        std::cerr << "failed to open " << filename << std::endl;
        return false;
    }
    char header[2];
    ifs.read(header, sizeof(header));
    if (memcmp(header, (const void *) "vw", 2) != 0) {
        cout << "Debug57\n";
//		throw new ParameterException("wrong format file " + filename);
        std::cerr << "wrong format file " << filename << std::endl;
        return false;
    }

    int ver;
    ifs.read((char *) (&ver), sizeof(int));
    if (ver != version) {
        cout << "Debug58\n";
//		throw new ParameterException("wrong version file: " + filename);
        std::cerr << "wrong version file: " << filename << std::endl;
        return false;
    }

    ifs.read((char *) (&radius), sizeof(radius));

    int num;
    ifs.read((char *) (&num), sizeof(num));
    descriptor_matcher->clear();
    for (int i = 0; i < num; i++) {
        Mat DBdata;
        readMatBinary(ifs, DBdata);
        vector<Mat> in_mat_vec;
        in_mat_vec.push_back(DBdata);
        descriptor_matcher->add(in_mat_vec);
    }

    return true;
}

//int VisualWords::write(CvFileStorage* cvfs, const char* name)
void VisualWords::write(FileStorage &fs, const string &name) const {
    WriteStructContext ws(fs, name, CV_NODE_MAP);
//	cv::write(fs, "feature_dim", feature_dim);
    cv::write(fs, "version", version);
    cv::write(fs, "radius", radius);

    vector<Mat> train_desc = descriptor_matcher->getTrainDescriptors();
    WriteStructContext ws2(fs, "TrainDescriptors", CV_NODE_SEQ);
    vector<Mat>::iterator itr;
    for (itr = train_desc.begin(); itr != train_desc.end(); itr++) {
        cv::write(fs, std::string(), *itr);
    }
}

//int VisualWords::read(CvFileStorage* cvfs, CvFileNode *node)
void VisualWords::read(const FileNode &node) {
//	int	i;

    try {
//		feature_dim = node["feature_dim"];
        int ver = node["version"];
        if (ver != version) {
            cout << "Debug59\n";
            throw new ParameterException("wrong version file");
        }
        radius = node["radius"];

        descriptor_matcher->clear();
        cv::FileNode data_node = node["TrainDescriptors"];
        cv::FileNodeIterator itr;
        cv::FileNodeIterator it_end = data_node.end();
        for (itr = data_node.begin(); itr != it_end; itr++) {
            cv::Mat DBdata;
            cv::read(*itr, DBdata);

            vector<Mat> mat_vec;
            mat_vec.push_back(DBdata);
            descriptor_matcher->add(mat_vec);
        }
    } catch (cv::Exception &e) {
        cout << "Debug60\n";
        ProcessException orce;
        orce.setFunctionName("VisualWords::read()");
        orce.setCvExceptionClass(e);
        throw orce;
    } catch (std::exception &e2) {
        cout << "Debug61\n";
        throw e2;
    }
}

Mat VisualWords::querySearchDB(const Mat &features) {
    int knn_size = voteNum + 1;
    int size = features.size().height;

    Mat indices(size, knn_size, CV_32SC1);
//		Mat dists(size, knn_size, CV_32FC1);

// search nearest descriptor in database
    vector<vector<DMatch> > match_idx;
    descriptor_matcher->knnMatch(features, match_idx, knn_size);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < voteNum; x++) {
            DMatch d_match(match_idx[y][x]);
            if (d_match.distance >= radius) {
                indices.at<int>(y, x) = -1;
            } else {
                indices.at<int>(y, x) = d_match.trainIdx;
            }
        }
    }

    return indices;
}

