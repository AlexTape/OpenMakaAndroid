#ifndef TIMER_H_
#define TIMER_H_

#include <time.h>

#include "Helper.hpp"

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
            startTime = Helper::now_ms();
        }

        void stop()
        {
            int stopTime = Helper::now_ms();
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

#endif // TIMER_H_