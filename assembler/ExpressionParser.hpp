#pragma once

#include <initializer_list>

#include "ast/Expression.hpp"
#include "ErrorHandler.hpp"
#include "SymbolTable.hpp"
#include "Lexer.hpp"
#include "Token.hpp"
#include "Types.hpp"

namespace dcpu { namespace parser {
	struct OperatorDefinition {
		ast::BinaryOperator _operator;
		std::function<bool (TokenPtr&)> isNextTokenOperator;
		bool leftRequiresLiteral;
		bool rightRequiresLiteral;

		OperatorDefinition(ast::BinaryOperator, std::function<bool (TokenPtr&)>);
		OperatorDefinition(ast::BinaryOperator, std::function<bool (TokenPtr&)>, bool, bool);
	};

	class FoundRegister {
	public:
		bool found;
		ast::Register _register;
		lexer::Location location;

		FoundRegister();

		void set(ast::Register _register, const lexer::Location &location);
		operator bool();
	};

	class ExpressionParser {
	protected:
		typedef ExpressionPtr (ExpressionParser::*ExpressionParserFunc)();

		TokenIterator &current, end;
		ErrorHandlerPtr errorHandler;
		SymbolTablePtr symbolTable;
		bool labelsAllowed, registersAllowed, indirection;
		FoundRegister foundRegister;

		void checkForNonLiteralExpression(const OperatorDefinition *, ExpressionPtr&, ExpressionPtr&);
		ExpressionPtr parseBinaryOperation(ExpressionParserFunc, std::initializer_list<OperatorDefinition>);
		ExpressionPtr parsePrimaryExpression(TokenPtr&);
		ExpressionPtr parseGroupedExpression();
		ExpressionPtr parseIdentifierExpression(TokenPtr&);
		ExpressionPtr parseLabelExpression(TokenPtr&);
		ExpressionPtr parseLiteralExpression(TokenPtr&);
		ExpressionPtr parseUnaryOperation();
		ExpressionPtr parseMultiplyOperation();
		ExpressionPtr parseAddOperation();
		ExpressionPtr parseBitwiseShiftOperation();
		ExpressionPtr parseBitwiseAndOperation();
		ExpressionPtr parseBitwiseXorOperation();
		ExpressionPtr parseBitwiseOrOperation();

		TokenPtr& nextToken();
	public:
		ExpressionParser(TokenIterator&, TokenIterator, ErrorHandlerPtr&, SymbolTablePtr&, bool, bool, bool);

		ExpressionPtr parse();
	};

}}