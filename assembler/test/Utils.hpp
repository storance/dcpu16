#pragma once

#include <string>
#include <cstdint>

#include "../Lexer.hpp"
#include "../Parser.hpp"

void runLexer(const std::string&, int, std::shared_ptr<dcpu::lexer::Lexer>&);
void runParser(const std::string&, int, std::shared_ptr<dcpu::parser::Parser>&);
void runParser(std::shared_ptr<dcpu::lexer::Lexer> &, int , std::shared_ptr<dcpu::parser::Parser>&);
void runExpressionParser(const std::string&, dcpu::ExpressionPtr&, bool, bool);
