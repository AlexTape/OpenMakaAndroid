#ifndef __OR_EXCEPTION__
#define __OR_EXCEPTION__

#include <string>
#include <opencv2/core/core.hpp>

namespace om {

// General Exception
    class ControlException : public std::exception {
    public:
        ControlException(void);

        ControlException(std::string msg);

        virtual ~ControlException(void) throw() {
        };

        void setMessage(std::string msg);

    public:
        std::string message;
    };

// Exception related to OpenCV Functions
    class ProcessException : public ControlException {
    public:
        ProcessException(void);

        ProcessException(std::string msg);

        virtual ~ProcessException(void) throw() {
        };

        void setFunctionName(std::string name);

        void setCvExceptionClass(cv::Exception e);

    public:
        std::string cvfunction;
        cv::Exception cv_e;
    };

// Illegal Argument
    class ParameterException : public ControlException {
    public:
        ParameterException(void);

        ParameterException(std::string msg);

        virtual ~ParameterException(void) throw() {
        };

        void setFunctionName(std::string name);

    public:
        std::string function;
    };

};

#endif
