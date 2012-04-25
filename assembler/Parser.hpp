#pragma once

#include <string>
#include <initializer_list>
#include <cstdint>

#include "ast/Statement.hpp"
#include "ErrorHandler.hpp"
#include "Lexer.hpp"

namespace dcpu { namespace parser {
	struct OpcodeDefinition {
		ast::Opcode _opcode;
		std::uint8_t _args;

		OpcodeDefinition(ast::Opcode opcode, std::uint8_t args)
			: _opcode(opcode), _args(args) {}
	};

	struct RegisterDefinition {
		ast::Register _register;
		bool _indirectable;

		RegisterDefinition(ast::Register reg, bool indirectable)
			: _register(reg), _indirectable(indirectable) {}
	};

	class Parser;

	struct OperatorDefinition {
		ast::BinaryOperator _operator;
		std::function<bool (Parser*)> isNextTokenOperator;
		bool leftRequiresLiteral;
		bool rightRequiresLiteral;

		OperatorDefinition(ast::BinaryOperator, std::function<bool (Parser*)>);
		OperatorDefinition(ast::BinaryOperator, std::function<bool (Parser*)>, bool, bool);
	};

	class Parser {
	protected:
		typedef lexer::TokenList::iterator Iterator;
		typedef ast::ExpressionPtr (Parser::*ExpressionParser)(lexer::TokenPtr&, bool);

		Iterator _current, _end;
		dcpu::ErrorHandler &_errorHandler;

		ast::ExpressionPtr parseBinaryOperation(lexer::TokenPtr&, bool, ExpressionParser,
			std::initializer_list<OperatorDefinition>);
		ast::ExpressionPtr parseGroupedExpression(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseIdentifierExpression(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseLabelExpression(lexer::TokenPtr&);
		ast::ExpressionPtr parseLiteralExpression(lexer::TokenPtr&);
		ast::ExpressionPtr parsePrimaryExpression(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseUnaryOperation(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseMultiplyOperation(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseAddOperation(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseBitwiseShiftOperation(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseBitwiseAndOperation(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseBitwiseXorOperation(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseBitwiseOrOperation(lexer::TokenPtr&, bool);
		ast::ExpressionPtr parseExpression(lexer::TokenPtr&, bool);

		bool parseLabel(lexer::TokenPtr&);
		bool parseInstruction(lexer::TokenPtr&);
		bool parseIndirectStackArgument(lexer::TokenPtr&, ast::ArgumentPtr&, ast::ArgumentPosition);
		bool parseMnemonicStackArgument(lexer::TokenPtr&, ast::ArgumentPtr&, ast::ArgumentPosition);
		bool parseArgument(lexer::TokenPtr&, ast::ArgumentPtr&, ast::ArgumentPosition);

		OpcodeDefinition* lookupOpcode(const std::string&);
		RegisterDefinition* lookupRegister(const std::string&);

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
