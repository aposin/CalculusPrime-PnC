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
#ifndef CP_EVALVISITOR_H
#define CP_EVALVISITOR_H 1

#include <memory>
#include "RatingEngineBaseVisitor.h"

namespace CalculusPrime {

class ParsingContext;

class EvalVisitor : public RatingEngineBaseVisitor
{
public:
    EvalVisitor(const std::shared_ptr<ParsingContext>& parsingContext_)
        : m_parsingContext(parsingContext_)
    {
    }

    virtual ~EvalVisitor();

    virtual antlrcpp::Any visitBlock(RatingEngineParser::BlockContext* ctx) override;

    virtual antlrcpp::Any visitErrorFunctionCall(RatingEngineParser::ErrorFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitMaxFunctionCall(RatingEngineParser::MaxFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitMinFunctionCall(RatingEngineParser::MinFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitRoundFunctionCall(RatingEngineParser::RoundFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitCeilFunctionCall(RatingEngineParser::CeilFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitFloorFunctionCall(RatingEngineParser::FloorFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitExpFunctionCall(RatingEngineParser::ExpFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitDayFunctionCall(RatingEngineParser::DayFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitMonthFunctionCall(RatingEngineParser::MonthFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitYearFunctionCall(RatingEngineParser::YearFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitSubstrFunctionCall(RatingEngineParser::SubstrFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitAddDaysFunctionCall(RatingEngineParser::AddDaysFunctionCallContext *ctx) override;

    virtual antlrcpp::Any visitGetDiffDaysFunctionCall(RatingEngineParser::GetDiffDaysFunctionCallContext *ctx) override;

    virtual antlrcpp::Any visitPaddedStringFunctionCall(RatingEngineParser::PaddedStringFunctionCallContext *ctx) override;

    virtual antlrcpp::Any visitDifferenceInMonthsFunctionCall(RatingEngineParser::DifferenceInMonthsFunctionCallContext *ctx) override;

    virtual antlrcpp::Any visitIdentifierFunctionCall(RatingEngineParser::IdentifierFunctionCallContext* ctx) override;

    virtual antlrcpp::Any visitIfStatement(RatingEngineParser::IfStatementContext* ctx) override;

    virtual antlrcpp::Any visitLtExpression(RatingEngineParser::LtExpressionContext* ctx) override;

    virtual antlrcpp::Any visitGtExpression(RatingEngineParser::GtExpressionContext* ctx) override;

    virtual antlrcpp::Any visitBoolExpression(RatingEngineParser::BoolExpressionContext* ctx) override;

    virtual antlrcpp::Any visitNotEqExpression(RatingEngineParser::NotEqExpressionContext* ctx) override;

    virtual antlrcpp::Any visitModuloExpression(RatingEngineParser::ModuloExpressionContext* ctx) override;

    virtual antlrcpp::Any visitNumberExpression(RatingEngineParser::NumberExpressionContext* ctx) override;

    virtual antlrcpp::Any visitIdentifierExpression(RatingEngineParser::IdentifierExpressionContext* ctx) override;

    virtual antlrcpp::Any visitNotExpression(RatingEngineParser::NotExpressionContext* ctx) override;

    virtual antlrcpp::Any visitMultiplyExpression(RatingEngineParser::MultiplyExpressionContext* ctx) override;

    virtual antlrcpp::Any visitGtEqExpression(RatingEngineParser::GtEqExpressionContext* ctx) override;

    virtual antlrcpp::Any visitDivideExpression(RatingEngineParser::DivideExpressionContext* ctx) override;

    virtual antlrcpp::Any visitOrExpression(RatingEngineParser::OrExpressionContext* ctx) override;

    virtual antlrcpp::Any visitUnaryMinusExpression(RatingEngineParser::UnaryMinusExpressionContext* ctx) override;

    virtual antlrcpp::Any visitPowerExpression(RatingEngineParser::PowerExpressionContext* ctx) override;

    virtual antlrcpp::Any visitEqExpression(RatingEngineParser::EqExpressionContext* ctx) override;

    virtual antlrcpp::Any visitAndExpression(RatingEngineParser::AndExpressionContext* ctx) override;

    virtual antlrcpp::Any visitStringExpression(RatingEngineParser::StringExpressionContext* ctx) override;

    virtual antlrcpp::Any visitExpressionExpression(RatingEngineParser::ExpressionExpressionContext* ctx) override;

    virtual antlrcpp::Any visitAddExpression(RatingEngineParser::AddExpressionContext* ctx) override;

    virtual antlrcpp::Any visitSubtractExpression(RatingEngineParser::SubtractExpressionContext* ctx) override;

    virtual antlrcpp::Any visitFunctionCallExpression(RatingEngineParser::FunctionCallExpressionContext* ctx) override;

    virtual antlrcpp::Any visitLtEqExpression(RatingEngineParser::LtEqExpressionContext* ctx) override;

private:
    const std::shared_ptr<ParsingContext> m_parsingContext;
};

} // namespace CalculusPrime

#endif // #ifndef CP_EVALVISITOR_H
