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
#include <calculusprime/cache/DefaultFunctionResultCache.h>

namespace CalculusPrime {

DefaultFunctionResultCache::~DefaultFunctionResultCache()
{
}

void DefaultFunctionResultCache::putFunctionResult(const std::string& insuranceRateKey_, const std::string& cacheKey_, const Value& result_)
{
    m_functionResults[cacheKey_] = result_;
}

void DefaultFunctionResultCache::putFunctionResultWithValidityDate(const std::string& insuranceRateKey_, const std::string& cacheKey_, const std::string& validFrom_,
                                                                   const std::string& validTo_, const Value& result_)
{
    FunctionResultWithValidityMap_t::iterator it = m_functionsResultsWithValidity.find(cacheKey_);
    if (it == m_functionsResultsWithValidity.end()) {
        it = m_functionsResultsWithValidity.emplace(cacheKey_, std::set<ValueWithValidity_t>()).first;
    }
    it->second.emplace(result_, validFrom_, validTo_);
}

boost::optional<Value> DefaultFunctionResultCache::getFunctionResult(const std::string& insuranceRateKey_, const std::string& cacheKey_) const
{
    boost::optional<Value> result;
    FunctionResultMap_t::const_iterator it = m_functionResults.find(cacheKey_);
    if (it != m_functionResults.end()) {
        result = it->second;
    }
    return std::move(result);
}

boost::optional<Value> DefaultFunctionResultCache::getFunctionResultWithValidityDate(const std::string& insuranceRateKey_, const std::string& date_, const std::string& cacheKey_) const
{
    boost::optional<Value> result;
    FunctionResultWithValidityMap_t::const_iterator it = m_functionsResultsWithValidity.find(cacheKey_);
    if (it != m_functionsResultsWithValidity.end()) {
        for (const ValueWithValidity_t& valueWithValidity : it->second) {
            if (isValidFor(valueWithValidity, date_)) {
                return std::get<0>(valueWithValidity);
            }
        }
    }
    return std::move(result);
}

} // namespace CalculusPrime
