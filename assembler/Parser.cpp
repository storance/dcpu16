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

OpcodeDefinition* Parser::lookupOpcode(const std::string &opcodeName) {
	auto it = opcodes.find(boost::algorithm::to_lower_copy(opcodeName));
	if (it == opcodes.end()) {
		return nullptr;
	}

	return &it->second;
}

RegisterDefinition* Parser::lookupRegister(const std::string &registerName) {
	auto it = registers.find(boost::algorithm::to_lower_copy(registerName));
	if (it == registers.end()) {
		return nullptr;
	}

	return &it->second;
}

shared_ptr<Expression> Parser::parsePrimaryExpression(shared_ptr<Token> currentToken, bool indirect) {
	if (currentToken->isCharacter('(')) {
		auto expr = parseExpression(nextToken(), indirect);

		auto nxtToken = nextToken();
		if (!nxtToken->isCharacter(')')) {
			_errorHandler.error(nxtToken->location, boost::format("Unexecpted token '%s'; expected ')'") % nxtToken->content);
			rewind();
		}

		return expr;
	} else if (currentToken->isIdentifier()) {
		auto registerDef = lookupRegister(currentToken->content);
		if (registerDef) {
			if (indirect && !registerDef->_indirectable) {
				_errorHandler.error(currentToken->location, boost::format("Register %s can't be used in an indirection") 
					% registerDef->_name);
				return make_shared<InvalidExpression>(currentToken->location);
			} else {
				return make_shared<RegisterOperand>(currentToken->location, registerDef->_register);
			}
		} else {
			return make_shared<LabelReferenceOperand>(currentToken->location, currentToken->content);
		}
	} else if (currentToken->isCharacter('$')) {
		auto nxtToken = nextToken();
		if (!nxtToken->isIdentifier()) {
			_errorHandler.error(nxtToken->location, boost::format("Expected a label name but found '%s'")
				% nxtToken->content);
			rewind();
			return make_shared<InvalidExpression>(currentToken->location);
		}

		return make_shared<LabelReferenceOperand>(currentToken->location, nxtToken->content);
	} else if (currentToken->isInteger()) {
		shared_ptr<IntegerToken> intToken = asInteger(currentToken);
		if (intToken->overflow) {
			_errorHandler.warning(intToken->location, boost::format(
				"%s is larger than the maximum intermediary value (%d).") % intToken->content % UINT32_MAX);
		}

		return make_shared<LiteralOperand>(currentToken->location, intToken->value);
	} else {
		_errorHandler.error(currentToken->location, boost::format(
			"Expected a label name, register, or literal but found '%s'") % currentToken->content);
		return make_shared<InvalidExpression>(currentToken->location);
	}
}

shared_ptr<Expression> Parser::parseUnaryOperation(shared_ptr<Token> currentToken, bool indirect) {
	UnaryOperator _operator;

	if (currentToken->isCharacter('+')) {
		_operator = UnaryOperator::PLUS;
	} else if (currentToken->isCharacter('-')) {
		_operator = UnaryOperator::MINUS;
	} else if (currentToken->isCharacter('~') || currentToken->isCharacter('!')) {
		_operator = UnaryOperator::NOT;
	} else {
		return parsePrimaryExpression(currentToken, indirect);
	}

	auto operand = parseExpression(nextToken(), indirect);
	return make_shared<UnaryOperation>(currentToken->location, _operator, operand);
}

shared_ptr<Expression> Parser::parseMultiplyOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseUnaryOperation(currentToken, indirect);

	BinaryOperator _operator;
	auto nxtToken = nextToken();
	if (nxtToken->isCharacter('*')) {
		_operator = BinaryOperator::MULTIPLY;
	} else if (nxtToken->isCharacter('/')) {
		_operator = BinaryOperator::DIVIDE;
	} else {
		rewind();
		return left;
	}

	auto right = parseUnaryOperation(nextToken(), indirect);
	return make_shared<BinaryOperation>(nxtToken->location, _operator, left, right);
}

shared_ptr<Expression> Parser::parseAddOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseMultiplyOperation(currentToken, indirect);

	BinaryOperator _operator;
	auto nxtToken = nextToken();
	if (nxtToken->isCharacter('+')) {
		_operator = BinaryOperator::PLUS;
	} else if (nxtToken->isCharacter('-')) {
		_operator = BinaryOperator::MINUS;
	} else {
		rewind();
		return left;
	}

	auto right = parseMultiplyOperation(nextToken(), indirect);
	return make_shared<BinaryOperation>(nxtToken->location, _operator, left, right);
}

shared_ptr<Expression> Parser::parseBitwiseShiftOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseAddOperation(currentToken, indirect);

	BinaryOperator _operator;
	auto nxtToken = nextToken();
	if (nxtToken->isShiftLeft()) {
		_operator = BinaryOperator::SHIFT_LEFT;
	} else if (nxtToken->isShiftRight()) {
		_operator = BinaryOperator::SHIFT_RIGHT;
	} else {
		rewind();
		return left;
	}

	auto right = parseAddOperation(nextToken(), indirect);
	return make_shared<BinaryOperation>(nxtToken->location, _operator, left, right);
}

shared_ptr<Expression> Parser::parseBitwiseAndOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseBitwiseShiftOperation(currentToken, indirect);

	auto nxtToken = nextToken();
	if (nxtToken->isCharacter('&')) {
		auto right = parseBitwiseShiftOperation(nextToken(), indirect);
		return make_shared<BinaryOperation>(nxtToken->location, BinaryOperator::AND, left, right);
	} else {
		rewind();
		return left;
	}
}

shared_ptr<Expression> Parser::parseBitwiseXorOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseBitwiseAndOperation(currentToken, indirect);

	auto nxtToken = nextToken();
	if (nxtToken->isCharacter('^')) {
		auto right = parseBitwiseAndOperation(nextToken(), indirect);
		return make_shared<BinaryOperation>(nxtToken->location, BinaryOperator::XOR, left, right);
	} else {
		rewind();
		return left;
	}
}

shared_ptr<Expression> Parser::parseBitwiseOrOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseBitwiseXorOperation(currentToken, indirect);

	auto nxtToken = nextToken();
	if (nxtToken->isCharacter('|')) {
		auto right = parseBitwiseXorOperation(nextToken(), indirect);
		return make_shared<BinaryOperation>(nxtToken->location, BinaryOperator::OR, left, right);
	} else {
		rewind();
		return left;
	}
}

shared_ptr<Expression> Parser::parseExpression(shared_ptr<Token> currentToken, bool indirect) {
	return parseBitwiseOrOperation(currentToken, indirect);
}

bool Parser::parseInstruction(std::shared_ptr<Token> currentToken) {
	if (currentToken->isStatementTerminator()) {
		return false;
	}

	OpcodeDefinition* opcodeDef = lookupOpcode(currentToken->content);

	if (!opcodeDef) {
		_errorHandler.error(currentToken->location, 
					boost::format("Unrecognized instruction '%s'") % currentToken->content);
		advanceToEndOfLine();
		return false;
	}

	shared_ptr<Argument> a, b;
	if (opcodeDef->_args > 0) {
		if (!parseArgument(nextToken(), a)) {
			advanceToEndOfLine();
			return false;
		}
	}

	if (opcodeDef->_args > 1) {
		auto token = nextToken();

		if (!token->isCharacter(',')) {
			if (token->isNewline()) {
				_errorHandler.error(token->location, 
					boost::format("Instruction '%s' requires two arguments.") % opcodeDef->_mnemonic);
			} else {
				_errorHandler.error(token->location, boost::format("Unxpected token '%s'; expected ','") 
					% token->content);
			}

			advanceToEndOfLine();
			return false;
		}

		if (!parseArgument(nextToken(), b)) {
			advanceToEndOfLine();
			return false;
		} 
	}

	auto eolToken = nextToken();
	if (!eolToken->isStatementTerminator()) {
		_errorHandler.error(eolToken->location, boost::format("Unexpected token '%s'; expected 'newline' or 'eof'") 
			% eolToken->content);
		advanceToEndOfLine();
	}

	addInstruction(currentToken->location, opcodeDef->_opcode, a, b);
	return true;
}

bool Parser::parseArgument(std::shared_ptr<Token> currentToken, shared_ptr<ast::Argument>& arg) {
	if (currentToken->isCharacter(',') || currentToken->isStatementTerminator()) {
		_errorHandler.error(currentToken->location, boost::format("Unexpected '%s'; expected an instruction argument.")
			% currentToken->content);
		return false;
	}

	if (currentToken->isCharacter('[')) {
		auto startPos = _current;
		if (!parseIndirectStackArgument(nextToken(), arg)) {
			_current = startPos;
			auto expr = parseExpression(currentToken, true);
			arg = make_shared<IndirectArgument>(expr);
		}

		auto nxtToken = nextToken();
		if (!nxtToken->isCharacter(']')) {
			_errorHandler.error(nxtToken->location, boost::format("Unexpected '%s'; expected a ']'.")
				% currentToken->content);
			rewind();
			return false;
		}

		return true;
	} else if (currentToken->isIdentifier()) {
		if (boost::algorithm::iequals(currentToken->content, "PUSH")) {
			arg = make_shared<StackArgument>(currentToken->location, StackOperation::PUSH);
			return true;
		} else if (boost::algorithm::iequals(currentToken->content, "POP")) {
			arg = make_shared<StackArgument>(currentToken->location, StackOperation::POP);
			return true;
		} else if (boost::algorithm::iequals(currentToken->content, "PEEK")) {
			arg = make_shared<StackArgument>(currentToken->location, StackOperation::PEEK);
			return true;
		}
	}

	auto expr = parseExpression(currentToken, false);
	arg = make_shared<ExpressionArgument>(expr);
	return true;
}

bool Parser::parseIndirectStackArgument(std::shared_ptr<Token> currentToken, shared_ptr<ast::Argument>& arg) {
	if (currentToken->isIdentifier()) {
		if (!boost::algorithm::iequals(currentToken->content, "SP")) {
			return false;
		}

		auto nxtToken = nextToken();

		if (nxtToken->isCharacter(']')) {
			arg = make_shared<StackArgument>(currentToken->location, StackOperation::PEEK);
			rewind();
			return true;
		} else if (nxtToken->isIncrement()) {
			arg = make_shared<StackArgument>(currentToken->location, StackOperation::POP);
			return true;
		}

		return false;
	} else if (currentToken->isDecrement()) {
		auto nxtToken = nextToken();
		if (nxtToken->isIdentifier() && boost::algorithm::iequals(nxtToken->content, "SP")) {
			arg = make_shared<StackArgument>(currentToken->location, StackOperation::PUSH);
			return true;
		}

		return false;
	}

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
	if (_current == _end) {
		--_current;
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
