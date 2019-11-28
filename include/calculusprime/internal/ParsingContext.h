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
#ifndef CP_PARSINGCONTEXT_H
#define CP_PARSINGCONTEXT_H 1

#include <memory>
#include <vector>
#include <unordered_map>
#include <calculusprime/IParsingContext.h>
#include <calculusprime/Logging.h>
#include <calculusprime/Value.h>

namespace CalculusPrime {
class IFunction;
class IValueHolder;

//! \brief The IParsingContext implementation for the rating engine
class ParsingContext : public IParsingContext, public std::enable_shared_from_this<ParsingContext>
{
    typedef std::unordered_map<std::string, std::shared_ptr<IFunction>> FunctionMap_t;

public:
    ParsingContext(const std::vector<std::shared_ptr<IFunction>>& functions_, const std::shared_ptr<IFunctionResultCache>& functionResultCache_, const std::shared_ptr<IParseTreeCache> parseTreeCache_,
                   const std::string& insuranceRateKey_, const log4cplus::Logger& logger_)
        : m_functionResultCache(functionResultCache_)
        , m_parseTreeCache(parseTreeCache_)
        , m_insuranceRateKey(insuranceRateKey_)
        , m_logger(logger_)
        , m_functionDurationsMicroSecs(0)
    {
        for (const std::shared_ptr<IFunction>& function : functions_) {
            addFunction(function);
        }
    }

    ParsingContext(const std::shared_ptr<ParsingContext>& parent_);

    virtual ~ParsingContext();

    virtual std::shared_ptr<IFunctionResultCache> getFunctionResultCache() const override;

    virtual std::shared_ptr<IParseTreeCache> getParseTreeCache() const override;

    virtual const std::string& getInsuranceRateKey() const override;

    virtual const std::string& getCurrentRatingoutputVariable() const override;

    virtual const std::string& getInstanceId() const override;

    void setInstanceId(const std::string& instanceId_)
    {
        m_instanceId = instanceId_;
    }

    void setCurrentRatingoutputVariable(const std::string& currentRatingoutputVariable_)
    {
        m_currentRatingoutputVariable = currentRatingoutputVariable_;
    }

    //! \brief get the function implementation for the specified id. Returns an empty object if not found
    std::shared_ptr<IFunction> getFunction(const std::string& id_) const
    {
        FunctionMap_t::const_iterator it = m_functions.find(id_);
        return it != m_functions.end() ? it->second : std::shared_ptr<IFunction>();
    }

    const log4cplus::Logger& getLogger() const
    {
        return m_logger;
    }

    //! \brief add a function to this parsing context
    void addFunction(const std::shared_ptr<IFunction>& function_);

    //! \brief add a variable and its holder class to this parsing context
    void assignParam(const std::string& variableName_, const std::shared_ptr<IValueHolder>& valueHolder_);

    //! \brief add the calculated result for the variable to this parsing context
    void addCalculatedVariable(const std::string& variableName_, const Value& value_);

    //! \brief return sum of the function durations
    uint64_t getFunctionDurationsMicroSecs() const
    {
        return m_parent ? m_parent->getFunctionDurationsMicroSecs() : m_functionDurationsMicroSecs;
    }

    //! \brief add a function duration to the parent (if it exists) or th the own counter
    void addFunctionDurationMicroSecs(uint64_t microSecs_)
    {
        if (m_parent) {
            m_parent->addFunctionDurationMicroSecs(microSecs_);
        }
        else {
            m_functionDurationsMicroSecs += microSecs_;
        }
    }

    //! \brief resolve the value for the specified variable
    Value resolve(const std::string& variable_);

private:
    typedef std::unordered_map<std::string, Value> ValueMap_t;
    typedef std::unordered_map<std::string, std::shared_ptr<ValueMap_t>> CalculatedVariableMap_t;
    typedef std::unordered_map<std::string, std::shared_ptr<IValueHolder>> ValueHolderMap_t;

    const FunctionMap_t& getFunctions() const
    {
        return m_functions;
    }

    const std::shared_ptr<ParsingContext> m_parent;
    const std::shared_ptr<IFunctionResultCache> m_functionResultCache;
    const std::shared_ptr<IParseTreeCache> m_parseTreeCache;
    const std::string m_insuranceRateKey;
    FunctionMap_t m_functions;
    std::string m_instanceId;
    std::string m_currentRatingoutputVariable;
    CalculatedVariableMap_t m_calculatedVariables;
    ValueHolderMap_t m_variables;
    log4cplus::Logger m_logger;
    uint64_t m_functionDurationsMicroSecs;
};

} // namespace CalculusPrime

#endif // #ifndef CP_PARSINGCONTEXT_H
