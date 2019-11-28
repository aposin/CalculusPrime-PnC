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
#ifndef CP_DEFAULTPARSETREECACHE_H
#define CP_DEFAULTPARSETREECACHE_H 1

#include <unordered_map>
#include <calculusprime/cache/IParseTreeCache.h>

namespace CalculusPrime {

//! \brief The default parse tree cache implementation using std::unordered_map and no synchronization. It doesn't cache per insurance rate and therefore is not
//! suited as a single global instance
class DefaultParseTreeCache : public IParseTreeCache
{
public:
    virtual ~DefaultParseTreeCache();

    virtual void putParseTree(const std::string& insuranceRateKey_, const std::string& variableName_, const std::shared_ptr<void>& parseTree_) override;

    virtual std::shared_ptr<void> getParseTree(const std::string& insuranceRateKey_, const std::string& variableName_) const override;

private:
    typedef std::unordered_map<std::string, std::shared_ptr<void>> ParseTreeMap_t;

    ParseTreeMap_t m_parseTrees;
};

} // namespace CalculusPrime

#endif // #ifndef CP_DEFAULTPARSETREECACHE_H
