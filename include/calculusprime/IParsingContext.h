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
#ifndef CP_IPARSINGCONTEXT_H
#define CP_IPARSINGCONTEXT_H 1

#include <memory>
#include <string>

namespace CalculusPrime {

class IFunctionResultCache;
class IParseTreeCache;


//! \brief Interface for obtaining context information during parsing
class IParsingContext
{
public:
    virtual ~IParsingContext()
    {
    }

    //! \brief returns the function result cache
    virtual std::shared_ptr<IFunctionResultCache> getFunctionResultCache() const = 0;

    //! \brief returns the parse-tree cache
    virtual std::shared_ptr<IParseTreeCache> getParseTreeCache() const = 0;

    //! \brief returns the insurance rate key
    virtual const std::string& getInsuranceRateKey() const = 0;

    //! \brief returns the current rating output variable name
    virtual const std::string& getCurrentRatingoutputVariable() const = 0;

    //! \brief returns the current instance key
    virtual const std::string& getInstanceId() const = 0;
};

} // namespace CalculusPrime

#endif // #ifndef CP_IPARSINGCONTEXT_H
