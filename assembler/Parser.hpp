#pragma once

#include <string>
#include <map>
#include <cstdint>

#include "ast/Statement.hpp"
#include "ErrorHandler.hpp"
#include "Lexer.hpp"

namespace dcpu { namespace parser {
	struct OpcodeDefinition {
		std::string _mnemonic;
		ast::Opcode _opcode;
		std::uint8_t _args;

		OpcodeDefinition(const std::string &mnemonic, ast::Opcode opcode, std::uint8_t args)
			: _mnemonic(mnemonic), _opcode(opcode), _args(args) {}
	};

	struct RegisterDefinition {
		std::string _mnemonic;
		common::Register _register;
		bool _indirectable;

		RegisterDefinition(const std::string &mnemonic, common::Register reg, bool indirectable)
			: _mnemonic(mnemonic), _register(reg), _indirectable(indirectable) {}
	};

	class Parser {
	protected:
		typedef std::list<std::shared_ptr<Token>>::iterator Iterator;
		typedef ast::ExpressionPtr (Parser::*ExpressionParser)(std::shared_ptr<Token>, bool);
		typedef std::map<ast::BinaryOperator, std::function<bool ()>> OperatorDefinition;

		dcpu::ErrorHandler &_errorHandler;
		Iterator _current, _end;

		ast::ExpressionPtr parseBinaryOperation(std::shared_ptr<Token>, bool, ExpressionParser, OperatorDefinition);
		ast::ExpressionPtr parseGroupedExpression(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseIdentifierExpression(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseLabelExpression(std::shared_ptr<Token>);
		ast::ExpressionPtr parseLiteralExpression(std::shared_ptr<Token>);
		ast::ExpressionPtr parsePrimaryExpression(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseUnaryOperation(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseMultiplyOperation(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseAddOperation(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseBitwiseShiftOperation(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseBitwiseAndOperation(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseBitwiseXorOperation(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseBitwiseOrOperation(std::shared_ptr<Token>, bool);
		ast::ExpressionPtr parseExpression(std::shared_ptr<Token>, bool);

		bool parseLabel(std::shared_ptr<Token>);
		bool parseInstruction(std::shared_ptr<Token>);
		bool parseIndirectStackArgument(std::shared_ptr<Token>, ast::ArgumentPtr&);
		bool parseArgument(std::shared_ptr<Token>, ast::ArgumentPtr&);

		OpcodeDefinition* lookupOpcode(const std::string&);
		RegisterDefinition* lookupRegister(const std::string&);

		bool isNextTokenChar(char);
		bool isNextToken(std::function<bool (const Token&)>);
		std::shared_ptr<Token> nextToken();
		void advanceUntil(std::function<bool (const Token&)>);

		void addLabel(const Location&, const std::string &labelName);
		void addInstruction(const Location&, ast::Opcode, ast::ArgumentPtr&, ast::ArgumentPtr&);
	public:
		ast::StatementList _statements;

		Parser(Iterator start, Iterator end, dcpu::ErrorHandler &errorHandler);

		void parse();
	};

} }
