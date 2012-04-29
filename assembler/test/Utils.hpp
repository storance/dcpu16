#pragma once

#include <string>
#include <cstdint>
#include <functional>

#include "../Lexer.hpp"
#include "../Parser.hpp"
#include "../ast/Expression.hpp"

/*typedef std::function<void (dcpu::ExpressionPtr&)> ExpressionFunc;

ExpressionFunc assertIsBinaryOperation(dcpu::ast::BinaryOperator, ExpressionFunc, ExpressionFunc);
ExpressionFunc assertIsUnaryOperation(dcpu::ast::UnaryOperator, ExpressionFunc);
ExpressionFunc assertIsLabelRef(const std::string&);
ExpressionFunc assertIsRegister(dcpu::ast::Register);
ExpressionFunc assertIsLiteral(std::uint32_t);
ExpressionFunc assertIsEvaluatedRegister(dcpu::ast::Register, bool, std::int32_t);
ExpressionFunc assertIsEvaluatedRegister(dcpu::ast::Register);
ExpressionFunc assertIsEvaluatedLiteral(std::int32_t);*/

void runLexer(const std::string&, int, std::shared_ptr<dcpu::lexer::Lexer>&);
void runParser(const std::string&, int, std::shared_ptr<dcpu::parser::Parser>&);
void runParser(std::shared_ptr<dcpu::lexer::Lexer> &, int , std::shared_ptr<dcpu::parser::Parser>&);
void runExpressionParser(const std::string&, dcpu::ExpressionPtr&, bool, bool);
