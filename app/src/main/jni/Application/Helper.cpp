#include <string>
#include <iostream>

#include <time.h>

#include <opencv2/core/core.hpp>

#include "native_logger.h"

using namespace std;

namespace Helper {

    /* return current time in milliseconds */
    double now_ms(void)
    {
        struct timespec res;
        clock_gettime(CLOCK_REALTIME, &res);
        return 1000.0 * res.tv_sec + (double) res.tv_nsec / 1e6;
    }

    string type2str(int type) {
      string r;
      uchar depth = type & CV_MAT_DEPTH_MASK;
      uchar chans = 1 + (type >> CV_CN_SHIFT);
      switch ( depth ) {
        case CV_8U:  r = "8U"; break;
        case CV_8S:  r = "8S"; break;
        case CV_16U: r = "16U"; break;
        case CV_16S: r = "16S"; break;
        case CV_32S: r = "32S"; break;
        case CV_32F: r = "32F"; break;
        case CV_64F: r = "64F"; break;
        default:     r = "User"; break;
      }
      r += "C";
      r += (chans+'0');
      return r;
    }

}