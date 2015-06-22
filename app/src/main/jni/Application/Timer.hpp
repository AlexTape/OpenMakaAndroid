#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>

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

        int value;

        Timer()
        {
            reset();
        }

        void start()
        {
            startTime = 0; //Helper::now_ms();
        }

        void stop()
        {
            int stopTime = 0; //Helper::now_ms();
            value += (stopTime - startTime);
        }

        int restart() {
            stop();
            int diff = value;
            start();
            return diff;
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