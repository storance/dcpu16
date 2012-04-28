#pragma once

#include <initializer_list>

#include "ast/Expression.hpp"
#include "ErrorHandler.hpp"
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


	class ExpressionParser {
	protected:
		typedef ExpressionPtr (ExpressionParser::*ExpressionParserFunc)();

		TokenIterator &_current, _end;
		ErrorHandlerPtr _errorHandler;
		bool _insideIndirect;
		bool _allowRegisters;
		bool _foundRegister;

		void checkForNonLiteralExpression(const OperatorDefinition *, ExpressionPtr&, ExpressionPtr&);
		ExpressionPtr parseBinaryOperation(ExpressionParserFunc, std::initializer_list<OperatorDefinition>);
		ExpressionPtr parsePrimaryExpression(TokenPtr&);
		ExpressionPtr parseGroupedExpression();
		ExpressionPtr parseIdentifierExpression(TokenPtr&);
		ExpressionPtr parseLabelExpression();
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
		ExpressionParser(TokenIterator&, TokenIterator, ErrorHandlerPtr&, bool, bool);

		ExpressionPtr parse();
	};

}}