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
#include <calculusprime/cache/DefaultCacheFactory.h>
#include <calculusprime/cache/DefaultFunctionResultCache.h>
#include <calculusprime/cache/DefaultParseTreeCache.h>

namespace CalculusPrime {

DefaultCacheFactory::FunctionResultCaches_t DefaultCacheFactory::m_functionResultCaches;
DefaultCacheFactory::ParseTreeCaches_t DefaultCacheFactory::m_parseTreeCaches;

std::shared_ptr<IFunctionResultCache> DefaultCacheFactory::getFunctionResultCache(const std::string& insuranceRateKey_)
{
    FunctionResultCaches_t::const_iterator it = m_functionResultCaches.find(insuranceRateKey_);
    if (it == m_functionResultCaches.end()) {
        it = m_functionResultCaches.emplace(insuranceRateKey_, std::make_shared<DefaultFunctionResultCache>()).first;
    }
    return it->second;
}

std::shared_ptr<IParseTreeCache> DefaultCacheFactory::getParseTreeCache(const std::string& insuranceRateKey_)
{
    ParseTreeCaches_t::const_iterator it = m_parseTreeCaches.find(insuranceRateKey_);
    if (it == m_parseTreeCaches.end()) {
        it = m_parseTreeCaches.emplace(insuranceRateKey_, std::make_shared<DefaultParseTreeCache>()).first;
    }
    return it->second;
}

void DefaultCacheFactory::clear()
{
    m_functionResultCaches.clear();
    m_parseTreeCaches.clear();
}

} // namespace CalculusPrime
