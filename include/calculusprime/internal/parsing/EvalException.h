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
#ifndef CP_EVALEXCEPTION_H
#define CP_EVALEXCEPTION_H 1

#include <exception>
#include <sstream>
#include <ParserRuleContext.h>

namespace CalculusPrime {

class EvalException : public std::runtime_error
{
public:
    EvalException(antlr4::ParserRuleContext* ctx_)
        : std::runtime_error(std::string("Illegal expression: " + ctx_->getText()))
    {
    }

    EvalException(const std::string& msg_, antlr4::ParserRuleContext* ctx_)
        : std::runtime_error(makeMsg(msg_, ctx_))
    {
    }

    virtual ~EvalException()
    {
    }

private:
    static std::string makeMsg(const std::string& msg_, antlr4::ParserRuleContext* ctx_)
    {
        std::ostringstream msg;
        msg << msg_ << ", line:" << ctx_->getStart()->getLine() << ", position:" << ctx_->getStart()->getStartIndex();
        return msg.str();

    }
};

} // namespace CalculusPrime

#endif // #ifndef CP_EVALEXCEPTION_H
