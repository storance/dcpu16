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
		bool _offsetIndirectable;

		RegisterDefinition(const std::string &name, common::Register reg, bool indirectable, bool offsetIndirectable)
			: _name(name), _register(reg), _indirectable(indirectable), _offsetIndirectable(offsetIndirectable) {}
	};


	class Parser {
	protected:
		typedef std::list<std::shared_ptr<Token>>::iterator Iterator;

		dcpu::ErrorHandler &_errorHandler;
		Iterator _current, _end;

		bool parseLabel(std::shared_ptr<Token>);
		bool parseInstruction(std::shared_ptr<Token>);
		bool parseArgument(std::shared_ptr<Token>, std::shared_ptr<ast::Argument>&);

		OpcodeDefinition* lookupOpcode(std::string);
		RegisterDefinition* lookupRegister(std::string);

		void advanceToEndOfLine();
		std::shared_ptr<Token> nextToken();
		void rewind();

		void addLabel(const Location&, const std::string &labelName);
		void addInstruction(const Location&, ast::Opcode, std::shared_ptr<ast::Argument>, std::shared_ptr<ast::Argument>);
	public:
		std::list<std::shared_ptr<ast::Statement>> _statements;

		Parser(Iterator start, Iterator end, dcpu::ErrorHandler &errorHandler);

		void parse();
	};

} }
