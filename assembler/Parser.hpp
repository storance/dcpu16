#pragma once

#include <string>
#include <cstdint>

#include "ast/Statement.hpp"
#include "SymbolTable.hpp"
#include "ErrorHandler.hpp"
#include "ExpressionParser.hpp"
#include "Types.hpp"

namespace dcpu { namespace parser {
	class Parser {
	protected:
		TokenIterator current, end;
		uint16Ptr outputPosition;

		StatementPtr parseLabel(TokenPtr&);
		StatementPtr parseInstruction(TokenPtr&);
		ArgumentPtr parseArgument(TokenPtr&, ast::ArgumentPosition);
		ArgumentPtr parseIndirectStackArgument(TokenPtr&, ast::ArgumentPosition);
		ArgumentPtr parseMnemonicStackArgument(TokenPtr&, ast::ArgumentPosition);
		ExpressionPtr parseExpression(bool, bool);

		TokenPtr& nextToken();
		void moveBack();
		void advanceUntil(std::function<bool (const lexer::Token&)>);

		bool addStatement(StatementPtr&&);

		Parser(lexer::Lexer &lexer, ErrorHandlerPtr&, SymbolTablePtr&, uint16Ptr &);
	public:
		ErrorHandlerPtr errorHandler;
		SymbolTablePtr symbolTable;
		StatementList statements;

		Parser(lexer::Lexer &lexer);

		void parse();
	};

} }
