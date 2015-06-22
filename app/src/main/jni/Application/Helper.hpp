#ifndef HELPER_H_
#define HELPER_H_

#include <string>
#include <time.h>

#define HelperTAG "OpenMaka::Helper"

#ifdef __cplusplus
extern "C" {
#endif

namespace Helper {

    double      now_ms(void);
    std::string type2str(int type);

};

#ifdef __cplusplus
}
#endif

#endif // HELPER_H_