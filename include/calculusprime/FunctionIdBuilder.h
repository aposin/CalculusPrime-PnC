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
#ifndef CP_FUNCTIONIDBUILDER_H
#define CP_FUNCTIONIDBUILDER_H 1

#include <string>

namespace CalculusPrime {

//! \brief Utility class for building function ids
class FunctionIdBuilder
{
public:
    //! \brief create an id from the specified function name and number of arguments
    //! \param functionName
    //! \param numberOfArgs
    //! \return the function id string
    static std::string createId(const std::string& functionName_, std::size_t numberOfArgs_);

private:
    FunctionIdBuilder();

    ~FunctionIdBuilder();
};

} // namespace CalculusPrime

#endif // #ifndef CP_FUNCTIONIDBUILDER_H
