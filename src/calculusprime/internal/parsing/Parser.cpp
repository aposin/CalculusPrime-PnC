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
#include <calculusprime/internal/parsing/Parser.h>
#include <chrono>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/scope_exit.hpp>
#include <boost/throw_exception.hpp>
#include <calculusprime/Logging.h>
#include <calculusprime/internal/parsing/ParserHolder.h>
#include <calculusprime/internal/parsing/EvalVisitor.h>
#include <calculusprime/internal/parsing/ParsingException.h>
#include <calculusprime/internal/ParsingContext.h>
#include <calculusprime/RatingEngineException.h>
#include <calculusprime/cache/IParseTreeCache.h>

namespace bal = boost::algorithm;

namespace CalculusPrime {

Parser::~Parser()
{
}

Value Parser::parse(const std::string& variableName_, const std::string& formula_)
{
    typedef std::chrono::high_resolution_clock Clock_t;

    using namespace antlr4;

    log4cplus::Logger logger = m_parsingContext->getLogger();
    LOG4CPLUS_TRACE_STR(logger, "start parse " + variableName_ + " " + formula_);

    const std::chrono::time_point<Clock_t> startTime = Clock_t::now();

    BOOST_SCOPE_EXIT(&logger, &startTime, &variableName_) {
        const std::chrono::time_point<Clock_t> endTime = Clock_t::now();
        LOG4CPLUS_TRACE(logger, "end parse " << variableName_ << ' ' << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << "ms");
    }
    BOOST_SCOPE_EXIT_END

    try {
        std::shared_ptr<IParseTreeCache> cache = m_parsingContext->getParseTreeCache();
        const std::string& insuranceRateKey = m_parsingContext->getInsuranceRateKey();

        std::shared_ptr<ParserHolder> parserHolder = std::static_pointer_cast<ParserHolder>(cache->getParseTree(insuranceRateKey, variableName_));

        if (!parserHolder) {
            std::string formulaForStream(formula_);

            // Make sure the formula ends with a line feed. Otherwise a line comment '//' in the last line
            // is not recognized
            if (!bal::ends_with(formulaForStream, "\n")) {
                formulaForStream.append(1, '\n');
            }

            parserHolder.reset(new ParserHolder());

            try {
                parserHolder->parse(formulaForStream);
            }
            catch (const ParseCancellationException& ex) {
                try {
                    std::rethrow_if_nested(ex);
                }
                catch (const RecognitionException& nested) {
                    Token* token = nested.getOffendingToken();
                    std::ostringstream msg;
                    msg << "Error while parsing formula " << variableName_ << ": "
                        << "Parsing error at line " << token->getLine() << ':' << token->getCharPositionInLine() << ' ' << nested.what()
                        << " formula=" << formula_;
                    BOOST_THROW_EXCEPTION(ParsingException(msg.str()));
                }
                catch (const std::exception& nested) {
                    std::ostringstream msg;
                    msg << "Error while parsing formula " << variableName_ << ": " "ParseCancellationException: " << ex.what() << " nested: " << nested.what() << " formula=" << formula_;
                    BOOST_THROW_EXCEPTION(ParsingException(msg.str()));
                }
                throw;
            }
            catch (const std::exception& ex) {
                const std::vector<std::string>& errors = parserHolder->getErrors();
                std::ostringstream msg;
                msg << "Error while parsing formula " << variableName_ << ": " << (!errors.empty() ? errors[0] : std::string()) << ' ' << ex.what() << " formula=" << formula_;
                BOOST_THROW_EXCEPTION(ParsingException(msg.str()));
            }
            cache->putParseTree(insuranceRateKey, variableName_, parserHolder);
        }

        EvalVisitor visitor(m_parsingContext);
        try {
            visitor.visit(parserHolder->getParseTree());
        }
        catch (const Value& value) {
            return value;
        }
    }
    catch (const ParsingException&) {
        throw;
    }
    catch (const RatingEngineException&) {
        throw;
    }
    catch (const std::exception& ex) {
        BOOST_THROW_EXCEPTION(ParsingException("Error while parsing formula " + variableName_ + ": " + ex.what()
                                               + " formula=" + formula_));
    }
    BOOST_THROW_EXCEPTION(ParsingException("Calculating formula " + variableName_ + " did not return a result, formula=" + formula_));
}

} // namespace CalculusPrime
