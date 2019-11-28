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
#include <memory>
#include <boost/program_options.hpp>
#include <boost/test/included/unit_test_framework.hpp>
#include <boost/test/unit_test.hpp>
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <calculusprime/cache/DefaultCacheFactory.h>
#include <calculusprime/cache/DefaultFunctionResultCache.h>
#include <calculusprime/cache/DefaultParseTreeCache.h>
#include <calculusprime/internal/defs.h>
#include <calculusprime/internal/parsing/Function.h>
#include <calculusprime/internal/parsing/Parser.h>
#include <calculusprime/internal/parsing/ParsingException.h>
#include <calculusprime/internal/parsing/ValueHolder.h>
#include <calculusprime/internal/ParsingContext.h>
#include <calculusprime/internal/RatingEngine.h>
#include <calculusprime/RatingEngineException.h>
#include <calculusprime/Value.h>

using boost::unit_test_framework::test_suite;
using namespace CalculusPrime;

static const std::string g_dummyVtarKey("ee3812d0-8cf1-11e9-b475-0800200c9a66");
static const std::string g_dummy("dummy");

void testCalculation();
void testInstanceCalculation();
void testCalculationPerformance();

namespace {
std::string getUniqueName()
{
    static std::size_t counter = 0;
    return g_dummy + std::to_string(counter++);
}

void testParser(const std::shared_ptr<ParsingContext>& parsingContext_, const std::string& formula_, const Value& expected_)
{
    Parser parser(parsingContext_);
    Value value = parser.parse(getUniqueName(), formula_);
    BOOST_CHECK_EQUAL(expected_, value);
}

void assignParamToParsingContext(const std::shared_ptr<ParsingContext>& parsingContext_, const std::string& variableName_, const Value::Variant_t& variant_)
{
    std::shared_ptr<ValueHolder::ValueMap_t> valueMap(std::make_shared<ValueHolder::ValueMap_t>());
    (*valueMap)[std::string()] = Value(variant_);
    parsingContext_->assignParam(variableName_, std::make_shared<ValueHolder>(valueMap));
}

std::unique_ptr<Parser> getSimpleParser()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(DefaultCacheFactory::getFunctionResultCache(g_dummyVtarKey));
    std::shared_ptr<IParseTreeCache> parseTreeCache(DefaultCacheFactory::getParseTreeCache(g_dummyVtarKey));
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));

    return std::make_unique<Parser>(parsingContext);
}

std::unique_ptr<Parser> getParser(const std::string& variableName_, const Value::Variant_t& variant_)
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(DefaultCacheFactory::getFunctionResultCache(g_dummyVtarKey));
    std::shared_ptr<IParseTreeCache> parseTreeCache(DefaultCacheFactory::getParseTreeCache(g_dummyVtarKey));
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));
    assignParamToParsingContext(parsingContext, variableName_, variant_);

    return std::make_unique<Parser>(parsingContext);
}

void testParser(const std::string& formula_, const Value& expected_)
{
    Value value = getSimpleParser()->parse(getUniqueName(), formula_);
    BOOST_CHECK_EQUAL(expected_, value);
}

void testParser(const std::string& variableName_, const Value::Variant_t& variant_, const std::string& formula_, const Value& expected_)
{
    Value value = getParser(variableName_, variant_)->parse(getUniqueName(), formula_);
    BOOST_CHECK_EQUAL(expected_, value);
}

void testParserExpectRatingengineErrorType(const std::string& formula_, const RatingEngineError& expectedErrorType_)
{
    try {
        testParser(formula_, Value("dummy-expected exception!"));
        BOOST_ERROR("this code shouldn't have been reached");
    }
    catch (const RatingEngineException& ex) {
        BOOST_CHECK_EQUAL(ex.getErrorType(), expectedErrorType_);
    }
}

template <class ExceptionType>
void testParserExpectException(const std::string& formula_)
{
    BOOST_CHECK_THROW(testParser(formula_, Value("dummy-expected exception!")), ExceptionType);
}

void testInvalidDates(const std::string& functionName_)
{
    testParserExpectException<ParsingException>("return " + functionName_ + "('asdf')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016-01')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('aaaa-01-01')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016-bb-cc')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016-01-00')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016-01-99')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016-13-01')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016-00-01')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('0000-01-01')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('201-01-01')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016-1-01')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016-01-1')");
    testParserExpectException<ParsingException>("return " + functionName_ + "('2016 01 01')");
    // 2015-02-29 is not a leap year
    testParserExpectException<ParsingException>("return " + functionName_ + "('2015-02-29')");
}
}

void testValue()
{
    Value v1;
    BOOST_CHECK(v1.isVoid());

    std::string s;
    Value v2(s);
    BOOST_CHECK(!v2.isVoid());
    BOOST_CHECK(!v2.isBool());
    BOOST_CHECK(v2.isString());
    BOOST_CHECK(!v2.isDouble());

    bool b = true;
    Value v3(b);
    BOOST_CHECK(!v3.isVoid());
    BOOST_CHECK(v3.isBool());
    BOOST_CHECK(!v3.isString());
    BOOST_CHECK(!v3.isDouble());

    double d = 123.45;
    Value v4(d);
    BOOST_CHECK(!v4.isVoid());
    BOOST_CHECK(!v4.isBool());
    BOOST_CHECK(!v4.isString());
    BOOST_CHECK(v4.isDouble());
}

/**
* test if then else expression
*/
void testIfThenElseExpression()
{
    const std::string formula = "if (value1=1.0) then return 'A' else if (value1=2.0) then return 'B' else return 'C' end";

    testParser("value1", 1.0, formula, Value("A"));
    testParser("value1", 2.0, formula, Value("B"));
    testParser("value1", 3.0, formula, Value("C"));
}

/**
* test add expression
*/
void testAddExpression()
{
    testParser("return 2+2", Value(4.0));
    testParser("return 2+1", Value(3.0));
    testParser("return 2+0", Value(2.0));
    testParser("return 0+0", Value(0.0));
    testParser("return -0+0", Value(0.0));
    testParser("return -2+1", Value(-1.0));
    testParser("return 4+0.5", Value(4.5));
}

/**
* test subtract expression
*/
void testSubtractExpression()
{
    testParser("return 2-2", Value(0.0));
    testParser("return 2-1", Value(1.0));
    testParser("return 2-0", Value(2.0));
    testParser("return 4-0.5", Value(3.5));
    testParser("return 0-0", Value(0.0));
    testParser("return -0-0", Value(0.0));
    testParser("return -2-1", Value(-3.0));
}

/**
* test multiply expression
*/
void testMultiplyExpression()
{
    testParser("return 2*2", Value(4.0));
    testParser("return 2*1", Value(2.0));
    testParser("return 2*0", Value(0.0));
    testParser("return 2*-1", Value(-2.0));
    testParser("return 4*0.5", Value(2.0));
    testParser("return -2*2", Value(-4.0));
    testParser("return -2*-2", Value(4.0));
}

/**
* test divide expression
*/
void testDivideExpression()
{
    testParser("return 2/2", Value(1.0));
    testParser("return 2/1", Value(2.0));
    testParser("return 2/-1", Value(-2.0));
    testParser("return 4/0.5", Value(8.0));
    testParser("return -2/2", Value(-1.0));
    testParser("return -2/-2", Value(1.0));

    testParserExpectRatingengineErrorType("return 2/0", RatingEngineError::DIVISION_BY_ZERO);
}

/**
* test power expression
*/
void testPowerExpression()
{
    testParser("return 2^2", Value(4.0));
    testParser("return 2^1", Value(2.0));
    testParser("return 2^0", Value(1.0));
    testParser("return 2^-1", Value(0.5));
    testParser("return 4^0.5", Value(2.0));
}

/**
* test modulo expression
*/
void testModuloExpression()
{
    testParser("return 10 mod 7", Value(3.0));
    testParser("return 20 mod 7", Value(6.0));
    testParser("return 10 mod 10", Value(0.0));

    testParserExpectRatingengineErrorType("return 10 mod 0", RatingEngineError::DIVISION_BY_ZERO);
}

/**
* test round function
*/
void testRoundFunction()
{
    testParser("return rnd(3.456, 0)", Value(3.0));
    testParser("return rnd(3.456, 1)", Value(3.5));
    testParser("return rnd(3.456, 2)", Value(3.46));
    testParser("return rnd(333.456, -1)", Value(330.0));
    testParser("return rnd(333.456, -2)", Value(300.0));
}

/**
* test min function
*/
void testMinFunction()
{
    testParser("return min(1.0, 2.0)", Value(1.0));
    testParser("return min(1.0, 1.0)", Value(1.0));
    testParser("return min(2.0, 1.0)", Value(1.0));
    testParser("return min(1.0, -2.0)", Value(-2.0));
    testParser("return min(-2.0, 1.0)", Value(-2.0));
    testParser("return min(-2.0, -1.0)", Value(-2.0));
    testParser("return min(-1.0, -2.0)", Value(-2.0));
}

/**
* test error function
*/
void testErrorFunction()
{
    try {
        testParser("if (1 = 1) then error(123) else return 0.0 end", Value(123.0));
        BOOST_ERROR("this code shouldn't have been reached");
    }
    catch (RatingEngineException ex) {
        BOOST_CHECK_EQUAL(ex.getErrorType(), RatingEngineError::CALCULATION_CANCELLED);
        BOOST_CHECK_EQUAL("123", ex.getAddOn());
    }
}

/**
* test max function
*/
void testMaxFunction()
{
    testParser("return max(1.0, 2.0)", Value(2.0));
    testParser("return max(1.0, 1.0)", Value(1.0));
    testParser("return max(2.0, 1.0)", Value(2.0));
    testParser("return max(1.0, -2.0)", Value(1.0));
    testParser("return max(-2.0, 1.0)", Value(1.0));
    testParser("return max(-2.0, -1.0)", Value(-1.0));
    testParser("return max(-1.0, -2.0)", Value(-1.0));
}

/**
* test day function
*/
void testDayFunction()
{
    testParser("return day('2016-01-20')", Value(20.0));
    testParser("return day('0001-01-01')", Value(1.0));
    testParser("return day('2999-12-31')", Value(31.0));
    // leap year
    testParser("return day('2016-02-29')", Value(29.0));

    testInvalidDates("day");
}

/**
* test date functions in Austria for year 1980 (summer time started on 5.4. 23:59:59 with a leap to 6.4. 01:00. The time 00:00 did not exist)
*/
void testDateFunctionsAustriaYear1980()
{
    testParser("return day('1980-04-06')", Value(6));
    testParser("return month('1980-04-06')", Value(4));
    testParser("return year('1980-04-06')", Value(1980));
}

/**
* test month function
*/
void testMonthFunction()
{
    testParser("return month('2016-01-20')", Value(1.0));
    testParser("return month('0001-01-01')", Value(1.0));
    testParser("return month('2999-12-31')", Value(12.0));
    // leap year
    testParser("return month('2016-02-29')", Value(2.0));

    testInvalidDates("month");
}

/**
* test year function
*/
void testYearFunction()
{
    testParser("return year('2016-01-20')", Value(2016.0));
    testParser("return year('0001-01-01')", Value(1.0));
    testParser("return year('2999-12-31')", Value(2999.0));
    testParser("return year('9999-12-31')", Value(9999.0));
    // leap year
    testParser("return year('2016-02-29')", Value(2016.0));

    testInvalidDates("year");
}

/**
* test substr function
*/
void testSubstrFunction()
{
    testParser("return substr('abcdefg',1,1)", Value("a"));
    testParser("return substr('abcdefg',1,7)", Value("abcdefg"));
    testParser("return substr('abcdefg',1,99)", Value("abcdefg"));
    testParser("return substr('abcdefg',2,6)", Value("bcdefg"));
    testParser("return substr('abcdefg',2,2)", Value("bc"));
    testParserExpectException<ParsingException>("return substr('abcdefg',2,-1)");
    testParserExpectException<ParsingException>("return substr('abcdefg',1,0)");
    testParserExpectException<ParsingException>("return substr('abcdefg',0,6)");
    testParserExpectException<ParsingException>("return substr('abcdefg',-1,6)");
    testParserExpectException<ParsingException>("return substr('abcdefg',99,6)");
}

/**
* test line comments
*/
void testLineComments()
{
    testParser("return //comment \n substr('abcdefg',1,1)", Value("a"));
    testParser("return substr('abcdefg', //comment\n 1,1)", Value("a"));
    testParser("return //asdf \n substr('abcdefg',//\n 1,1)", Value("a"));
    testParser("return substr('ab//cdefg',1,9)", Value("ab//cdefg"));
    testParser("return substr('ab//cdefg',1,9)\n", Value("ab//cdefg"));
    testParser("return // /*comment \n substr('abcdefg',1,1)", Value("a"));
    testParser("return substr('abcdefg',1,1) \n//asdf", Value("a"));
    testParser("return 1//*2", Value(1.0));
}

/**
* test comments
*/
void testComments()
{
    testParser("return /* comment */ substr('abcdefg',1,1)", Value("a"));
    testParser("return substr('abcdefg', /*comment*/1,1)", Value("a"));
    testParser("return /* asdf \n comment */ substr('abcdefg', /*\n*/ 1,1)", Value("a"));
    testParser("return substr('ab/*cd*/g',1,9)", Value("ab/*cd*/g"));
    testParser("return /* //comment */ substr('abcdefg',1,1)", Value("a"));
}

/**
* test number to string conversion
*/
void testNumberToStringConversion()
{
    testParser("return 'A'+50.0", Value("A50"));
    testParser("return 'A'+50.5", Value("A50.5"));
    testParser("return 'A'+0", Value("A0"));
    testParser("return 'A'+0.0", Value("A0"));
    testParser("return 'A'+50.120", Value("A50.12"));
}

/**
* tests function defintion and calls
*/
void testSimpleFunction()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(std::make_shared<DefaultFunctionResultCache>());
    std::shared_ptr<IParseTreeCache> parseTreeCache(std::make_shared<DefaultParseTreeCache>());
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));

    assignParamToParsingContext(parsingContext, "faktor", 0.8);

    // argument names non case sensitive (input / Input)
    parsingContext->addFunction(std::make_shared<Function>("F_Rabatt(input)", "return Input*faktor"));

    testParser(parsingContext, "return F_Rabatt(1000)", Value(800.0));
    testParser(parsingContext, "return F_Rabatt(faktor)", Value(0.64));
    // function name non case sensitive
    testParser(parsingContext, "return f_rabatt(faktor)", Value(0.64));
}

/**
* tests handling of multiple functions with different signatures
*/
void testFunctionsWithSameNameAndDifferentSignatures()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(std::make_shared<DefaultFunctionResultCache>());
    std::shared_ptr<IParseTreeCache> parseTreeCache(std::make_shared<DefaultParseTreeCache>());
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));

    assignParamToParsingContext(parsingContext, "faktor", 0.8);

    parsingContext->addFunction(std::make_shared<Function>("F_Rabatt(input)", "return input*faktor"));
    parsingContext->addFunction(std::make_shared<Function>("F_Rabatt(input, input2)", "return input*input2*faktor"));

    testParser(parsingContext, "return F_Rabatt(1000)", Value(800.0));
    testParser(parsingContext, "return F_Rabatt(2000, 10)", Value(16000.0));
}

/**
* tests that a function can call other functions
*/
void testFunctionCallsOtherFunction()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(std::make_shared<DefaultFunctionResultCache>());
    std::shared_ptr<IParseTreeCache> parseTreeCache(std::make_shared<DefaultParseTreeCache>());
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));

    assignParamToParsingContext(parsingContext, "faktor", 0.8);

    parsingContext->addFunction(std::make_shared<Function>("F_Rabatt(input)", "return input*faktor"));
    parsingContext->addFunction(std::make_shared<Function>("F_Rabatt(input, input2)", "return F_Rabatt(input)*input2+input"));

    testParser(parsingContext, "return F_Rabatt(2000, 10)", Value(18000.0));
}

/**
* tests scope of function arguments
*/
void testFunctionArgumentScope()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(std::make_shared<DefaultFunctionResultCache>());
    std::shared_ptr<IParseTreeCache> parseTreeCache(std::make_shared<DefaultParseTreeCache>());
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));

    assignParamToParsingContext(parsingContext, "faktor", 0.8);

    // input2 is not visible here
    parsingContext->addFunction(std::make_shared<Function>("F_Rabatt(input)", "return input2*faktor"));
    parsingContext->addFunction(std::make_shared<Function>("F_Rabatt(input, input2)", "return F_Rabatt(input)*input2+input"));

    BOOST_CHECK_THROW(testParser(parsingContext, "return F_Rabatt(2000, 10)", Value(18000.0)), RatingEngineException);
}

/**
* tests a recursive function call
*/
void testRecursiveFunctionCall()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(std::make_shared<DefaultFunctionResultCache>());
    std::shared_ptr<IParseTreeCache> parseTreeCache(std::make_shared<DefaultParseTreeCache>());
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));

    parsingContext->addFunction(std::make_shared<Function>("factorial(input)", "if (input = 1) then return 1 else return input*factorial(input-1) end"));

    testParser(parsingContext, "return factorial(5)", Value(120.0));
}

/**
* tests bool expressions
*/
void testBoolExpression()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(std::make_shared<DefaultFunctionResultCache>());
    std::shared_ptr<IParseTreeCache> parseTreeCache(std::make_shared<DefaultParseTreeCache>());
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));

    parsingContext->addFunction(std::make_shared<Function>("testFunction(input)", "if (input) then return 'yes' else return 'no' end"));
    parsingContext->addFunction(std::make_shared<Function>("testFunction2(input)", "if (input = true) then return 'yes' else return 'no' end"));
    parsingContext->addFunction(std::make_shared<Function>("testFunction3(input)", "if (input = 'yes') then return true else return false end"));

    testParser(parsingContext, "return testFunction(true)", Value("yes"));
    testParser(parsingContext, "return testFunction(false)", Value("no"));

    testParser(parsingContext, "return testFunction2(true)", Value("yes"));
    testParser(parsingContext, "return testFunction2(false)", Value("no"));

    testParser(parsingContext, "return testFunction3('yes')", Value(true));
    testParser(parsingContext, "return testFunction3('no')", Value(false));
}

/**
* tests string expressions
*/
void testStringExpression()
{
    std::shared_ptr<IFunctionResultCache> functionResultCache(std::make_shared<DefaultFunctionResultCache>());
    std::shared_ptr<IParseTreeCache> parseTreeCache(std::make_shared<DefaultParseTreeCache>());
    std::shared_ptr<ParsingContext> parsingContext(std::make_shared<ParsingContext>(std::vector<std::shared_ptr<IFunction>>(), functionResultCache, parseTreeCache, g_dummyVtarKey, log4cplus::Logger::getRoot()));

    parsingContext->addFunction(std::make_shared<Function>("testFunction(input)", "if (input) then return '1' else return '0' end"));

    testParser(parsingContext, "return testFunction(true)", Value("1"));
    testParser(parsingContext, "return testFunction(false)", Value("0"));
}

/**
* test ceil function
*/
void testCeilFunction()
{
    testParser("return ceil(2.3)", Value(3.0));
    testParser("return ceil(3.8)", Value(4.0));
    testParser("return ceil(0.0)", Value(0.0));
    testParser("return ceil(-2.3)", Value(-2.0));
    testParser("return ceil(-3.8)", Value(-3.0));
}

/**
* test floor function
*/
void testFloorFunction()
{
    testParser("return floor(2.3)", Value(2.0));
    testParser("return floor(3.8)", Value(3.0));
    testParser("return floor(0.0)", Value(0.0));
    testParser("return floor(-2.3)", Value(-3.0));
    testParser("return floor(-3.8)", Value(-4.0));
}

/**
* test exp function
*/
void testExpFunction()
{
    testParser("return rnd(exp(5.0),6)", Value(148.413159));
    testParser("return rnd(exp(1.0),6)", Value(2.718282));
    testParser("return rnd(exp(-1.0),6)", Value(0.367879));
    // overflow
    testParserExpectException<ParsingException>("return exp(999999.0)");
}


test_suite* init_unit_test_suite(int argc, char* argv[])
{
    namespace bpo = boost::program_options;

    log4cplus::initialize();

    // Programmparameter
    std::string properties;

    bpo::options_description desc("test_calculusprime options");
    desc.add_options()
    ("properties,p", bpo::value<std::string>(&properties), "properties")
    ;

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);

    if (!properties.empty()) {
        log4cplus::PropertyConfigurator::doConfigure(properties);
    }
    else {
        log4cplus::Logger::getRoot().setLogLevel(log4cplus::WARN_LOG_LEVEL);
        log4cplus::Logger::getInstance(LOGGER_CALCULUSPRIME).setLogLevel(log4cplus::WARN_LOG_LEVEL);
    }

    test_suite* test = BOOST_TEST_SUITE("Calculusprime Test Suite");

    test->add(BOOST_TEST_CASE(&testValue));
    test->add(BOOST_TEST_CASE(&testCalculation));
    test->add(BOOST_TEST_CASE(&testInstanceCalculation));
    test->add(BOOST_TEST_CASE(&testCalculationPerformance));
    test->add(BOOST_TEST_CASE(&testIfThenElseExpression));
    test->add(BOOST_TEST_CASE(&testAddExpression));
    test->add(BOOST_TEST_CASE(&testSubtractExpression));
    test->add(BOOST_TEST_CASE(&testMultiplyExpression));
    test->add(BOOST_TEST_CASE(&testDivideExpression));
    test->add(BOOST_TEST_CASE(&testPowerExpression));
    test->add(BOOST_TEST_CASE(&testModuloExpression));
    test->add(BOOST_TEST_CASE(&testRoundFunction));
    test->add(BOOST_TEST_CASE(&testCeilFunction));
    test->add(BOOST_TEST_CASE(&testFloorFunction));
    test->add(BOOST_TEST_CASE(&testExpFunction));
    test->add(BOOST_TEST_CASE(&testMinFunction));
    test->add(BOOST_TEST_CASE(&testErrorFunction));
    test->add(BOOST_TEST_CASE(&testMaxFunction));
    test->add(BOOST_TEST_CASE(&testDayFunction));
    test->add(BOOST_TEST_CASE(&testDateFunctionsAustriaYear1980));
    test->add(BOOST_TEST_CASE(&testMonthFunction));
    test->add(BOOST_TEST_CASE(&testYearFunction));
    test->add(BOOST_TEST_CASE(&testSubstrFunction));
    test->add(BOOST_TEST_CASE(&testLineComments));
    test->add(BOOST_TEST_CASE(&testComments));
    test->add(BOOST_TEST_CASE(&testNumberToStringConversion));
    test->add(BOOST_TEST_CASE(&testSimpleFunction));
    test->add(BOOST_TEST_CASE(&testFunctionsWithSameNameAndDifferentSignatures));
    test->add(BOOST_TEST_CASE(&testFunctionCallsOtherFunction));
    test->add(BOOST_TEST_CASE(&testFunctionArgumentScope));
    test->add(BOOST_TEST_CASE(&testRecursiveFunctionCall));
    test->add(BOOST_TEST_CASE(&testBoolExpression));
    test->add(BOOST_TEST_CASE(&testStringExpression));

    return test;
}
