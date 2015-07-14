#include "Tracker.h"
#include "TrackerKLT.h"

using namespace om;
using namespace om::track;

Tracker::Tracker(void) {
}

Tracker::~Tracker(void) {
}

Tracker *Tracker::create(TRACKER_TYPE type) {
    if (type == TRACKER_KLT) {
        return new TrackerKLT();
    }
    else {
        return 0;
    }
}
