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
#include <calculusprime/internal/parsing/ParserHolder.h>
#include <calculusprime/internal/require.h>

using namespace antlr4;

namespace CalculusPrime {

void ParserHolder::parse(const std::string& formula_)
{
    m_inputStream.reset(new ANTLRInputStream(formula_));
    m_lexer.reset(new RatingEngineLexer(m_inputStream.get()));
    m_tokens.reset(new CommonTokenStream(m_lexer.get()));
    m_ratingEngineParser.reset(new RatingEngineParser(m_tokens.get()));
    m_ratingEngineParser->setBuildParseTree(true);
    m_ratingEngineParser->setErrorHandler(std::make_shared<BailErrorStrategy>());
    m_ratingEngineParser->removeErrorListeners();

    m_collectingErrorListener.reset(new CollectingErrorListener());
    m_ratingEngineParser->addErrorListener(m_collectingErrorListener.get());

    m_parseTree = m_ratingEngineParser->parse();
}

const std::vector<std::string>& ParserHolder::getErrors() const
{
    CP_REQUIRE(m_collectingErrorListener);
    return m_collectingErrorListener->getErrors();
}

}
