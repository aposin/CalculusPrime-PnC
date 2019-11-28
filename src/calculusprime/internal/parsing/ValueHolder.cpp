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
#include <calculusprime/internal/parsing/ValueHolder.h>
#include <calculusprime/internal/ParsingContext.h>

namespace CalculusPrime {

boost::optional<Value> ValueHolder::getValue(const std::shared_ptr<ParsingContext>& parsingContext_)
{
    const std::string& instanceId = parsingContext_->getInstanceId();
    ValueMap_t::const_iterator it = m_value->find(instanceId);
    if (it == m_value->end() && !instanceId.empty()) {
        // we are in an instance tree but have no value stored for
        // the current instanckey, so we need the default value
        it = m_value->find(std::string());
    }
    boost::optional<Value> result;
    if (it != m_value->end()) {
        result = it->second;
    }
    return result;
}

} // namespace CalculusPrime

