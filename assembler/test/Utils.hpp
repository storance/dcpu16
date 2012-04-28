#pragma once

#include <string>
#include <cstdint>
#include <functional>

#include "../ast/Expression.hpp"

typedef std::function<void (dcpu::ast::ExpressionPtr&)> ExpressionFunc;

ExpressionFunc assertIsBinaryOperation(dcpu::ast::BinaryOperator, ExpressionFunc, ExpressionFunc);
ExpressionFunc assertIsUnaryOperation(dcpu::ast::UnaryOperator, ExpressionFunc);
ExpressionFunc assertIsLabelRef(const std::string&);
ExpressionFunc assertIsRegister(dcpu::ast::Register);
ExpressionFunc assertIsLiteral(std::uint32_t);
ExpressionFunc assertIsEvaluatedRegister(dcpu::ast::Register, bool, std::int32_t);
ExpressionFunc assertIsEvaluatedRegister(dcpu::ast::Register);
ExpressionFunc assertIsEvaluatedLiteral(std::int32_t);