#pragma once

#include <string>
#include <cstdint>

#include "ast/Statement.hpp"
#include "ExpressionParser.hpp"

namespace dcpu { namespace parser {
	class Parser {
	protected:
		typedef lexer::TokenList::iterator Iterator;

		Iterator _current, _end;
		dcpu::ErrorHandler &_errorHandler;
		dcpu::SymbolTable &_symbolTable;
		std::uint16_t _outputPosition;

		ast::StatementPtr parseLabel(lexer::TokenPtr&);
		ast::StatementPtr parseInstruction(lexer::TokenPtr&);
		ast::ArgumentPtr parseArgument(lexer::TokenPtr&, ast::ArgumentPosition);
		ast::ArgumentPtr parseIndirectStackArgument(lexer::TokenPtr&, ast::ArgumentPosition);
		ast::ArgumentPtr parseMnemonicStackArgument(lexer::TokenPtr&, ast::ArgumentPosition);
		ast::ExpressionPtr parseExpression(bool, bool);

		bool isNextTokenChar(char);
		bool isNextToken(std::function<bool (const lexer::Token&)>);
		lexer::TokenPtr& nextToken();
		void moveBack();
		void advanceUntil(std::function<bool (const lexer::Token&)>);

		bool addStatement(ast::StatementPtr&&);
	public:
		ast::StatementList statements;

		Parser(Iterator start, Iterator end, dcpu::ErrorHandler &errorHandler, dcpu::SymbolTable &symbolTable);

		void parse();
	};

} }
