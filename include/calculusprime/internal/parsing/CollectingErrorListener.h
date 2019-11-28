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
#ifndef CP_COLLECTINGERRORLISTENER_H
#define CP_COLLECTINGERRORLISTENER_H 1

#include <BaseErrorListener.h>

namespace CalculusPrime {

class CollectingErrorListener : public antlr4::BaseErrorListener
{
public:
    virtual ~CollectingErrorListener();

    virtual void syntaxError(antlr4::Recognizer* recognizer, antlr4::Token* offendingSymbol, size_t line, size_t charPositionInLine,
                             const std::string& msg, std::exception_ptr e) override;

    const std::vector<std::string>& getErrors() const
    {
        return m_errors;
    }
private:
    std::vector<std::string> m_errors;
};

} // namespace CalculusPrime

#endif // #ifndef CP_COLLECTINGERRORLISTENER_H
