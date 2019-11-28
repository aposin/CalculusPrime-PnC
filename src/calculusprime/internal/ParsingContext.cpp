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
#include <calculusprime/internal/ParsingContext.h>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/throw_exception.hpp>
#include <calculusprime/IFunction.h>
#include <calculusprime/RatingEngineException.h>
#include <calculusprime/FunctionIdBuilder.h>
#include <calculusprime/internal/parsing/ValueHolder.h>

namespace bal = boost::algorithm;


namespace CalculusPrime {

ParsingContext::~ParsingContext()
{
}

ParsingContext::ParsingContext(const std::shared_ptr<ParsingContext>& parent_)
    : m_parent(parent_)
    , m_functionResultCache(parent_->getFunctionResultCache())
    , m_parseTreeCache(parent_->getParseTreeCache())
    , m_insuranceRateKey(parent_->getInsuranceRateKey())
    , m_functions(parent_->getFunctions())
    , m_instanceId(parent_->getInstanceId())
    , m_logger(parent_->getLogger())
    , m_functionDurationsMicroSecs(0)
{
    for (const ValueHolderMap_t::value_type& variable : parent_->m_variables) {
        if (!variable.second->isFunctionArgument()) {
            m_variables[variable.first] = variable.second;
        }
    }
}

std::shared_ptr<IFunctionResultCache> ParsingContext::getFunctionResultCache() const
{
    return m_functionResultCache;
}

std::shared_ptr<IParseTreeCache> ParsingContext::getParseTreeCache() const
{
    return m_parseTreeCache;
}

const std::string& ParsingContext::getInsuranceRateKey() const
{
    return m_insuranceRateKey;
}

const std::string& ParsingContext::getCurrentRatingoutputVariable() const
{
    return m_currentRatingoutputVariable;
}

const std::string& ParsingContext::getInstanceId() const
{
    return m_instanceId;
}

void ParsingContext::addFunction(const std::shared_ptr<IFunction>& function_)
{
    m_functions[FunctionIdBuilder::createId(function_->getName(), function_->getNumberOfArgs())] = function_;
}

void ParsingContext::assignParam(const std::string& variableName_, const std::shared_ptr<IValueHolder>& valueHolder_)
{
    // convert var to lower case to support non-case-sensitive lookup
    m_variables[bal::to_lower_copy(variableName_)] = valueHolder_;
}

void ParsingContext::addCalculatedVariable(const std::string& variableName_, const Value& value_)
{
    const std::string varName(bal::to_lower_copy(variableName_));
    CalculatedVariableMap_t::iterator it = m_calculatedVariables.find(varName);
    if (it == m_calculatedVariables.end()) {
        std::shared_ptr<ValueMap_t> valueMap(std::make_shared<ValueMap_t>());
        (*valueMap)[getInstanceId()] = value_;
        m_calculatedVariables[varName] = valueMap;
        m_variables[varName] = std::make_shared<ValueHolder>(valueMap);
    }
    else {
        (*(it->second))[getInstanceId()] = value_;
    }
}

Value ParsingContext::resolve(const std::string& variable_)
{
    // convert var to lower case to support non-case-sensitive lookup
    ValueHolderMap_t::const_iterator it = m_variables.find(bal::to_lower_copy(variable_));
    if (it != m_variables.end()) {
        // The variable resides in this parsingContext
        boost::optional<Value> value = it->second->getValue(shared_from_this());
        if (!value) {
            BOOST_THROW_EXCEPTION(std::runtime_error("Could not find a value for variable " + variable_ + " with instance-id='" + getInstanceId() + "'"));
        }
        return *value;
    }
    else {
        BOOST_THROW_EXCEPTION(RatingEngineException(RatingEngineError::INPUT_PARAMETER_MISSING, "Parameter " + variable_ + " not defined"));
    }
}

} // namespace CalculusPrime
