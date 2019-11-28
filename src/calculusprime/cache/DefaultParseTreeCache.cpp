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
#include <calculusprime/cache/DefaultParseTreeCache.h>

namespace CalculusPrime {

DefaultParseTreeCache::~DefaultParseTreeCache()
{
}

void DefaultParseTreeCache::putParseTree(const std::string& insuranceRateKey_, const std::string& cacheKey_, const std::shared_ptr<void>& parseTree_)
{
    m_parseTrees[cacheKey_] = parseTree_;
}

std::shared_ptr<void> DefaultParseTreeCache::getParseTree(const std::string& insuranceRateKey_, const std::string& cacheKey_) const
{
    std::shared_ptr<void> result;
    std::unordered_map<std::string, std::shared_ptr<void>>::const_iterator it = m_parseTrees.find(cacheKey_);
    if (it != m_parseTrees.end()) {
        result = it->second;
    }
    return result;
}

} // namespace CalculusPrime
