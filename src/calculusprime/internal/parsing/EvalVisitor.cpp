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
#define NOMINMAX
#include <calculusprime/internal/parsing/EvalVisitor.h>
#include <chrono>
#include <cfenv>
#include <regex>
#include <sstream>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/spirit/include/classic.hpp>
#include <boost/spirit/include/classic_chset.hpp>
#include <boost/throw_exception.hpp>
#include "RatingEngineParser.h"
#include <calculusprime/IFunction.h>
#include <calculusprime/Logging.h>
#include <calculusprime/RatingEngineException.h>
#include <calculusprime/FunctionIdBuilder.h>
#include <calculusprime/internal/ParsingContext.h>
#include <calculusprime/internal/parsing/EvalException.h>

namespace bgr = boost::gregorian;
namespace bsp = boost::spirit::classic;

namespace {
const std::string gDefaultDate("0001-01-01");

typedef bsp::uint_parser<unsigned, 10, 4, 4> uint4_parser_t;
typedef bsp::uint_parser<unsigned, 10, 2, 2> uint2_parser_t;

const uint4_parser_t g_uint4_p = uint4_parser_t();
const uint2_parser_t g_uint2_p = uint2_parser_t();
const bsp::uint_parser<unsigned> g_uint_p = bsp::uint_parser<unsigned>();

const std::regex& getRegex()
{
    static const std::regex rgx("\\\\(.)");
    return rgx;
}

// converts the string to date - ISO format is expected (YYYY-MM-DD)
boost::gregorian::date dateFromString(const std::string& str_)
{
    using namespace boost::spirit::classic;

    unsigned year, month, day;
    parse_info<> info = bsp::parse(str_.c_str(),
                                   (
                                       g_uint4_p[assign_a(year)] >> '-' >>
                                       g_uint2_p[assign_a(month)] >> '-' >>
                                       g_uint2_p[assign_a(day)]
                                   ));
    if (info.full) {
        return bgr::date(year, month, day);
    }
    else {
        BOOST_THROW_EXCEPTION(std::runtime_error("invalid date format: " + str_));
    }
}

inline bool compareChar(char left_, char right_)
{
    return left_ == right_ || std::toupper(left_) == std::toupper(right_);
}

inline bool equalsIgnCase(const std::string& left_, const std::string& right_)
{
    return ((left_.size() == right_.size()) &&
        std::equal(left_.begin(), left_.end(), right_.begin(), &compareChar));
}
}

namespace CalculusPrime {

EvalVisitor::~EvalVisitor()
{
}

antlrcpp::Any EvalVisitor::visitBlock(RatingEngineParser::BlockContext* ctx)
{
    // block
    // : ifStatement | (Return expression) | errorFunctionCall
    // ;
    RatingEngineParser::IfStatementContext* ifStmt;
    if ((ifStmt = ctx->ifStatement()) != nullptr) {
        visit(ifStmt);
    }

    RatingEngineParser::ExpressionContext* ex;
    if ((ex = ctx->expression()) != nullptr) {
        Value returnValue = visit(ex);
        throw returnValue;
    }

    RatingEngineParser::ErrorFunctionCallContext* errFunc;
    if ((errFunc = ctx->errorFunctionCall()) != nullptr) {
        visit(errFunc);
    }

    return Value();
}

antlrcpp::Any EvalVisitor::visitErrorFunctionCall(RatingEngineParser::ErrorFunctionCallContext* ctx)
{
    // Error '(' expression ')'                #errorFunctionCall

    Value code = visit(ctx->expression());
    if (!code.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'ERROR', number expected", ctx));
    }
    // cancel the entire calculation
    std::ostringstream errorCode;
    errorCode << code;
    BOOST_THROW_EXCEPTION(RatingEngineException(RatingEngineError::CALCULATION_CANCELLED, "Calculation was cancelled by a call of the error function with code " + errorCode.str(), errorCode.str()));
}

antlrcpp::Any EvalVisitor::visitMaxFunctionCall(RatingEngineParser::MaxFunctionCallContext* ctx)
{
    // Max '(' expression ',' expression ')'  #maxFunctionCall

    Value left = visit(ctx->expression(0));
    if (!left.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'MAX', number expected", ctx));
    }
    Value right = visit(ctx->expression(1));
    if (!right.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for second parameter of function 'MAX', number expected", ctx));
    }

    return Value(std::max(left.asDouble(), right.asDouble()));
}

antlrcpp::Any EvalVisitor::visitMinFunctionCall(RatingEngineParser::MinFunctionCallContext* ctx)
{
    // Min '(' expression ',' expression ')'  #minFunctionCall

    Value left = visit(ctx->expression(0));
    if (!left.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'MIN', number expected", ctx));
    }
    Value right = visit(ctx->expression(1));
    if (!right.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for second parameter of function 'MIN', number expected", ctx));
    }

    return Value(std::min(left.asDouble(), right.asDouble()));
}

antlrcpp::Any EvalVisitor::visitRoundFunctionCall(RatingEngineParser::RoundFunctionCallContext* ctx)
{
    // Rnd '(' expression ',' expression ')'   #roundFunctionCall

    Value value = visit(ctx->expression(0));
    if (!value.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'RND', number expected", ctx));
    }
    Value placesValue = visit(ctx->expression(1));
    if (!placesValue.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for second parameter of function 'RND', number expected", ctx));
    }

    double places = placesValue.asDouble();
    if (places >= 0) {
        int x = 1;
        for (double d = 0; d < places; d += 1.0) {
            x *= 10;
        }
        return Value(std::round(value.asDouble() * x) / x);
    }
    else {
        int x = 1;
        for (double d = 0; d > places; d -= 1.0) {
            x *= 10;
        }
        return Value(std::round(value.asDouble() / x) * x);
    }
}

antlrcpp::Any EvalVisitor::visitCeilFunctionCall(RatingEngineParser::CeilFunctionCallContext* ctx)
{
    // Ceil '(' expression ')'                 #ceilFunctionCall

    Value value = visit(ctx->expression());
    if (!value.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'CEIL', number expected", ctx));
    }
    return Value(std::ceil(value.asDouble()));
}

antlrcpp::Any EvalVisitor::visitFloorFunctionCall(RatingEngineParser::FloorFunctionCallContext* ctx)
{
    // Floor '(' expression ')'                #floorFunctionCall

    Value value = visit(ctx->expression());
    if (!value.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'FLOOR', number expected", ctx));
    }
    return Value(std::floor(value.asDouble()));
}

antlrcpp::Any EvalVisitor::visitExpFunctionCall(RatingEngineParser::ExpFunctionCallContext* ctx)
{
    // Exp '(' expression ')'                  #expFunctionCall

    Value value = visit(ctx->expression());
    if (!value.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'EXP', number expected", ctx));
    }
    errno = 0;
    double result = std::exp(value.asDouble());
    if (errno == ERANGE) {
        std::ostringstream msg;
        msg << "overflow range error in function 'exp' for value: " << value.asDouble();
        BOOST_THROW_EXCEPTION(EvalException(msg.str(), ctx));
    }
    return Value(result);
}

antlrcpp::Any EvalVisitor::visitDayFunctionCall(RatingEngineParser::DayFunctionCallContext* ctx)
{
    // Day '(' expression ')'                  #dayFunctionCall

    Value parm = visit(ctx->expression());
    if (!parm.isString()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'DAY', string expected", ctx));
    }

    const std::string& dateStr = parm.asString();
    // the boost datetime lib cannot handle dates < 1400, so make a hardcoded workaround for default date 0001-01-01
    if (dateStr == gDefaultDate || dateStr.empty()) {
        return Value(1);
    }

    try {
        bgr::date date = dateFromString(dateStr);
        return Value(double(date.day()));
    }
    catch (const std::exception&) {
        BOOST_THROW_EXCEPTION(EvalException("Cannot convert first parameter of function 'day' to date: " + parm.asString(), ctx));
    }
}

antlrcpp::Any EvalVisitor::visitMonthFunctionCall(RatingEngineParser::MonthFunctionCallContext* ctx)
{
    // Month '(' expression ')'                #monthFunctionCall

    Value parm = visit(ctx->expression());
    if (!parm.isString()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'MONTH', string expected", ctx));
    }

    const std::string& dateStr = parm.asString();
    // the boost datetime lib cannot handle dates < 1400, so make a hardcoded workaround for default date 0001-01-01
    if (dateStr == gDefaultDate || dateStr.empty()) {
        return Value(1);
    }

    try {
        bgr::date date = dateFromString(dateStr);
        return Value(double(date.month()));
    }
    catch (const std::exception&) {
        BOOST_THROW_EXCEPTION(EvalException("Cannot convert first parameter of function 'month' to date: " + parm.asString(), ctx));
    }
}

antlrcpp::Any EvalVisitor::visitYearFunctionCall(RatingEngineParser::YearFunctionCallContext* ctx)
{
    // Year '(' expression ')'                 #yearFunctionCall

    Value parm = visit(ctx->expression());
    if (!parm.isString()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'YEAR', string expected", ctx));
    }

    const std::string& dateStr = parm.asString();
    // the boost datetime lib cannot handle dates < 1400, so make a hardcoded workaround for default date 0001-01-01
    if (dateStr == gDefaultDate || dateStr.empty()) {
        return Value(1);
    }

    try {
        bgr::date date = dateFromString(dateStr);
        return Value(double(date.year()));
    }
    catch (const std::exception&) {
        BOOST_THROW_EXCEPTION(EvalException("Cannot convert first parameter of function 'year' to date: " + parm.asString(), ctx));
    }
}

antlrcpp::Any EvalVisitor::visitSubstrFunctionCall(RatingEngineParser::SubstrFunctionCallContext* ctx)
{
    // Substr '(' expression ',' expression ',' expression')'   #substrFunctionCall

    Value value = visit(ctx->expression(0));
    if (!value.isString()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'SUBSTR', string expected", ctx));
    }
    Value startParm = visit(ctx->expression(1));
    if (!startParm.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for second parameter of function 'SUBSTR', number expected", ctx));
    }
    Value lengthParm = visit(ctx->expression(2));
    if (!lengthParm.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for third parameter of function 'SUBSTR', number expected", ctx));
    }

    std::string str = value.asString();
    int start = static_cast<int>(startParm.asDouble() + 0.5);
    // 1-based start index
    if (start < 1) {
        BOOST_THROW_EXCEPTION(EvalException("Second parameter of function 'SUBSTR' must be >= 1", ctx));
    }

    int length = static_cast<int>(lengthParm.asDouble() + 0.5);
    if (length < 1) {
        BOOST_THROW_EXCEPTION(EvalException("Third parameter of function 'SUBSTR' must be >= 1", ctx));
    }

    try {
        return Value(str.substr(start - 1, length));
    }
    catch (const std::out_of_range&) {
        std::ostringstream msg;
        msg << "String index out of range in function 'SUBSTR('" << str << "', " << start << ", " << length << ")";
        BOOST_THROW_EXCEPTION(EvalException(msg.str(), ctx));
    }
}

antlrcpp::Any EvalVisitor::visitAddDaysFunctionCall(RatingEngineParser::AddDaysFunctionCallContext *ctx)
{

	Value dateParm = visit(ctx->expression(0));
	if (!dateParm.isString()) {
		BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'ADDDAYS', string expected", ctx));
	}
	Value daysParm = visit(ctx->expression(1));
	if (!daysParm.isDouble()) {
		BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for second parameter of function 'ADDDAYS', number expected", ctx));
	}

	const std::string& dateStr = dateParm.asString();

	try {
		bgr::date startDate = dateFromString(dateStr);
		bgr::date_duration days(static_cast<int>(daysParm.asDouble() + 0.5));
		bgr::date resultDate = startDate + days;

		return Value(bgr::to_iso_extended_string(resultDate));
	}
	catch (const std::exception&) {
		BOOST_THROW_EXCEPTION(EvalException("Cannot convert first parameter of function 'year' to date: " + dateParm.asString(), ctx));
	}

}

antlrcpp::Any EvalVisitor::visitGetDiffDaysFunctionCall(RatingEngineParser::GetDiffDaysFunctionCallContext *ctx)
{

	Value dateLeftParm = visit(ctx->expression(0));
	if (!dateLeftParm.isString()) {
		BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'GetDiffDays', string expected", ctx));
	}
	Value dateRightParm = visit(ctx->expression(1));
	if (!dateRightParm.isString()) {
		BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for second parameter of function 'GetDiffDays', number expected", ctx));
	}

	const std::string& dateLeftStr = dateLeftParm.asString();
	const std::string& dateRightStr = dateRightParm.asString();

	bgr::date dateLeft;
	bgr::date dateRight;

	try {
		dateLeft = dateFromString(dateLeftStr);
	}
	catch (const std::exception&) {
		BOOST_THROW_EXCEPTION(EvalException("Cannot convert first parameter of function 'GetDiffDays' to date: " + dateLeftParm.asString(), ctx));
	}

	try {
		dateRight = dateFromString(dateRightStr);
	}
	catch (const std::exception&) {
		BOOST_THROW_EXCEPTION(EvalException("Cannot convert second parameter of function 'GetDiffDays' to date: " + dateRightParm.asString(), ctx));
	}

	try {
		bgr::days diffDays = dateLeft - dateRight;

		if (diffDays.days() < 0) {
			BOOST_THROW_EXCEPTION(EvalException("Days is difference is less than zero in function 'GetDiffDays' with first parameter '" + 
				dateLeftParm.asString() + "' and second parameter '" + dateRightParm.asString() + "'", ctx));
		}

		return Value(double(diffDays.days()));
	}
	catch (const std::exception&) {
		BOOST_THROW_EXCEPTION(EvalException("Cannot calculate differnce in days in function 'GetDiffDays' with first parameter '" + 
			dateLeftParm.asString() + "' and second parameter '" + dateRightParm.asString() + "'", ctx));
	}

}

antlrcpp::Any EvalVisitor::visitPaddedStringFunctionCall(RatingEngineParser::PaddedStringFunctionCallContext *ctx)
{

	Value stringParm = visit(ctx->expression(0));
	if (!stringParm.isString()) {
		BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'PaddedString', string expected", ctx));
	}
	Value expectedLengthParm = visit(ctx->expression(1));
	if (!expectedLengthParm.isDouble()) {
		BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for second parameter of function 'PaddedString', number expected", ctx));
	}

	std::string stringStr = stringParm.asString();
	std::size_t expectedLength = static_cast<std::size_t>(expectedLengthParm.asDouble());
	std::size_t actualLength = stringStr.length();
	std::size_t missingLength = expectedLength - actualLength;
	if (missingLength > 0) {
		std::string(missingLength, '0').append(stringStr);
	}

	return Value(stringStr);
}

antlrcpp::Any EvalVisitor::visitDifferenceInMonthsFunctionCall(RatingEngineParser::DifferenceInMonthsFunctionCallContext *ctx)
{

	Value dateLeftParm = visit(ctx->expression(0));
	if (!dateLeftParm.isString()) {
		BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for first parameter of function 'DifferenceInMonths', string expected", ctx));
	}
	Value dateRightParm = visit(ctx->expression(1));
	if (!dateRightParm.isString()) {
		BOOST_THROW_EXCEPTION(EvalException("Illegal argument type for second parameter of function 'DifferenceInMonths', number expected", ctx));
	}

	const std::string& dateLeftStr = dateLeftParm.asString();
	const std::string& dateRightStr = dateRightParm.asString();

	bgr::date dateLeft;
	bgr::date dateRight;

	try {
		dateLeft = dateFromString(dateLeftStr);
	}
	catch (const std::exception&) {
		BOOST_THROW_EXCEPTION(EvalException("Cannot convert first parameter of function 'DifferenceInMonths' to date: " + dateLeftParm.asString(), ctx));
	}

	try {
		dateRight = dateFromString(dateRightStr);
	}
	catch (const std::exception&) {
		BOOST_THROW_EXCEPTION(EvalException("Cannot convert second parameter of function 'DifferenceInMonths' to date: " + dateRightParm.asString(), ctx));
	}

	try {
		bgr::days diffDays = dateLeft - dateRight;
		int months = (dateLeft.year() - dateRight.year()) * 12 + dateLeft.month() - dateRight.month();
		return Value(double(months));
	}
	catch (const std::exception&) {
		BOOST_THROW_EXCEPTION(EvalException("Cannot calculate differnce in days in function 'DifferenceInMonths' with first parameter '" +
			dateLeftParm.asString() + "' and second parameter '" + dateRightParm.asString() + "'", ctx));
	}


}

antlrcpp::Any EvalVisitor::visitIdentifierFunctionCall(RatingEngineParser::IdentifierFunctionCallContext* ctx)
{
    // Identifier '(' exprList? ')' #identifierFunctionCall

    typedef std::chrono::high_resolution_clock Clock_t;

    std::vector<RatingEngineParser::ExpressionContext*> params(ctx->exprList() != nullptr ? ctx->exprList()->expression() : std::vector<RatingEngineParser::ExpressionContext*>());
    const std::string functionName(ctx->Identifier()->getText());

    try {
        const std::string functionId = FunctionIdBuilder::createId(functionName, params.size());
        std::shared_ptr<CalculusPrime::IFunction> function = m_parsingContext->getFunction(functionId);

        // check if function implementation exists
        if (!function) {
            std::ostringstream msg;
            msg << "function " << functionName << " with " << params.size() << " parameters not defined";
            BOOST_THROW_EXCEPTION(std::runtime_error(msg.str()));
        }

        // parse the parameters
        EvalVisitor evalVisitor(m_parsingContext);
        std::vector<Value> paramValues;
        paramValues.reserve(params.size());

        for (RatingEngineParser::ExpressionContext* param : params) {
            paramValues.push_back(evalVisitor.visit(param));
        }

        // get function result from business logic
        const std::chrono::time_point<Clock_t> startTime = Clock_t::now();

        Value result = function->execute(paramValues, m_parsingContext);

        const std::chrono::time_point<Clock_t> endTime = Clock_t::now();
        m_parsingContext->addFunctionDurationMicroSecs(std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count());

        log4cplus::Logger logger = m_parsingContext->getLogger();
        if (logger.isEnabledFor(log4cplus::DEBUG_LOG_LEVEL)) {
            std::ostringstream params;
            for (std::size_t i = 0; i < paramValues.size(); ++i) {
                if (i != 0) {
                    params << ',';
                }
                params << paramValues[i];
            }
            LOG4CPLUS_DEBUG(m_parsingContext->getLogger(), functionName << '(' << params.str() << ") result=" << result);
        }

        return result;
    }
    catch (const RatingEngineException&) {
        throw;
    }
    catch (const std::exception& ex) {
        BOOST_THROW_EXCEPTION(EvalException(ex.what(), ctx));
    }
}

antlrcpp::Any EvalVisitor::visitIfStatement(RatingEngineParser::IfStatementContext* ctx)
{
    // ifStatement
    //  : ifStat elseIfStat* elseStat? End
    //  ;
    //
    // ifStat
    //  : If '(' expression ')' Then block
    //  ;
    //
    // elseIfStat
    //  : Else If '(' expression ')' Then block
    //  ;
    //
    // elseStat
    //  : Else block
    //  ;

    // if ...
    Value ifExpr = visit(ctx->ifStat()->expression());
    if (ifExpr.asBool()) {
        return visit(ctx->ifStat()->block());
    }

    // else if ...
    for (std::size_t i = 0; i < ctx->elseIfStat().size(); i++) {
        Value elseIfExpr = visit(ctx->elseIfStat(i)->expression());
        if (elseIfExpr.asBool()) {
            return visit(ctx->elseIfStat(i)->block());
        }
    }

    // else ...
    if (ctx->elseStat() != nullptr) {
        return visit(ctx->elseStat()->block());
    }

    return Value();
}

antlrcpp::Any EvalVisitor::visitLtExpression(RatingEngineParser::LtExpressionContext* ctx)
{
    // expression '<' expression                #ltExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    if (lhs.isDouble() && rhs.isDouble()) {
        return Value(lhs.asDouble() < rhs.asDouble());
    }
    if (lhs.isString() && rhs.isString()) {
        return Value(lhs.asString() < (rhs.asString()));
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}

antlrcpp::Any EvalVisitor::visitGtExpression(RatingEngineParser::GtExpressionContext* ctx)
{
    // expression '>' expression                #gtExpression
    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    if (lhs.isDouble() && rhs.isDouble()) {
        return Value(lhs.asDouble() > rhs.asDouble());
    }
    if (lhs.isString() && rhs.isString()) {
        return Value(lhs.asString() > (rhs.asString()));
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}


antlrcpp::Any EvalVisitor::visitBoolExpression(RatingEngineParser::BoolExpressionContext* ctx)
{
    return Value(equalsIgnCase(ctx->getText(), "true"));
}

antlrcpp::Any EvalVisitor::visitNotEqExpression(RatingEngineParser::NotEqExpressionContext* ctx)
{
    // expression '!=' expression               #notEqExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    return Value(!(lhs == rhs));
}

antlrcpp::Any EvalVisitor::visitModuloExpression(RatingEngineParser::ModuloExpressionContext* ctx)
{
    // expression '%' expression                #moduloExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    if (lhs.isDouble() && rhs.isDouble()) {
        std::feclearexcept(FE_ALL_EXCEPT);
        double result = std::fmod(lhs.asDouble(), rhs.asDouble());
        if (std::fetestexcept(FE_INVALID)) {
            BOOST_THROW_EXCEPTION(RatingEngineException(RatingEngineError::DIVISION_BY_ZERO));
        }
        return Value(result);
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}

antlrcpp::Any EvalVisitor::visitNumberExpression(RatingEngineParser::NumberExpressionContext* ctx)
{
    // Number                                   #numberExpression

    return Value(std::stod(ctx->getText()));
}

antlrcpp::Any EvalVisitor::visitIdentifierExpression(RatingEngineParser::IdentifierExpressionContext* ctx)
{
    // Identifier indexes?                      #identifierExpression

    std::string id = ctx->Identifier()->getText();
    return m_parsingContext->resolve(id);
}

antlrcpp::Any EvalVisitor::visitNotExpression(RatingEngineParser::NotExpressionContext* ctx)
{
    // '!' expression                           #notExpression

    Value v = visit(ctx->expression());
    if (!v.isBool()) {
        BOOST_THROW_EXCEPTION(EvalException(ctx));
    }
    return Value(!v.asBool());
}

antlrcpp::Any EvalVisitor::visitMultiplyExpression(RatingEngineParser::MultiplyExpressionContext* ctx)
{
    // expression '*' expression                #multiplyExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));

    // number * number
    if (lhs.isDouble() && rhs.isDouble()) {
        return Value(lhs.asDouble() * rhs.asDouble());
    }

    // string * number
    if (lhs.isString() && rhs.isDouble()) {
        std::string str;
        int amount = static_cast<int>(rhs.asDouble() + 0.5);
        for (int i = 0; i < amount; ++i) {
            str += lhs.asString();
        }
        return Value(str);
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}

antlrcpp::Any EvalVisitor::visitGtEqExpression(RatingEngineParser::GtEqExpressionContext* ctx)
{
    // expression '>=' expression               #gtEqExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    if (lhs.isDouble() && rhs.isDouble()) {
        return Value(lhs.asDouble() >= rhs.asDouble());
    }
    if (lhs.isString() && rhs.isString()) {
        return Value(lhs.asString() >= (rhs.asString()));
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}

antlrcpp::Any EvalVisitor::visitDivideExpression(RatingEngineParser::DivideExpressionContext* ctx)
{
    // expression '/' expression                #divideExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));

    if (lhs.isDouble() && rhs.isDouble()) {
        double result = lhs.asDouble() / rhs.asDouble();
        if (std::isinf(result)) {
            BOOST_THROW_EXCEPTION(RatingEngineException(RatingEngineError::DIVISION_BY_ZERO));
        }
        return Value(result);
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}

antlrcpp::Any EvalVisitor::visitOrExpression(RatingEngineParser::OrExpressionContext* ctx)
{
    // expression Or expression               #orExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));

    if (!lhs.isBool() || !rhs.isBool()) {
        BOOST_THROW_EXCEPTION(EvalException(ctx));
    }
    return Value(lhs.asBool() || rhs.asBool());
}

antlrcpp::Any EvalVisitor::visitUnaryMinusExpression(RatingEngineParser::UnaryMinusExpressionContext* ctx)
{
    // '-' expression                           #unaryMinusExpression

    Value v = visit(ctx->expression());
    if (!v.isDouble()) {
        BOOST_THROW_EXCEPTION(EvalException(ctx));
    }
    return Value(-1 * v.asDouble());
}

antlrcpp::Any EvalVisitor::visitPowerExpression(RatingEngineParser::PowerExpressionContext* ctx)
{
    // expression '^' expression                #powerExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    if (lhs.isDouble() && rhs.isDouble()) {
        return Value(std::pow(lhs.asDouble(), rhs.asDouble()));
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}

antlrcpp::Any EvalVisitor::visitEqExpression(RatingEngineParser::EqExpressionContext* ctx)
{
    // expression '==' expression               #eqExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    return Value(lhs == rhs);
}

antlrcpp::Any EvalVisitor::visitAndExpression(RatingEngineParser::AndExpressionContext* ctx)
{
    // expression And expression               #andExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));

    if (!lhs.isBool() || !rhs.isBool()) {
        BOOST_THROW_EXCEPTION(EvalException(ctx));
    }
    return Value(lhs.asBool() && rhs.asBool());
}

antlrcpp::Any EvalVisitor::visitStringExpression(RatingEngineParser::StringExpressionContext* ctx)
{
    // String indexes?                          #stringExpression

    std::string text = ctx->getText();
    // first and last char are '
    text = std::regex_replace(text.substr(1, text.length() - 2), getRegex(), "$1");
    return Value(text);
}

antlrcpp::Any EvalVisitor::visitExpressionExpression(RatingEngineParser::ExpressionExpressionContext* ctx)
{
    // '(' expression ')' indexes?              #expressionExpression

    Value val = visit(ctx->expression());
    return val;
}

antlrcpp::Any EvalVisitor::visitAddExpression(RatingEngineParser::AddExpressionContext* ctx)
{
    // expression '+' expression                #addExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));

    // number + number
    if (lhs.isDouble() && rhs.isDouble()) {
        return Value(lhs.asDouble() + rhs.asDouble());
    }

    std::ostringstream oss;
    // string + any
    if (lhs.isString()) {
		if (rhs.isString()) {
			oss << lhs.asString() << rhs.asString();
		}
		else {
			oss << lhs.asString() << rhs;
		}
        return Value(oss.str());
    }

    // any + string
    if (rhs.isString()) {
		if (lhs.isString()) {
			oss << lhs.asString() << rhs.asString();
		}
		else {
			oss << lhs << rhs.asString();
		}

		
        return Value(oss.str());
    }
    oss << lhs << rhs;
    return Value(oss.str());
}

antlrcpp::Any EvalVisitor::visitSubtractExpression(RatingEngineParser::SubtractExpressionContext* ctx)
{
    // expression '-' expression                #subtractExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    if (lhs.isDouble() && rhs.isDouble()) {
        return Value(lhs.asDouble() - rhs.asDouble());
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}

antlrcpp::Any EvalVisitor::visitFunctionCallExpression(RatingEngineParser::FunctionCallExpressionContext* ctx)
{
    // functionCall indexes?                    #functionCallExpression

    return visit(ctx->functionCall());
}

antlrcpp::Any EvalVisitor::visitLtEqExpression(RatingEngineParser::LtEqExpressionContext* ctx)
{
    // expression '<=' expression               #ltEqExpression

    Value lhs = visit(ctx->expression(0));
    Value rhs = visit(ctx->expression(1));
    if (lhs.isDouble() && rhs.isDouble()) {
        return Value(lhs.asDouble() <= rhs.asDouble());
    }
    if (lhs.isString() && rhs.isString()) {
        return Value(lhs.asString() <= (rhs.asString()));
    }
    BOOST_THROW_EXCEPTION(EvalException(ctx));
}

} // namespace CalculusPrime
