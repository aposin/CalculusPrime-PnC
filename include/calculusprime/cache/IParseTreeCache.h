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
#ifndef CP_IPARSETREECACHE_H
#define CP_IPARSETREECACHE_H 1

#include <memory>
#include <string>

namespace CalculusPrime {

//! \brief Interface for getting/putting parse trees from/to the caches.
class IParseTreeCache
{
public:
    virtual ~IParseTreeCache()
    {
    }

    //! \brief cache the parse tree object
    //! \param insuranceRateKey_ the insurance rate key
    //! \param cacheKey_ the cache key
    //! \param parseTree_ the parse tree object
    virtual void putParseTree(const std::string& insuranceRateKey_, const std::string& cacheKey_, const std::shared_ptr<void>& parseTree_) = 0;

    //! \brief get the cached parse tree object
    //! \param insuranceRateKey_ the insurance rate key
    //! \param cacheKey_ the cache key
    //! \return the cached parse tree (if found) or an empty object
    virtual std::shared_ptr<void> getParseTree(const std::string& insuranceRateKey_, const std::string& cacheKey_) const = 0;

};

} // namespace CalculusPrime

#endif // #ifndef CP_IPARSETREECACHE_H
