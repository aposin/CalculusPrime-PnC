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
#include <calculusprime/internal/RatingEngine.h>
#include <chrono>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/throw_exception.hpp>
#include <boost/variant/static_visitor.hpp>
#include <calculusprime/internal/parsing/Function.h>
#include <calculusprime/internal/parsing/Parser.h>
#include <calculusprime/internal/parsing/ValueHolder.h>
#include <calculusprime/internal/parsing/LazyValueHolder.h>
#include <calculusprime/internal/ParsingContext.h>
#include <calculusprime/internal/defs.h>
#include <calculusprime/internal/require.h>

namespace bal = boost::algorithm;

namespace CalculusPrime {

Value getValueFromVariant(const IRatingEngine::Variant_t& variant_)
{
    const double* d = boost::get<double>(&variant_);
    if (d != nullptr) {
        return Value(*d);
    }
    else {
        const std::string* s = boost::get<std::string>(&variant_);
        CP_REQUIRE(s != nullptr);
        return Value(*s);
    }
}

void fillVariableValueMap(std::shared_ptr<ValueHolder::ValueMap_t>& valueMap_, const IRatingEngine::Value_t& value_)
{
    const IRatingEngine::InstanceVariable_t* instanceVariable = boost::get<IRatingEngine::InstanceVariable_t>(&value_);
    if (instanceVariable != nullptr) {
        for (const IRatingEngine::InstanceVariable_t::value_type& instanceValue : *instanceVariable) {
            IRatingEngine::Variant_t variant = instanceValue.second;
            valueMap_->emplace(instanceValue.first, getValueFromVariant(instanceValue.second));
        }
    }
    else {
        const IRatingEngine::Variant_t* variant = boost::get<IRatingEngine::Variant_t>(&value_);
        CP_REQUIRE(variant != nullptr);
        valueMap_->emplace(std::string(), getValueFromVariant(*variant));
    }
}

inline bool isFunction(const std::string& str_)
{
    return str_.find('(') != std::string::npos && bal::ends_with(bal::trim_copy(str_), ")");
}

template <class ValueType>
class InsertVisitor
    : public boost::static_visitor<>
{
public:
    InsertVisitor(const std::string& key_, std::unordered_map<std::string, ValueType>& map_)
        : m_key(key_)
        , m_map(map_)
    {
    }

    void operator()(const double& d_) const
    {
        m_map[m_key] = d_;
    }

    void operator()(const std::string& str_) const
    {
        m_map[m_key] = str_;
    }

    void operator()(const bool& b_) const
    {
        m_map[m_key] = b_ ? "1" : "0";
    }

private:
    const std::string m_key;
    std::unordered_map<std::string, ValueType>& m_map;
};

void addValueToResult(const Value& value_, IRatingEngine::Map_t& result_, const RatingOutput& output_)
{
    const std::string& variableName = output_.getVariableName();
    const std::string& instanceId = output_.getInstanceId();

    if (instanceId.empty()) {
        // we have a simple result
        value_.applyVisitor(InsertVisitor<RatingEngine::Value_t>(variableName, result_));
    }
    else {
        // we have a result for a specific instance
        RatingEngine::Map_t::iterator it = result_.find(variableName);
        if (it == result_.end()) {
            IRatingEngine::InstanceVariable_t instanceVariable;
            value_.applyVisitor(InsertVisitor<RatingEngine::Variant_t>(instanceId, instanceVariable));
            result_.emplace(variableName, instanceVariable).first;
        }
        else {
            IRatingEngine::InstanceVariable_t* instanceVariable = boost::get<IRatingEngine::InstanceVariable_t>(&(it->second));
            if (instanceVariable != nullptr) {
                value_.applyVisitor(InsertVisitor<RatingEngine::Variant_t>(instanceId, *instanceVariable));
            }
            else {
                BOOST_THROW_EXCEPTION(std::runtime_error("Inconsistent result type! Expected InstanceVariable"));
            }
        }
    }
}


RatingEngine::RatingEngine(const std::vector<std::shared_ptr<IFunction>>& functions_, const std::shared_ptr<IFunctionResultCache>& functionResultCache_, const std::shared_ptr<IParseTreeCache>& parseTreeCache_)
    : m_functions(functions_)
    , m_functionResultCache(functionResultCache_)
    , m_parseTreeCache(parseTreeCache_)
    , m_logger(log4cplus::Logger::getInstance(LOGGER_CALCULUSPRIME))
{
}

RatingEngine::~RatingEngine()
{
}

RatingEngine::Map_t RatingEngine::calculate(const std::string& insuranceRateKey_, const Map_t& input_, const RatingFormulaMap_t& ratingFormulas_, const std::vector<RatingOutput>& ratingOutput_)
{
    typedef std::chrono::high_resolution_clock Clock_t;

    LOG4CPLUS_DEBUG(m_logger, "start calculate");

    try {
        // create parsing context
        std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(m_functions, m_functionResultCache, m_parseTreeCache, insuranceRateKey_, m_logger));

        // fill parsing context with input variables
        for (const Map_t::value_type& variable : input_) {
            std::shared_ptr<ValueHolder::ValueMap_t> valueMap(std::make_shared<ValueHolder::ValueMap_t>());
            fillVariableValueMap(valueMap, variable.second);
            parsingContext->assignParam(variable.first, std::make_shared<ValueHolder>(valueMap));
        }

        // fill parsing context with formulas which will be evaluated lazily
        for (const RatingFormulaMap_t::value_type& formula : ratingFormulas_) {
            if (isFunction(formula.first)) {
                parsingContext->addFunction(std::make_shared<Function>(formula.first, formula.second));
            }
            else {
                parsingContext->assignParam(formula.first, std::make_shared<LazyValueHolder>(formula.first, formula.second));
            }
        }

        // sort rating output by sort order
        std::vector<RatingOutput> ratingOutput(ratingOutput_);
        std::sort(ratingOutput.begin(), ratingOutput.end(), [](const RatingOutput & lhs_, const RatingOutput & rhs_) {
            return lhs_.getSortOrder() < rhs_.getSortOrder();
        });

        Parser parser(parsingContext);
        RatingEngine::Map_t result;

        LOG4CPLUS_DEBUG(m_logger, "start parsing");
        const std::chrono::time_point<Clock_t> startTime = Clock_t::now();

        // calculate every requested output variable
        for (const RatingOutput& output : ratingOutput) {
            const std::string& variableName = output.getVariableName();
            const std::string& instanceId = output.getInstanceId();
            parsingContext->setCurrentRatingoutputVariable(variableName);
            parsingContext->setInstanceId(instanceId);

            log4cplus::NDCContextCreator ndc((!instanceId.empty() ? instanceId + ' ' : std::string()) + variableName);

            Value value = parser.parse(variableName, output.getFormula());
            LOG4CPLUS_DEBUG(m_logger, "result=" << value);

            // add the calculated value to the parsing context
            parsingContext->addCalculatedVariable(variableName, value);
            // add the calculated value to the result container
            addValueToResult(value, result, output);
        }
        const std::chrono::time_point<Clock_t> endTime = Clock_t::now();
        LOG4CPLUS_DEBUG(m_logger, "end parsing/calculate " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms, thereof function durations=" << parsingContext->getFunctionDurationsMicroSecs() / 1000 << "ms");

        return std::move(result);
    }
    catch (...) {
        LOG4CPLUS_DEBUG(m_logger, "end calculate");
        throw;
    }
}

} // namespace CalculusPrime
