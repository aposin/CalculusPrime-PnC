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
#ifndef CP_DEFAULTCACHEFACTORY_H
#define CP_DEFAULTCACHEFACTORY_H 1

#include <memory>
#include <string>
#include <unordered_map>

namespace CalculusPrime {

class IFunctionResultCache;
class IParseTreeCache;
class DefaultFunctionResultCache;
class DefaultParseTreeCache;


//! \brief The default cache factory returns one cache instance per insurance rate key
class DefaultCacheFactory
{
public:
    //! \brief returns a function result cache instance for the given insurance rate key
    static std::shared_ptr<IFunctionResultCache> getFunctionResultCache(const std::string& insuranceRateKey_);

    //! \brief returns a parse tree cache instance for the given insurance rate key
    static std::shared_ptr<IParseTreeCache> getParseTreeCache(const std::string& insuranceRateKey_);

    //! \brief clear all caches
    static void clear();

private:
    DefaultCacheFactory();
    ~DefaultCacheFactory();

    typedef std::unordered_map<std::string, std::shared_ptr<DefaultFunctionResultCache>> FunctionResultCaches_t;
    typedef std::unordered_map<std::string, std::shared_ptr<DefaultParseTreeCache>> ParseTreeCaches_t;

    static FunctionResultCaches_t m_functionResultCaches;
    static ParseTreeCaches_t m_parseTreeCaches;
};

} // namespace CalculusPrime

#endif // #ifndef CP_DEFAULTCACHEFACTORY_H
