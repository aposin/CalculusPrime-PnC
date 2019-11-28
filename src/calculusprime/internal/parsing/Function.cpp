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
#include <calculusprime/internal/parsing/Function.h>
#include <regex>
#include <sstream>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/throw_exception.hpp>
#include <calculusprime/internal/defs.h>
#include <calculusprime/FunctionIdBuilder.h>
#include <calculusprime/Logging.h>
#include <calculusprime/internal/ParsingContext.h>
#include <calculusprime/internal/parsing/FunctionArgument.h>
#include <calculusprime/internal/parsing/Parser.h>

namespace bal = boost::algorithm;

namespace {
const std::regex& getRegex()
{
    static const std::regex rgx("([a-zA-Z_]\\w*)\\s*\\(([^\\)]*)\\)");
    return rgx;
}
}

namespace CalculusPrime {

Function::Function(const std::string& header_, const std::string& implementation_)
    : m_implementation(implementation_)
{
    const std::regex& rgx(getRegex());
    std::smatch matches;
    if (std::regex_search(header_, matches, rgx)) {
        m_name = matches[1];
        std::string arguments(matches[2]);

        bal::split(m_arguments, arguments, [](char c) {
            return c == ',';
        });
        for (std::string& arg : m_arguments) {
            bal::trim(arg);
        }

        m_functionId = FunctionIdBuilder::createId(m_name, m_arguments.size());
    }
    else {
        BOOST_THROW_EXCEPTION(std::runtime_error("Invalid function header: " + header_));
    }
}

Function::~Function()
{
}

Value Function::execute(const std::vector<Value>& params_, const std::shared_ptr<IParsingContext>& parsingContext_)
{
    if (params_.size() != m_arguments.size()) {
        std::ostringstream msg;
        msg << "invalid number of arguments (" << params_.size() << ") for function " + m_name
            << ", expected: " << m_arguments.size();
        BOOST_THROW_EXCEPTION(std::runtime_error(msg.str()));
    }

    // create a copy of parsingContext_
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::static_pointer_cast<ParsingContext>(parsingContext_)));
    Parser parser(parsingContext);
    std::ostringstream oss;
    oss << m_name << '(';

    for (std::size_t i = 0; i < m_arguments.size(); ++i) {
        const Value& param = params_[i];
        parsingContext->assignParam(m_arguments[i], std::make_shared<FunctionArgument>(param));
        if (i != 0) {
            oss << ',';
        }
        oss << param;
    }
    oss << ')';
    log4cplus::NDCContextCreator ndc(oss.str());

    Value value = parser.parse(m_functionId, m_implementation);
    return value;
}

} // namespace CalculusPrime
