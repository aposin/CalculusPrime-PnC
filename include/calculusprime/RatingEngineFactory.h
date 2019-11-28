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
#ifndef CP_RATINGENGINEFACTORY_H
#define CP_RATINGENGINEFACTORY_H 1

#include <memory>
#include <vector>
#include <boost/noncopyable.hpp>

namespace CalculusPrime {

class IRatingEngine;
class IFunction;
class IFunctionResultCache;
class IParseTreeCache;


//! \brief Rating engine factory class for obtaining IRatingEngine instances
class RatingEngineFactory : private boost::noncopyable
{
public:
    //! \brief the factory function
    //! \param functions_ functions the list of business functions the should be supported
    //! \param functionResultCache_ the function result cache to use (may not be null)
    //! \param parseTreeCache_ the parse-tree cache to use (may not be null)
    //! \return a new IRatingeEngine instance
    static std::unique_ptr<IRatingEngine> createRatingEngine(const std::vector<std::shared_ptr<IFunction>>& functions_, const std::shared_ptr<IFunctionResultCache>& functionResultCache_, const std::shared_ptr<IParseTreeCache>& parseTreeCache_);
};

} // namespace CalculusPrime

#endif // #ifndef CP_RATINGENGINEFACTORY_H
