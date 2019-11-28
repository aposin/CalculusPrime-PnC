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
#ifndef CP_IFUNCTIONRESULTCACHE_H
#define CP_IFUNCTIONRESULTCACHE_H 1

#include <string>
#include <boost/optional/optional.hpp>
#include <calculusprime/Value.h>

namespace CalculusPrime {

//! \brief Interface for getting/putting function results from/to the caches
class IFunctionResultCache
{
public:
    virtual ~IFunctionResultCache()
    {
    }

    //! \brief cache the function result
    //! \param insuranceRateKey_ the insurance rate key
    //! \param cacheKey_ the cache key
    //! \param result_ the function result
    virtual void putFunctionResult(const std::string& insuranceRateKey_, const std::string& cacheKey_, const Value& result_) = 0;

    //! \brief cache a function result with a validity date in ISO format. This is used for functions with different results depending on the date (e.g. tarrif date)
    //! \param insuranceRateKey_ the insurance rate key
    //! \param cacheKey_ the cache key
    //! \param validFrom_ valid from date
    //! \param validTo_ valid to date
    //! \param result_ the function result
    virtual void putFunctionResultWithValidityDate(const std::string& insuranceRateKey_, const std::string& cacheKey_, const std::string& validFrom_,
                                                   const std::string& validTo_, const Value& result_) = 0;

    //! \brief get the cached function result
    //! \param insuranceRateKey_ the insurance rate key
    //! \param cacheKey_ the cache key
    //! \return the cached value (if found) or an empty object
    virtual boost::optional<Value> getFunctionResult(const std::string& insuranceRateKey_, const std::string& cacheKey_) const = 0;

    //! \brief get the cached function result for the specified date in ISO format
    //! \param insuranceRateKey_ the insurance rate key
    //! \param date_ the date for which the result should be searched
    //! \param cacheKey_ the cache key
    //! \return the cached value (if found) or an empty object
    virtual boost::optional<Value> getFunctionResultWithValidityDate(const std::string& insuranceRateKey_, const std::string& date_, const std::string& cacheKey_) const = 0;

};

} // namespace CalculusPrime

#endif // #ifndef CP_IFUNCTIONRESULTCACHE_H
