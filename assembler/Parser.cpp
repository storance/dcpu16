#include "Parser.hpp"

#include <iostream>
#include <iterator>
#include <map>

#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace dcpu::common;
using namespace dcpu::lexer;
using namespace dcpu::ast;

namespace dcpu { namespace parser {

// TODO: switch to initializer list
static map<std::string, OpcodeDefinition> opcodes = {
	{"set", OpcodeDefinition("SET", Opcode::SET, 2)},
	{"add", OpcodeDefinition("ADD", Opcode::ADD, 2)},
	{"sub", OpcodeDefinition("SUB", Opcode::SUB, 2)},
	{"mul", OpcodeDefinition("MUL", Opcode::MUL, 2)},
	{"div", OpcodeDefinition("DIV", Opcode::DIV, 2)},
	{"mod", OpcodeDefinition("MOD", Opcode::MOD, 2)},
	{"shr", OpcodeDefinition("SHR", Opcode::SHR, 2)},
	{"shl", OpcodeDefinition("SHL", Opcode::SHL, 2)},
	{"and", OpcodeDefinition("AND", Opcode::AND, 2)},
	{"bor", OpcodeDefinition("BOR", Opcode::BOR, 2)},
	{"xor", OpcodeDefinition("XOR", Opcode::XOR, 2)},
	{"ife", OpcodeDefinition("IFE", Opcode::IFE, 2)},
	{"ifn", OpcodeDefinition("IFN", Opcode::IFN, 2)},
	{"ifg", OpcodeDefinition("IFG", Opcode::IFG, 2)},
	{"ifb", OpcodeDefinition("IFB", Opcode::IFB, 2)},
	{"jsr", OpcodeDefinition("JSR", Opcode::JSR, 1)},
	{"jmp", OpcodeDefinition("JMP", Opcode::JMP, 1)},
	{"push", OpcodeDefinition("PUSH", Opcode::PUSH, 1)},
	{"pop", OpcodeDefinition("POP", Opcode::POP, 1)}
};

static map<std::string, RegisterDefinition> registers = {
	{"a",  RegisterDefinition("A", Register::A, true, true)},
	{"b",  RegisterDefinition("B", Register::B, true, true)},
	{"c",  RegisterDefinition("C", Register::C, true, true)},
	{"x",  RegisterDefinition("X", Register::X, true, true)},
	{"y",  RegisterDefinition("Y", Register::Y, true, true)},
	{"z",  RegisterDefinition("X", Register::Z, true, true)},
	{"i",  RegisterDefinition("I", Register::I, true, true)},
	{"j",  RegisterDefinition("J", Register::J, true, true)},
	{"sp", RegisterDefinition("SP", Register::SP, true, false)},
	{"pc", RegisterDefinition("PC", Register::PC, false, false)},
	{"o",  RegisterDefinition("O", Register::O, false, false)}
};

Parser::Parser(Iterator start, Iterator end, ErrorHandler &errorHandler)
	: _current(start), _end(end), _errorHandler(errorHandler) {
}

OpcodeDefinition* Parser::lookupOpcode(std::string opcodeName) {
	boost::algorithm::to_lower(opcodeName);

	auto it = opcodes.find(opcodeName);
	if (it == opcodes.end()) {
		return nullptr;
	}

	return &it->second;
}

RegisterDefinition* Parser::lookupRegister(std::string registerName) {
	boost::algorithm::to_lower(registerName);

	auto it = registers.find(registerName);
	if (it == registers.end()) {
		return nullptr;
	}

	return &it->second;
}

bool Parser::parseInstruction(std::shared_ptr<Token> currentToken) {
	if (currentToken->isStatementTerminator()) {
		return false;
	}

	OpcodeDefinition* opcodeDef = lookupOpcode(currentToken->content);

	if (!opcodeDef) {
		_errorHandler.error(currentToken->location, 
					boost::format("Unrecognized instruction '%s'") % opcodeDef->_mnemonic);
		return false;
	}

	shared_ptr<Argument> a, b;
	if (opcodeDef->_args > 0 && !parseArgument(nextToken(), a)) {
		advanceToEndOfLine();
		return false;
	}

	if (opcodeDef->_args > 1) {
		auto token = nextToken();

		if (!token->isCharacter(',')) {
			if (token->isNewline()) {
				_errorHandler.error(token->location, 
					boost::format("Instruction '%s' requires two arguments.") % currentToken->content);
			} else {
				_errorHandler.error(token->location, boost::format("Expected ',' but found '%s'") % token->content);
			}

			advanceToEndOfLine();
			return false;
		}

		if (!parseArgument(nextToken(), b)) {
			advanceToEndOfLine();
			return false;
		} 
	}

	addInstruction(currentToken->location, opcodeDef->_opcode, a, b);
	return true;
}

bool Parser::parseArgument(std::shared_ptr<Token> currentToken, shared_ptr<ast::Argument>& arg) {
	if (currentToken->isCharacter(',')) {
		_errorHandler.error(currentToken->location, "Unexpected ','; expected an instruction argument.");
		return false;
	}

	if (currentToken->isStatementTerminator()) {
		_errorHandler.error(currentToken->location, "Unexpected newline; expected an instruction argument.");
		return false;
	}

	if (currentToken->isIdentifier()) {
		auto registerDef = lookupRegister(currentToken->content);
		if (registerDef) {
			arg = make_shared<ast::ExpressionArgument>(make_shared<ast::RegisterOperand>(
				currentToken->location, registerDef->_register));
			return true;
		}
	}

	_errorHandler.error(currentToken->location, boost::format("Unrecognized instruction argument '%s'") 
		% currentToken->content);
	return false;
}

void Parser::addLabel(const Location& location, const std::string &labelName) {
	_statements.push_back(make_shared<ast::Label>(location, labelName));
}

void Parser::addInstruction(const Location& location, ast::Opcode opcode, shared_ptr<ast::Argument> a,
	shared_ptr<ast::Argument> b) {

	_statements.push_back(make_shared<ast::Instruction>(location, opcode, a, b));
}

bool Parser::parseLabel(std::shared_ptr<Token> currentToken) {
	if (currentToken->isCharacter(':')) {
		auto token = nextToken();

		if (token->isIdentifier()) {
			addLabel(currentToken->location, token->content);
			return true;
		} else if (token->isStatementTerminator()) {
			_errorHandler.error(token->location, boost::format(
				"Unexpected token '%s', expected a label name.") % token->content);
			return false;
		} else {
			_errorHandler.error(currentToken->location, "Unexpected token ':', expected a label or instruction.");
			advanceToEndOfLine();
			return false;
		}
	} else if (currentToken->isIdentifier()) {
		auto token = nextToken();

		if (token->isCharacter(':')) {
			addLabel(currentToken->location, currentToken->content);
			return true;
		} else {
			rewind();
			return false;
		}
	} else {
		_errorHandler.error(currentToken->location, boost::format(
			"Unexpected token '%s', expected a label or instruction.") % currentToken->content);
		advanceToEndOfLine();
		return false;
	}
}

void Parser::advanceToEndOfLine() {
	while (_current != _end) {
		if ((*_current)->isNewline() || (*_current)->isEOI()) {
			break;
		}

		_current++;
	}
}

void Parser::rewind() {
	--_current;
}

std::shared_ptr<Token> Parser::nextToken() {
	if ((*_current)->isEOI()) {
		return *_current;
	}

	return *_current++;
}

void Parser::parse() {
	while (_current != _end) {
		std::shared_ptr<Token> currentToken = nextToken();

		// we've reached the end
		if (currentToken->isEOI()) {
			break;
		}

		// skip empty lines
		if (currentToken->isNewline()) {
			continue;
		}

		if (parseLabel(currentToken)) {
			currentToken = nextToken();
		}

		parseInstruction(currentToken);
	}
}

}}
