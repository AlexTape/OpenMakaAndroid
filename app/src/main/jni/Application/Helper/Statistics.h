#ifndef OPENMAKAENGINE_STATISTICS_H
#define OPENMAKAENGINE_STATISTICS_H

#include <iostream>
#include <map>

namespace om {

    class Statistics {

    private:

        std::map<std::string, std::string> stats;

    public:

        Statistics(void);

        virtual ~Statistics(void);

        void add(std::string key, std::string value);

        void write(std::string filename);
    };


};

#endif
