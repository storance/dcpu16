#pragma once

#include <string>
#include <stack>
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
		std::string _name;
		common::Register _register;
		bool _indirectable;

		RegisterDefinition(const std::string &name, common::Register reg, bool indirectable)
			: _name(name), _register(reg), _indirectable(indirectable) {}
	};


	class Parser {
	protected:
		typedef std::list<std::shared_ptr<Token>>::iterator Iterator;

		dcpu::ErrorHandler &_errorHandler;
		Iterator _current, _end;

		std::shared_ptr<ast::Expression> parseGroupedExpression(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseIdentifierExpression(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseLabelExpression(std::shared_ptr<Token>);
		std::shared_ptr<ast::Expression> parseLiteralExpression(std::shared_ptr<Token>);
		std::shared_ptr<ast::Expression> parsePrimaryExpression(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseUnaryOperation(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseMultiplyOperation(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseAddOperation(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseBitwiseShiftOperation(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseBitwiseAndOperation(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseBitwiseXorOperation(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseBitwiseOrOperation(std::shared_ptr<Token>, bool);
		std::shared_ptr<ast::Expression> parseExpression(std::shared_ptr<Token>, bool);

		bool parseLabel(std::shared_ptr<Token>);
		bool parseInstruction(std::shared_ptr<Token>);
		bool parseIndirectStackArgument(std::shared_ptr<Token>, std::shared_ptr<ast::Argument>&);
		bool parseArgument(std::shared_ptr<Token>, std::shared_ptr<ast::Argument>&);

		OpcodeDefinition* lookupOpcode(const std::string&);
		RegisterDefinition* lookupRegister(const std::string&);

		bool isNextTokenChar(char);
		template<typename Predicate> bool isNextToken(Predicate);
		std::shared_ptr<Token> nextToken();
		template<typename Predicate> void advanceUntil(Predicate);

		void addLabel(const Location&, const std::string &labelName);
		void addInstruction(const Location&, ast::Opcode, std::shared_ptr<ast::Argument>, std::shared_ptr<ast::Argument>);
	public:
		std::list<std::shared_ptr<ast::Statement>> _statements;

		Parser(Iterator start, Iterator end, dcpu::ErrorHandler &errorHandler);

		void parse();
	};

} }
