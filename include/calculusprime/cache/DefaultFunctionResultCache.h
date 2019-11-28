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
#ifndef CP_DEFAULTFUNCTIONRESULTCACHE_H
#define CP_DEFAULTFUNCTIONRESULTCACHE_H 1

#include <set>
#include <unordered_map>
#include <tuple>
#include <calculusprime/Value.h>
#include <calculusprime/cache/IFunctionResultCache.h>

namespace CalculusPrime {

//! \brief The default function result cache implementation using std::unordered_map and no synchronization. It doesn't cache per insurance rate and therefore is not
//! suited as a single global instance
class DefaultFunctionResultCache : public IFunctionResultCache
{
public:
    virtual ~DefaultFunctionResultCache();

    virtual void putFunctionResult(const std::string& insuranceRateKey_, const std::string& cacheKey_, const Value& result_) override;

    virtual void putFunctionResultWithValidityDate(const std::string& insuranceRateKey_, const std::string& cacheKey_, const std::string& validFrom_,
                                                   const std::string& validTo_, const Value& result_) override;

    virtual boost::optional<Value> getFunctionResult(const std::string& insuranceRateKey_, const std::string& cacheKey_) const override;

    virtual boost::optional<Value> getFunctionResultWithValidityDate(const std::string& insuranceRateKey_, const std::string& date_, const std::string& cacheKey_) const override;

private:
    typedef std::unordered_map<std::string, Value> FunctionResultMap_t;
    typedef std::tuple<Value, std::string, std::string> ValueWithValidity_t;
    typedef std::unordered_map<std::string, std::set<ValueWithValidity_t>> FunctionResultWithValidityMap_t;

    static bool isValidFor(const ValueWithValidity_t& valueWithValidity_, const std::string& testDate_)
    {
        return std::get<1>(valueWithValidity_) <= testDate_ && std::get<2>(valueWithValidity_) >= testDate_;
    }

    FunctionResultMap_t m_functionResults;
    FunctionResultWithValidityMap_t m_functionsResultsWithValidity;
};

} // namespace CalculusPrime

#endif // #ifndef CP_DEFAULTFUNCTIONRESULTCACHE_H
