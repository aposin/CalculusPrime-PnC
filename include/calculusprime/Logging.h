/*
Copyright 2019 Association for the promotion of open - source insurance software and for the establishment of open interface standards in the insurance industry

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef CP_LOGGING_H
#define CP_LOGGING_H 1

#ifdef CALCULUSPRIME_NO_LOG4CPLUS
#include <string>

#define LOG4CPLUS_DEBUG(logger, logEvent)
#define LOG4CPLUS_TRACE_STR(logger, logEvent)
#define LOG4CPLUS_TRACE(logger, logEvent)

namespace log4cplus {

    static const int DEBUG_LOG_LEVEL = 0;
    inline void initialize()
    {
    }

    class Logger {
    public:
        static Logger getInstance(const std::string&) 
        {
            return logger;
        }
        static Logger getRoot() 
        {
            return logger;
        }

        bool isEnabledFor(int) const 
        {
            return false;
        }
    private:
        static Logger logger;
    };

    class NDCContextCreator {
    public:
        NDCContextCreator(const std::string&)
        {
        }
    };
}

#else 
#   include <log4cplus/logger.h>
#   include <log4cplus/loggingmacros.h>
#   include <log4cplus/ndc.h>
#endif // #ifdef CALCULUSPRIME_NO_LOG4CPLUS

#endif // #ifndef CP_LOGGING_H
