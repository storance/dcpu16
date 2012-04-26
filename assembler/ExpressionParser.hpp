#pragma once

#include <initializer_list>

#include "ast/Expression.hpp"
#include "ErrorHandler.hpp"
#include "Lexer.hpp"
#include "Token.hpp"

namespace dcpu { namespace parser {
	struct OperatorDefinition {
		ast::BinaryOperator _operator;
		std::function<bool (lexer::TokenPtr&)> isNextTokenOperator;
		bool leftRequiresLiteral;
		bool rightRequiresLiteral;

		OperatorDefinition(ast::BinaryOperator, std::function<bool (lexer::TokenPtr&)>);
		OperatorDefinition(ast::BinaryOperator, std::function<bool (lexer::TokenPtr&)>, bool, bool);
	};


	class ExpressionParser {
	protected:
		typedef lexer::TokenList::iterator Iterator;
		typedef ast::ExpressionPtr (ExpressionParser::*ExpressionParserFunc)();

		Iterator &_current, _end;
		dcpu::ErrorHandler &_errorHandler;
		bool _insideIndirect;
		bool _allowRegisters;
		bool _foundRegister;

		void checkForNonLiteralExpression(const OperatorDefinition *, ast::ExpressionPtr&, ast::ExpressionPtr&);
		ast::ExpressionPtr parseBinaryOperation(ExpressionParserFunc, std::initializer_list<OperatorDefinition>);
		ast::ExpressionPtr parsePrimaryExpression(lexer::TokenPtr&);
		ast::ExpressionPtr parseGroupedExpression();
		ast::ExpressionPtr parseIdentifierExpression(lexer::TokenPtr&);
		ast::ExpressionPtr parseLabelExpression();
		ast::ExpressionPtr parseLiteralExpression(lexer::TokenPtr&);
		ast::ExpressionPtr parseUnaryOperation();
		ast::ExpressionPtr parseMultiplyOperation();
		ast::ExpressionPtr parseAddOperation();
		ast::ExpressionPtr parseBitwiseShiftOperation();
		ast::ExpressionPtr parseBitwiseAndOperation();
		ast::ExpressionPtr parseBitwiseXorOperation();
		ast::ExpressionPtr parseBitwiseOrOperation();

		lexer::TokenPtr& nextToken();
	public:
		ExpressionParser(Iterator&, Iterator, dcpu::ErrorHandler&, bool, bool);

		ast::ExpressionPtr parse();
	};

}}