#ifndef OPENMAKAENGINE_STATISTICS_CPP
#define OPENMAKAENGINE_STATISTICS_CPP

#include <fstream>
#include "Statistics.h"
#include "../Controller.h"

using namespace std;
using namespace om;

Statistics::Statistics(void) {
    if (Controller::MODE_DEBUG) {
        cout << "Creating Statistics instance.." << endl;
    }
}

Statistics::~Statistics(void) {
}

void Statistics::add(string key, string value) {
    cout << key << ":" << value << endl;
    stats[key] = value;
}

void Statistics::write(string filename) {
    ofstream file;
    file.open (Controller::STORAGE_PATH + "/example.txt");
    file << "Writing this to a file.\n";
    file << "Writing this to a file.\n";
    file << "Writing this to a file.\n";
    file << "Writing this to a file.\n";
    file.close();
}

#endif
