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
#ifndef CP_RATINGENGINE_H
#define CP_RATINGENGINE_H 1

#include <vector>
#include <calculusprime/IRatingEngine.h>
#include <calculusprime/Logging.h>


namespace CalculusPrime {

class IFunction;
class IFunctionResultCache;
class IParseTreeCache;


class RatingEngine : public IRatingEngine
{
public:

    RatingEngine(const std::vector<std::shared_ptr<IFunction>>& functions_, const std::shared_ptr<IFunctionResultCache>& functionResultCache_, const std::shared_ptr<IParseTreeCache>& parseTreeCache_);

    virtual ~RatingEngine();

    virtual Map_t calculate(const std::string& insuranceRateKey_, const Map_t& input_, const RatingFormulaMap_t& ratingFormulas_, const std::vector<RatingOutput>& ratingOutput_) override;

private:
    const std::vector<std::shared_ptr<IFunction>> m_functions;
    const std::shared_ptr<IFunctionResultCache> m_functionResultCache;
    const std::shared_ptr<IParseTreeCache> m_parseTreeCache;
    const log4cplus::Logger m_logger;
};

} // namespace CalculusPrime

#endif // #ifndef CP_RATINGENGINE_H
