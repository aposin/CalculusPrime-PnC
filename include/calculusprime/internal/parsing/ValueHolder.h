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
#ifndef CP_VALUEHOLDER_H
#define CP_VALUEHOLDER_H 1

#include <memory>
#include <unordered_map>
#include <calculusprime/internal/parsing/IValueHolder.h>

namespace CalculusPrime {

class ParsingContext;

class ValueHolder : public IValueHolder
{
public:
    typedef std::unordered_map<std::string, Value> ValueMap_t;

    ValueHolder(const std::shared_ptr<ValueMap_t>& value_)
        : m_value(value_)
    {
    }

    virtual ~ValueHolder()
    {
    }

    virtual boost::optional<Value> getValue(const std::shared_ptr<ParsingContext>& parsingContext_) override;

    virtual bool isFunctionArgument() const override
    {
        return false;
    }

private:
    std::shared_ptr<ValueMap_t> m_value;
};

} // namespace CalculusPrime

#endif // #ifndef CP_VALUEHOLDER_H
