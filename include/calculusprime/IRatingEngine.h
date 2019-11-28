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
#ifndef CP_IRATINGENGINE_H
#define CP_IRATINGENGINE_H 1

#include <unordered_map>
#include <string>
#include <vector>
#include <boost/variant.hpp>
#include <calculusprime/RatingOutput.h>

namespace CalculusPrime {

//! \brief The rating engine interface
class IRatingEngine
{
public:
    //! \brief the basic input and output types are string and double
    typedef boost::variant<double, std::string> Variant_t;

    //! \brief instance variables are variables that can exist multiple times within an instance tarrif hierarchy.
    //! the key is the instance-id, value the associated value
    //! for nested instance tarrifs the key consists of the instance-ids for the hierarchy separated by '@'
    typedef std::unordered_map<std::string, Variant_t> InstanceVariable_t;

    //! \brief an input/output value may be a basic type (Variant_t) or instance variable
    typedef boost::variant<Variant_t, InstanceVariable_t> Value_t;

    //! \brief Input/output map. Key is the variable name, value an basic type (Variant_t) or instance variable
    typedef std::unordered_map<std::string, Value_t> Map_t;

    //! \brief Rating formulas map. Key ist the name of the formula, value the formula
    typedef std::unordered_map<std::string, std::string> RatingFormulaMap_t;

    virtual ~IRatingEngine()
    {
    }

    //! \brief Calculates the results from the given input parameters
    //! \param insuranceRateKey_ the insurance rate key
    //! \param input_ key-value mapping. Values may be String or Double or InstanceVariables
    //! \param ratingFormulas_ the rating formulas
    //! \param ratingOutput_ the output variables to calculate. They are calculated by their sort order
    //! \return the calculated values as a map the variable names as keys and the appropriate values as string or double or instance variables
    //! \throws RatingEngineException
    virtual Map_t calculate(const std::string& insuranceRateKey_, const Map_t& input_, const RatingFormulaMap_t& ratingFormulas_, const std::vector<RatingOutput>& ratingOutput_) = 0;
};

} // namespace CalculusPrime

#endif // #ifndef CP_IRATINGENGINE_H
