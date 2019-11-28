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
#ifndef CP_RATINGOUTPUT_H
#define CP_RATINGOUTPUT_H 1

#include <string>

namespace CalculusPrime {

//! \brief Rating output data tuple
class RatingOutput
{
public:
    //! \brief constructor
    //! \param variableName_ the variable name
    //! \param sortOrder_ the sort order - results are calculated by sort order
    //! \param formula_ the formula for the variable
    //! \param instanceId_ the instance id
    RatingOutput(const std::string& variableName_, int sortOrder_, const std::string& formula_, const std::string& instanceId_)
        : m_variableName(variableName_)
        , m_sortOrder(sortOrder_)
        , m_formula(formula_)
        , m_instanceId(instanceId_)
    {
    }

    //! \brief returns the variable name
    const std::string& getVariableName() const
    {
        return m_variableName;
    }

    //! \brief returns the sort order
    const int getSortOrder() const
    {
        return m_sortOrder;
    }

    //! \brief returns the formula for the variable
    const std::string& getFormula() const
    {
        return m_formula;
    }

    //! \brief returns the instance id
    const std::string& getInstanceId() const
    {
        return m_instanceId;
    }

private:
    std::string m_variableName;
    int m_sortOrder;
    std::string m_formula;
    std::string m_instanceId;
};

} // namespace CalculusPrime

#endif // #ifndef CP_RATINGOUTPUT_H
