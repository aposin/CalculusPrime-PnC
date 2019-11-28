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
#ifndef CP_IFUNCTION_H
#define CP_IFUNCTION_H 1

#include <memory>
#include <vector>
#include <calculusprime/Value.h>

namespace CalculusPrime {

class IParsingContext;

//! \brief interface for business function implementations
class IFunction
{
public:
    virtual ~IFunction()
    {
    }

    //! \brief get the name of the function
    virtual const std::string& getName() const  = 0;

    //! \brief get the number of arguments for the function
    virtual std::size_t getNumberOfArgs() const  = 0;

    //! \brief execute the function with the specified input parameters and return the result value
    //! \param params_ the input parameters for the function
    //! \param parsingContext_ the IParsingContext object
    //! \return the calculated value
    virtual Value execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_) = 0;

};

} // namespace CalculusPrime

#endif // #ifndef CP_IFUNCTION_H
