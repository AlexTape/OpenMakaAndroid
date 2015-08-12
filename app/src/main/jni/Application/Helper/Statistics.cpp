#ifndef OPENMAKAENGINE_STATISTICS_CPP
#define OPENMAKAENGINE_STATISTICS_CPP

#include <fstream>
#include "Statistics.h"
#include "../Controller.h"

using namespace std;
using namespace om;

const std::string Statistics::values[25] = {

        "Detector",
        "Extractor",
        "Matcher",

        "InputResolution",
        "ProcessingResolution",

        "DisplayFunction(ms)",
        "ObjectFound",

        "AnalyseSceneFrame(ms)",
        "AnalyzerProcess(ms)",

        "SceneKeypoints",
        "ObjectKeypoints",
        "GoodSceneKeypoints",
        "GoodTrainKeypoints",

        "Inliers",
        "Outliers",
        "InliersCalc(ms)",

        "DescriptorType",
        "MatchFloatDescriptors(ms)",
        "MatchBinaryDescriptors(ms)",

        "PerspectiveTransform(ms)",

        "TrackerInitialize(ms)",
        "TrackerCorners",
        "isInImage",
        "isObjectInsideImage(ms)",
        "TrackingProcess(ms)"

};

Statistics::Statistics(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Creating Statistics instance.." << endl;
    }
    wroteHeader = false;
}

Statistics::~Statistics(void) {
}

void Statistics::add(string key, string value) {
    stats[key] = value;
}

void Statistics::write(string filename) {
    ofstream file;
    if (!wroteHeader) {

        // overwrite existing file
        file.open(Controller::STORAGE_PATH + "/" + filename, ios::out);

        // print header line (csv)
        for (int i = 0; i < (sizeof(values)/sizeof(*values)); i++) {
            file << values[i] << ";";
        }

        // print linefeed
        file << endl;

        // set header state true
        wroteHeader = true;

    }

    // append to existing file
    file.open(Controller::STORAGE_PATH + "/" + filename, ios::app);

    // print all attribute keys
    // for (auto outer_iter = stats.begin(); outer_iter != stats.end(); ++outer_iter) {
        // used this to generate header line
        // cout << outer_iter->first << ";";
        // }
    // cout <<endl;

    // print value line (csv)
    for (int i = 0; i < (sizeof(values)/sizeof(*values)); i++) {
        file << stats[values[i]] << ";";
    }

    // print linefeed
    file << std::endl;
    file.close();
}

void Statistics::reset() {
    stats.clear();
}

#endif
