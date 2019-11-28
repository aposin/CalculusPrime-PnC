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
#ifndef CP_FUNCTION_H
#define CP_FUNCTION_H 1

#include <string>
#include <vector>
#include <calculusprime/IFunction.h>

namespace CalculusPrime {

class Function : public IFunction
{
public:
    Function(const std::string& header_, const std::string& implementation_);

    virtual ~Function();

    virtual const std::string& getName() const override
    {
        return m_name;
    }

    virtual std::size_t getNumberOfArgs() const override
    {
        return m_arguments.size();
    }

    virtual Value execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_) override;

private:
    const std::string m_implementation;
    std::string m_name;
    std::string m_functionId;
    std::vector<std::string> m_arguments;
};

} // namespace CalculusPrime

#endif // #ifndef CP_FUNCTION_H
