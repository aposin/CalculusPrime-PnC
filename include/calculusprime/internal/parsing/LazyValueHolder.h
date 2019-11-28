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
#ifndef CP_LAZYVALUEHOLDER_H
#define CP_LAZYVALUEHOLDER_H 1

#include <memory>
#include <unordered_map>
#include <calculusprime/internal/parsing/IValueHolder.h>
#include <calculusprime/internal/parsing/ValueHolder.h>

namespace CalculusPrime {

class ParsingContext;

// value holder with lazy evaluation - value is calculated on demand
class LazyValueHolder : public IValueHolder
{
public:
    LazyValueHolder(const std::string& variableName_, const std::string& formula_)
        : m_value(std::make_shared<ValueHolder::ValueMap_t>())
        , m_variableName(variableName_)
        , m_formula(formula_)
    {
    }

    virtual ~LazyValueHolder()
    {
    }

    virtual boost::optional<Value> getValue(const std::shared_ptr<ParsingContext>& parsingContext_) override;

    virtual bool isFunctionArgument() const override
    {
        return false;
    }

private:
    std::shared_ptr<ValueHolder::ValueMap_t> m_value;
    const std::string m_variableName;
    const std::string m_formula;
};

} // namespace CalculusPrime

#endif // #ifndef CP_LAZYVALUEHOLDER_H
