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
#ifndef CP_RATINGENGINEEXCEPTION_H
#define CP_RATINGENGINEEXCEPTION_H 1

#include <exception>
#include <string>

namespace CalculusPrime {

enum class RatingEngineError {
    /** input parameter missing */
    INPUT_PARAMETER_MISSING,
    /** division by zero*/
    DIVISION_BY_ZERO,
    /** calculation cancelled */
    CALCULATION_CANCELLED,
    /** caller-specific */
    CALLER_SPECIFIC
};

inline std::ostream& operator<< (std::ostream& os_, const RatingEngineError& error_)
{
    switch (error_) {
    case RatingEngineError::INPUT_PARAMETER_MISSING:
        os_ << "INPUT_PARAMETER_MISSING";
        break;
    case RatingEngineError::DIVISION_BY_ZERO:
        os_ << "DIVISION_BY_ZERO";
        break;
    case RatingEngineError::CALCULATION_CANCELLED:
        os_ << "CALCULATION_CANCELLED";
        break;
    case RatingEngineError::CALLER_SPECIFIC:
        os_ << "CALLER_SPECIFIC";
        break;
    default:
        os_ << "unknown";
    }
    return os_;
}

//! \brief Rating engine exception class
class RatingEngineException : public std::runtime_error
{
public:
    RatingEngineException(const RatingEngineError errorType_)
        : RatingEngineException(errorType_, std::string(), std::string())
    {
    }

    RatingEngineException(const RatingEngineError errorType_, const std::string& message_)
        : RatingEngineException(errorType_, message_, std::string())
    {
    }

    RatingEngineException(const RatingEngineError errorType_, const std::string& message_, const std::string& addOn_)
        : std::runtime_error(message_)
        , m_errorType(errorType_)
        , m_addOn(addOn_)
    {
    }

    virtual ~RatingEngineException()
    {
    }

    //! \brief returns the error-type (RatingEngineError)
    CalculusPrime::RatingEngineError getErrorType() const
    {
        return m_errorType;
    }

    //! \brief returns the add-on text
    const std::string& getAddOn() const
    {
        return m_addOn;
    }

private:
    const RatingEngineError m_errorType;
    const std::string m_addOn;
};

} // namespace CalculusPrime

#endif // #ifndef CP_RATINGENGINEEXCEPTION_H
