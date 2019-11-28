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
#include <calculusprime/RatingEngineFactory.h>
#include <calculusprime/internal/RatingEngine.h>
#include <calculusprime/IFunction.h>

namespace CalculusPrime {

std::unique_ptr<IRatingEngine> RatingEngineFactory::createRatingEngine(const std::vector<std::shared_ptr<IFunction>>& functions_, const std::shared_ptr<IFunctionResultCache>& functionResultCache_, const std::shared_ptr<IParseTreeCache>& parseTreeCache_)
{
    if (!functionResultCache_) {
        BOOST_THROW_EXCEPTION(std::invalid_argument("parameter 'functionResultCache_' is null"));
    }
    if (!parseTreeCache_) {
        BOOST_THROW_EXCEPTION(std::invalid_argument("parameter 'parseTreeCache_' is null"));
    }

    return std::make_unique<RatingEngine>(functions_, functionResultCache_, parseTreeCache_);
}

} // namespace CalculusPrime
