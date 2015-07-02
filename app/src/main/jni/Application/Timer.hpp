#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>
#include "opencv2/core.hpp"
#include "opencv2/core/utility.hpp"

#include "Helper.hpp"

#ifdef __cplusplus
extern "C" {
#endif

namespace OpenMaka
{

    class Timer
    {
        int64 startTime;

    public:

        double value;

        Timer()
        {
            reset();
        }

        void start()
        {
            startTime = cv::getTickCount();
        }

        void stop()
        {
            int64 stopTime = cv::getTickCount();
            value += ( (double)stopTime - startTime) / cv::getTickFrequency();
        }

        void reset()
        {
            value = 0;
        }

    };

}

#ifdef __cplusplus
}
#endif

#endif // TIMER_H_