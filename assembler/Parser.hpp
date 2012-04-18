#pragma once

#include <string>
#include <stack>
#include <cstdint>

#include "../common.hpp"
#include "AbstractSyntaxTree.hpp"
#include "ErrorHandler.hpp"
#include "Lexer.hpp"

namespace dcpu { namespace parser {
	struct OpcodeDefinition {
		std::string mnemonic;
		dcpu::ast::Opcode opcode;
		std::uint8_t numArgs;

		OpcodeDefinition(dcpu::ast::Opcode opcode, std::uint8_t numArgs)
			: opcode(opcode), numArgs(numArgs) {};
	};

	struct RegisterDefinition {
		Register registerType;
		bool indirectable;
		bool offsetIndirectable;

		RegisterDefinition(Register registerType, bool indirectable, bool offsetIndirectable)
			: registerType(registerType), indirectable(indirectable), offsetIndirectable(offsetIndirectable) {}
	};


	class Parser {
	protected:
		typedef std::list<lexer::token_type>::const_iterator Iterator;

		dcpu::ErrorHandler &errorHandler;
		Iterator current, end;

		boost::optional<ast::Statement> parseStatement();
		boost::optional<ast::Label> parseLabel(lexer::token_type);
		boost::optional<ast::Instruction> parseInstruction(lexer::token_type);
		ast::Argument parseArgument(lexer::token_type);

		boost::optional<OpcodeDefinition> lookupOpcode(std::string);
		boost::optional<RegisterDefinition> lookupRegister(std::string);

		std::string concatTokens(lexer::token_type initial, Iterator end, bool inclusive);
		template<typename Predicate> Iterator findToken(Predicate pred);

		bool isEndOfStatement(Iterator it);
		void advanceToEndOfLine();
		lexer::token_type nextToken(bool skipWsComments=true, bool skipNewline=false);
		void rewind();
	public:
		std::list<ast::Statement> statements;

		Parser(Iterator start, Iterator end, dcpu::ErrorHandler &errorHandler);

		void parse();
	};

} }
