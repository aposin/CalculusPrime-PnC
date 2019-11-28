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
#include <calculusprime/internal/parsing/LazyValueHolder.h>
#include <calculusprime/internal/defs.h>
#include <calculusprime/Logging.h>
#include <calculusprime/internal/ParsingContext.h>
#include <calculusprime/internal/parsing/Parser.h>


namespace CalculusPrime {

boost::optional<Value> LazyValueHolder::getValue(const std::shared_ptr<ParsingContext>& parsingContext_)
{
    const std::string& instanceId = parsingContext_->getInstanceId();
    ValueHolder::ValueMap_t::const_iterator it = m_value->find(instanceId);
    if (it == m_value->end()) {
        // value has not been calculated yet
        log4cplus::NDCContextCreator ndc(m_variableName);
        Parser parser(parsingContext_);
        Value value = parser.parse(m_variableName, m_formula);
        (*m_value)[instanceId] = value;

        LOG4CPLUS_DEBUG(log4cplus::Logger::getInstance(LOGGER_CALCULUSPRIME), "result=" << value);

        if (instanceId.empty()) {
            // we put the result as static value into the context,
            // so it is not lazy any more
            // this cannot be done when multiple instances exist
            parsingContext_->assignParam(m_variableName, std::make_shared<ValueHolder>(m_value));
        }
        return value;
    }
    else {
        return it->second;
    }
}

} // namespace CalculusPrime
