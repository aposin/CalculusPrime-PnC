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
#ifndef CP_PARSER_H
#define CP_PARSER_H 1

#include <memory>
#include <string>
#include <calculusprime/Value.h>

namespace CalculusPrime {

class ParsingContext;

class Parser
{
public:
    Parser(const std::shared_ptr<ParsingContext>& parsingContext_)
        : m_parsingContext(parsingContext_)
    {
    }

    ~Parser();

    Value parse(const std::string& variableName_, const std::string& formula_);

private:
    const std::shared_ptr<ParsingContext> m_parsingContext;
};

} // namespace CalculusPrime

#endif // #ifndef CP_PARSER_H
