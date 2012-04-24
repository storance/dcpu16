#include "Parser.hpp"

#include <functional>
#include <iostream>
#include <iterator>
#include <map>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace dcpu::common;
using namespace dcpu::lexer;
using namespace dcpu::ast;

namespace dcpu { namespace parser {

static map<string, OpcodeDefinition> opcodes = {
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

static map<string, RegisterDefinition> registers = {
	{"a",  RegisterDefinition("A", Register::A, true)},
	{"b",  RegisterDefinition("B", Register::B, true)},
	{"c",  RegisterDefinition("C", Register::C, true)},
	{"x",  RegisterDefinition("X", Register::X, true)},
	{"y",  RegisterDefinition("Y", Register::Y, true)},
	{"z",  RegisterDefinition("X", Register::Z, true)},
	{"i",  RegisterDefinition("I", Register::I, true)},
	{"j",  RegisterDefinition("J", Register::J, true)},
	{"sp", RegisterDefinition("SP", Register::SP, false)}, // this supports indirection, but is special cased
	{"pc", RegisterDefinition("PC", Register::PC, false)},
	{"o",  RegisterDefinition("O", Register::O, false)}
};

Parser::Parser(Iterator start, Iterator end, ErrorHandler &errorHandler)
	: _current(start), _end(end), _errorHandler(errorHandler) {
}

void Parser::parse() {
	while (_current != _end) {
		auto* currentToken = &nextToken();

		// we've reached the end
		if ((*currentToken)->isEOI()) {
			break;
		}

		// skip empty lines
		if ((*currentToken)->isNewline()) {
			continue;
		}

		if (parseLabel(*currentToken)) {
			currentToken = &nextToken();
		}

		parseInstruction(*currentToken);
	}
}

bool Parser::parseLabel(TokenPtr& currentToken) {
	if (currentToken->isCharacter(':')) {
		auto& token = nextToken();

		if (token->isIdentifier()) {
			addLabel(currentToken->location, token->content);
			return true;
		} else if (token->isStatementTerminator()) {
			_errorHandler.errorUnexpectedToken(token, "a label name");
			return false;
		} else {
			_errorHandler.errorUnexpectedToken(currentToken, "a label or instruction");
			advanceUntil(mem_fn(&Token::isStatementTerminator));
			return false;
		}
	} else if (currentToken->isIdentifier()) {
		if (isNextTokenChar(':')) {
			nextToken();
			addLabel(currentToken->location, currentToken->content);
			return true;
		} else {
			return false;
		}
	} else {
		_errorHandler.errorUnexpectedToken(currentToken, "a label or instruction");
		advanceUntil(mem_fn(&Token::isStatementTerminator));
		return false;
	}
}

bool Parser::parseInstruction(TokenPtr& currentToken) {
	if (currentToken->isStatementTerminator()) {
		return false;
	}

	OpcodeDefinition* opcodeDef = lookupOpcode(currentToken->content);

	if (!opcodeDef) {
		_errorHandler.error(currentToken->location, 
					boost::format("Unrecognized instruction '%s'") % currentToken->content);
		advanceUntil(mem_fn(&Token::isStatementTerminator));
		return false;
	}

	ArgumentPtr a, b;
	if (opcodeDef->_args > 0) {
		if (!parseArgument(nextToken(), a)) {
			advanceUntil([] (const Token& token) {
				return token.isCharacter(',') || token.isStatementTerminator();
			});
			return false;
		}
	}

	if (opcodeDef->_args > 1) {
		if (!isNextTokenChar(',')) {
			_errorHandler.errorUnexpectedToken(*_current, ',');
		} else {
			nextToken();
		}

		if (!parseArgument(nextToken(), b)) {
			advanceUntil(mem_fn(&Token::isStatementTerminator));
			return false;
		} 
	}

	auto& eolToken = nextToken();
	if (!eolToken->isStatementTerminator()) {
		_errorHandler.errorUnexpectedToken(eolToken, "a 'newline' or 'eof'");
		advanceUntil(mem_fn(&Token::isStatementTerminator));
	}

	addInstruction(currentToken->location, opcodeDef->_opcode, a, b);
	return true;
}

bool Parser::parseArgument(TokenPtr& currentToken, ArgumentPtr& arg) {
	if (currentToken->isCharacter(',') || currentToken->isStatementTerminator()) {
		_errorHandler.errorUnexpectedToken(currentToken, "an instruction argument");
		return false;
	}

	if (currentToken->isCharacter('[')) {
		auto startPos = _current;
		if (!parseIndirectStackArgument(nextToken(), arg)) {
			_current = startPos;
			arg = move(ArgumentPtr(new IndirectArgument(parseExpression(currentToken, true))));
		}

		if (!isNextTokenChar(']')) {
			_errorHandler.errorUnexpectedToken(*_current, ']');
			return false;
		}

		nextToken();
		return true;
	} else if (currentToken->isIdentifier()) {
		if (boost::algorithm::iequals(currentToken->content, "PUSH")) {
			arg = move(ArgumentPtr(new StackArgument(currentToken->location, StackOperation::PUSH)));
			return true;
		} else if (boost::algorithm::iequals(currentToken->content, "POP")) {
			arg = move(ArgumentPtr(new StackArgument(currentToken->location, StackOperation::POP)));
			return true;
		} else if (boost::algorithm::iequals(currentToken->content, "PEEK")) {
			arg = move(ArgumentPtr(new StackArgument(currentToken->location, StackOperation::PEEK)));
			return true;
		}
	}

	ExpressionPtr expr = move(parseExpression(currentToken, false));
	if (!expr->isSimple() && !expr->isEvalsToLiteral()) {
		_errorHandler.error(expr->_location, "Registers may not be operands in expressions outside of an indirection");
		return false;
	}
	arg = move(ArgumentPtr(new ExpressionArgument(expr)));
	return true;
}

bool Parser::parseIndirectStackArgument(TokenPtr& currentToken, ArgumentPtr& arg) {
	if (currentToken->isIdentifier()) {
		if (!boost::algorithm::iequals(currentToken->content, "SP")) {
			return false;
		}

		if (isNextTokenChar(']')) {
			arg = move(ArgumentPtr(new StackArgument(currentToken->location, StackOperation::PEEK)));
			return true;
		} else if (isNextToken(mem_fn(&Token::isIncrement))) {
			nextToken();
			arg = move(ArgumentPtr(new StackArgument(currentToken->location, StackOperation::POP)));
			return true;
		}

		return false;
	} else if (currentToken->isDecrement()) {
		auto& nxtToken = nextToken();
		if (nxtToken->isIdentifier() && boost::algorithm::iequals(nxtToken->content, "SP")) {
			arg = move(ArgumentPtr(new StackArgument(currentToken->location, StackOperation::PUSH)));
			return true;
		}

		return false;
	}

	return false;
}

ExpressionPtr Parser::parseExpression(TokenPtr& currentToken, bool indirect) {
	return parseBitwiseOrOperation(currentToken, indirect);
}

ExpressionPtr Parser::parseBitwiseOrOperation(TokenPtr& currentToken, bool indirect) {
	return parseBinaryOperation(currentToken, indirect, &Parser::parseBitwiseXorOperation,
		{{BinaryOperator::OR, [this] { return this->isNextTokenChar('|'); }}});
}

ExpressionPtr Parser::parseBitwiseXorOperation(TokenPtr& currentToken, bool indirect) {
	return parseBinaryOperation(currentToken, indirect, &Parser::parseBitwiseAndOperation,
		{{BinaryOperator::XOR, [this] { return this->isNextTokenChar('^'); }}});
}

ExpressionPtr Parser::parseBitwiseAndOperation(TokenPtr& currentToken, bool indirect) {
	return parseBinaryOperation(currentToken, indirect, &Parser::parseBitwiseShiftOperation,
		{{BinaryOperator::AND, [this] { return this->isNextTokenChar('&'); }}});
}

ExpressionPtr Parser::parseBitwiseShiftOperation(TokenPtr& currentToken, bool indirect) {
	return parseBinaryOperation(currentToken, indirect, &Parser::parseAddOperation,
		{
			{BinaryOperator::SHIFT_LEFT, [this] { return this->isNextToken(mem_fn(&Token::isShiftLeft)); }},
			{BinaryOperator::SHIFT_RIGHT, [this] { return this->isNextToken(mem_fn(&Token::isShiftRight)); }}
		});
}

ExpressionPtr Parser::parseAddOperation(TokenPtr& currentToken, bool indirect) {
	return parseBinaryOperation(currentToken, indirect, &Parser::parseMultiplyOperation,
		{
			{BinaryOperator::PLUS, [this] { return this->isNextTokenChar('+'); }},
			{BinaryOperator::MINUS, [this] { return this->isNextTokenChar('-'); }}
		});
}

ExpressionPtr Parser::parseMultiplyOperation(TokenPtr& currentToken, bool indirect) {
	return parseBinaryOperation(currentToken, indirect, &Parser::parseUnaryOperation,
		{
			{BinaryOperator::MULTIPLY, [this] { return this->isNextTokenChar('*'); }},
			{BinaryOperator::DIVIDE, [this] { return this->isNextTokenChar('/'); }},
			{BinaryOperator::MODULO, [this] { return this->isNextTokenChar('%'); }}
		});
}

ExpressionPtr Parser::parseBinaryOperation(TokenPtr& currentToken, bool indirect,
	ExpressionParser parseFunc, OperatorDefinition definitions) {

	ExpressionPtr left = (this->*parseFunc)(currentToken, indirect);

	BinaryOperator _operator = BinaryOperator::NONE;
	for (auto definition : definitions) {
		if (definition.second()) {
			_operator = definition.first;
		}
	}

	if (_operator == BinaryOperator::NONE) {
		return move(left);
	}

	auto& operatorToken = nextToken();
	ExpressionPtr right = (this->*parseFunc)(nextToken(), indirect);

	if (_operator != BinaryOperator::PLUS && !left->isEvalsToLiteral()) {
		_errorHandler.error(left->_location, boost::format("The left-hand expression of the operator '%s' "
			"must evaluate to a literal.") % str(_operator));
		return ExpressionPtr(new InvalidExpression(left->_location));
	}

	if (_operator != BinaryOperator::PLUS && _operator != BinaryOperator::MINUS && !right->isEvalsToLiteral()) {
		_errorHandler.error(right->_location, boost::format("The right-hand expression of the operator '%s' "
			"must evaluate to a literal.") % str(_operator));
		return ExpressionPtr(new InvalidExpression(left->_location));
	}

	return ExpressionPtr(new BinaryOperation(operatorToken->location, _operator, left, right));
}

ExpressionPtr Parser::parseUnaryOperation(TokenPtr& currentToken, bool indirect) {
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

	ExpressionPtr operand = parsePrimaryExpression(nextToken(), indirect);
	if (!operand->isEvalsToLiteral()) {
		_errorHandler.error(currentToken->location, boost::format("Unary '%s' may only be used with an expression that "
			"evaluates to a literal.") % str(_operator));
		return ExpressionPtr(new InvalidExpression(currentToken->location));
	}

	return ExpressionPtr(new UnaryOperation(currentToken->location, _operator, operand));
}

ExpressionPtr Parser::parsePrimaryExpression(TokenPtr& currentToken, bool indirect) {
	if (currentToken->isCharacter('(')) {
		return parseGroupedExpression(nextToken(), indirect);
	} else if (currentToken->isIdentifier()) {
		return parseIdentifierExpression(currentToken, indirect);
	} else if (currentToken->isCharacter('$')) {
		return parseLabelExpression(currentToken);
	} else if (currentToken->isInteger()) {
		return parseLiteralExpression(currentToken);
	} else {
		_errorHandler.errorUnexpectedToken(currentToken, "a label name, register, or literal");
		return ExpressionPtr(new InvalidExpression(currentToken->location));
	}
}

ExpressionPtr Parser::parseGroupedExpression(TokenPtr& currentToken, bool indirect) {
	ExpressionPtr expr = parseExpression(currentToken, indirect);

	if (!isNextTokenChar(')')) {
		_errorHandler.errorUnexpectedToken(*_current, ')');
	} else {
		nextToken();
	}

	return expr;
}

ExpressionPtr Parser::parseIdentifierExpression(TokenPtr& currentToken, bool indirect) {
	auto registerDef = lookupRegister(currentToken->content);
	if (registerDef) {
		if (indirect && !registerDef->_indirectable) {
			_errorHandler.error(currentToken->location, boost::format("Register %s can't be used in an indirection") 
				% registerDef->_mnemonic);
			return ExpressionPtr(new InvalidExpression(currentToken->location));
		}
		
		return ExpressionPtr(new RegisterOperand(currentToken->location, registerDef->_register));
	}

	return ExpressionPtr(new LabelReferenceOperand(currentToken));
}

ExpressionPtr Parser::parseLabelExpression(TokenPtr& currentToken) {
	if (!isNextToken(mem_fn(&Token::isIdentifier))) {
		_errorHandler.errorUnexpectedToken(*_current, "a label name");

		return ExpressionPtr(new InvalidExpression(currentToken->location));
	}

	return ExpressionPtr(new LabelReferenceOperand(nextToken()));
}

ExpressionPtr Parser::parseLiteralExpression(TokenPtr& currentToken) {
	IntegerToken* intToken = asInteger(currentToken);
	if (intToken->overflow) {
		_errorHandler.warning(intToken->location, boost::format(
			"%s is larger than the maximum intermediary value (%d).") % intToken->content % UINT32_MAX);
	}

	return ExpressionPtr(new LiteralOperand(intToken->location, intToken->value));
}

void Parser::addLabel(const Location& location, const string &labelName) {
	statements.push_back(StatementPtr(new Label(location, labelName)));
}

void Parser::addInstruction(const Location& location, ast::Opcode opcode, ArgumentPtr &a,
	ArgumentPtr &b) {

	statements.push_back(StatementPtr(new Instruction(location, opcode, a, b)));
}

void Parser::advanceUntil(function<bool (const Token&)> predicate) {
	while (_current != _end) {
		if (predicate(**_current)) {
			break;
		}

		_current++;
	}
}

OpcodeDefinition* Parser::lookupOpcode(const string &opcodeName) {
	auto it = opcodes.find(boost::algorithm::to_lower_copy(opcodeName));
	if (it == opcodes.end()) {
		return nullptr;
	}

	return &it->second;
}

RegisterDefinition* Parser::lookupRegister(const string &registerName) {
	auto it = registers.find(boost::algorithm::to_lower_copy(registerName));
	if (it == registers.end()) {
		return nullptr;
	}

	return &it->second;
}

bool Parser::isNextTokenChar(char c) {
	return isNextToken(bind(&Token::isCharacter, placeholders::_1, c));
}

bool Parser::isNextToken(function<bool (const Token&)> predicate) {
	if (_current == _end) {
		return false;
	}

	return predicate(**_current);
}

TokenPtr& Parser::nextToken() {
	if (_current == _end) {
		--_current;
	}

	return *_current++;
}

}}
