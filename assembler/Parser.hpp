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

		bool parseLabel(lexer::TokenPtr&);
		bool parseInstruction(lexer::TokenPtr&);
		bool parseIndirectStackArgument(lexer::TokenPtr&, ast::ArgumentPtr&, ast::ArgumentPosition);
		bool parseMnemonicStackArgument(lexer::TokenPtr&, ast::ArgumentPtr&, ast::ArgumentPosition);
		bool parseArgument(lexer::TokenPtr&, ast::ArgumentPtr&, ast::ArgumentPosition);
		ast::ExpressionPtr parseExpression(bool, bool);

		bool isNextTokenChar(char);
		bool isNextToken(std::function<bool (const lexer::Token&)>);
		lexer::TokenPtr& nextToken();
		void advanceUntil(std::function<bool (const lexer::Token&)>);

		void addLabel(const lexer::Location&, const std::string &labelName);
		void addInstruction(const lexer::Location&, ast::Opcode, ast::ArgumentPtr&, ast::ArgumentPtr&);
	public:
		ast::StatementList statements;

		Parser(Iterator start, Iterator end, dcpu::ErrorHandler &errorHandler);

		void parse();
	};

} }
