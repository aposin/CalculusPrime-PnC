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
#ifndef CP_PARSERHOLDER_H
#define CP_PARSERHOLDER_H 1

#include <memory>
#pragma push_macro("ERROR")
#undef ERROR
#include <antlr4-runtime.h>
#pragma pop_macro("ERROR")
#include "RatingEngineLexer.h"
#include "RatingEngineParser.h"
#include <calculusprime/internal/parsing/CollectingErrorListener.h>

namespace CalculusPrime {

class ParserHolder
{
public:
    ~ParserHolder()
    {
    }

    void parse(const std::string& formula_);

    RatingEngineParser::ParseContext* getParseTree()
    {
        return m_parseTree;
    }

    const std::vector<std::string>& getErrors() const;

private:
    std::unique_ptr<antlr4::ANTLRInputStream> m_inputStream;
    std::unique_ptr<RatingEngineLexer> m_lexer;
    std::unique_ptr<antlr4::CommonTokenStream> m_tokens;
    std::unique_ptr<RatingEngineParser> m_ratingEngineParser;
    std::unique_ptr<CollectingErrorListener> m_collectingErrorListener;
    RatingEngineParser::ParseContext* m_parseTree;
};

} // namespace CalculusPrime

#endif // #ifndef CP_PARSERHOLDER_H
