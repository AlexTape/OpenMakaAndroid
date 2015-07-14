#include "ControlException.h"

using namespace std;
using namespace om;

ControlException::ControlException(void) {
}

ControlException::ControlException(string msg) {
    message = msg;
}

void ControlException::setMessage(string msg) {
    message = msg;
}

// ProcessException
ProcessException::ProcessException(void) {
}

ProcessException::ProcessException(string msg) :
        ControlException(msg) {
}

void ProcessException::setFunctionName(string name) {
    cvfunction = name;
}

void ProcessException::setCvExceptionClass(cv::Exception e) {
    cv_e = e;
}

// ParameterException
ParameterException::ParameterException(void) {
}

ParameterException::ParameterException(string msg) :
        ControlException(msg) {
}

void ParameterException::setFunctionName(string name) {
    function = name;
}
