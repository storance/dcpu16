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
		shared_ptr<Token> currentToken = nextToken();

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

bool Parser::parseLabel(shared_ptr<Token> currentToken) {
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
		_errorHandler.error(currentToken->location, boost::format(
			"Unexpected token '%s', expected a label or instruction.") % currentToken->content);
		advanceUntil(mem_fn(&Token::isStatementTerminator));
		return false;
	}
}

bool Parser::parseInstruction(shared_ptr<Token> currentToken) {
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

	shared_ptr<Argument> a, b;
	if (opcodeDef->_args > 0) {
		if (!parseArgument(nextToken(), a)) {
			advanceUntil([] (shared_ptr<Token> token) {
				return token->isCharacter(',') || token->isStatementTerminator();
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

	auto eolToken = nextToken();
	if (!eolToken->isStatementTerminator()) {
		_errorHandler.errorUnexpectedToken(eolToken, "a 'newline' or 'eof'");
		advanceUntil(mem_fn(&Token::isStatementTerminator));
	}

	addInstruction(currentToken->location, opcodeDef->_opcode, a, b);
	return true;
}

bool Parser::parseArgument(shared_ptr<Token> currentToken, shared_ptr<ast::Argument>& arg) {
	if (currentToken->isCharacter(',') || currentToken->isStatementTerminator()) {
		_errorHandler.errorUnexpectedToken(currentToken, "an instruction argument");
		return false;
	}

	if (currentToken->isCharacter('[')) {
		auto startPos = _current;
		if (!parseIndirectStackArgument(nextToken(), arg)) {
			_current = startPos;
			auto expr = parseExpression(currentToken, true);
			arg = make_shared<IndirectArgument>(expr);
		}

		if (!isNextTokenChar(']')) {
			_errorHandler.errorUnexpectedToken(*_current, ']');
			return false;
		}

		nextToken();
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
	if (expr->getType() != ExpressionType::REGISTER && !expr->isEvalsToLiteral()) {
		_errorHandler.error(expr->_location, "Expressions involving registers are not supported outside of "
			"an indirection");
		return false;
	}
	arg = make_shared<ExpressionArgument>(expr);
	return true;
}

bool Parser::parseIndirectStackArgument(shared_ptr<Token> currentToken, shared_ptr<ast::Argument>& arg) {
	if (currentToken->isIdentifier()) {
		if (!boost::algorithm::iequals(currentToken->content, "SP")) {
			return false;
		}

		if (isNextTokenChar(']')) {
			arg = make_shared<StackArgument>(currentToken->location, StackOperation::PEEK);
			return true;
		} else if (isNextToken(mem_fn(&Token::isIncrement))) {
			nextToken();
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

shared_ptr<Expression> Parser::parseExpression(shared_ptr<Token> currentToken, bool indirect) {
	return parseBitwiseOrOperation(currentToken, indirect);
}

shared_ptr<Expression> Parser::parseBitwiseOrOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseBitwiseXorOperation(currentToken, indirect);

	BinaryOperator _operator;
	if (isNextTokenChar('|')) {
		_operator = BinaryOperator::OR;
	} else {
		return left;
	}

	auto operatorToken = nextToken();
	auto right = parseBitwiseXorOperation(nextToken(), indirect);
	if (!left->isEvalsToLiteral() || !right->isEvalsToLiteral()) {
		_errorHandler.error(operatorToken->location, boost::format("Operator '%s' may only be used in expressions "
			"that evaluate to a literal.") % str(_operator));
		return make_shared<InvalidExpression>(currentToken->location);
	}

	return make_shared<BinaryOperation>(operatorToken->location, _operator, left, right);
}

shared_ptr<Expression> Parser::parseBitwiseXorOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseBitwiseAndOperation(currentToken, indirect);

	BinaryOperator _operator;
	if (isNextTokenChar('^')) {
		_operator = BinaryOperator::XOR;
	} else {
		return left;
	}

	auto operatorToken = nextToken();
	auto right = parseBitwiseAndOperation(nextToken(), indirect);
	if (!left->isEvalsToLiteral() || !right->isEvalsToLiteral()) {
		_errorHandler.error(operatorToken->location, boost::format("Operator '%s' may only be used in expressions "
			"that evaluate to a literal.") % str(_operator));
		return make_shared<InvalidExpression>(currentToken->location);
	}

	return make_shared<BinaryOperation>(operatorToken->location, _operator, left, right);
}

shared_ptr<Expression> Parser::parseBitwiseAndOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseBitwiseShiftOperation(currentToken, indirect);

	BinaryOperator _operator;
	if (isNextTokenChar('&')) {
		_operator = BinaryOperator::AND;
	} else {
		return left;
	}

	auto operatorToken = nextToken();
	auto right = parseBitwiseShiftOperation(nextToken(), indirect);
	if (!left->isEvalsToLiteral() || !right->isEvalsToLiteral()) {
		_errorHandler.error(operatorToken->location, boost::format("Operator '%s' may only be used in expressions "
			"that evaluate to a literal.") % str(_operator));
		return make_shared<InvalidExpression>(currentToken->location);
	}

	return make_shared<BinaryOperation>(operatorToken->location, _operator, left, right);
}

shared_ptr<Expression> Parser::parseBitwiseShiftOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseAddOperation(currentToken, indirect);

	BinaryOperator _operator;
	if (isNextToken(mem_fn(&Token::isShiftLeft))) {
		_operator = BinaryOperator::SHIFT_LEFT;
	} else if (isNextToken(mem_fn(&Token::isShiftRight))) {
		_operator = BinaryOperator::SHIFT_RIGHT;
	} else {
		return left;
	}

	auto operatorToken = nextToken();
	auto right = parseAddOperation(nextToken(), indirect);
	if (!left->isEvalsToLiteral() || !right->isEvalsToLiteral()) {
		_errorHandler.error(operatorToken->location, boost::format("Operator '%s' may only be used in expressions "
			"that evaluate to a literal.") % str(_operator));
		return make_shared<InvalidExpression>(currentToken->location);
	}

	return make_shared<BinaryOperation>(operatorToken->location, _operator, left, right);
}

shared_ptr<Expression> Parser::parseAddOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseMultiplyOperation(currentToken, indirect);

	BinaryOperator _operator;
	if (isNextTokenChar('+')) {
		_operator = BinaryOperator::PLUS;
	} else if (isNextTokenChar('-')) {
		_operator = BinaryOperator::MINUS;
	} else {
		return left;
	}

	auto operatorToken = nextToken();
	auto right = parseMultiplyOperation(nextToken(), indirect);

	if (!left->isEvalsToLiteral()) {
		_errorHandler.error(operatorToken->location, boost::format("The expression to the left of the operator "
			"'-' must evaluate to a literal."));
		return make_shared<InvalidExpression>(currentToken->location);
	}

	return make_shared<BinaryOperation>(operatorToken->location, _operator, left, right);
}

shared_ptr<Expression> Parser::parseMultiplyOperation(shared_ptr<Token> currentToken, bool indirect) {
	auto left = parseUnaryOperation(currentToken, indirect);

	BinaryOperator _operator;
	if (isNextTokenChar('*')) {
		_operator = BinaryOperator::MULTIPLY;
	} else if (isNextTokenChar('/')) {
		_operator = BinaryOperator::DIVIDE;
	} else if (isNextTokenChar('%')) {
		_operator = BinaryOperator::MODULO;
	} else {
		return left;
	}

	auto operatorToken = nextToken();
	auto right = parseUnaryOperation(nextToken(), indirect);
	if (!left->isEvalsToLiteral() || !right->isEvalsToLiteral()) {
		_errorHandler.error(operatorToken->location, boost::format("Operator '%s' may only be used in expressions "
			"that evaluate to a literal.") % str(_operator));
		return make_shared<InvalidExpression>(currentToken->location);
	}

	return make_shared<BinaryOperation>(operatorToken->location, _operator, left, right);
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

	auto operand = parsePrimaryExpression(nextToken(), indirect);
	if (_operator != UnaryOperator::PLUS && !operand->isEvalsToLiteral()) {
		_errorHandler.error(currentToken->location, boost::format("Unary '%s' may only be used with an expression that "
			"evaluates to a literal.") % str(_operator));
		return make_shared<InvalidExpression>(currentToken->location);
	}

	return make_shared<UnaryOperation>(currentToken->location, _operator, operand);
}

shared_ptr<Expression> Parser::parsePrimaryExpression(shared_ptr<Token> currentToken, bool indirect) {
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
		return make_shared<InvalidExpression>(currentToken->location);
	}
}

shared_ptr<Expression> Parser::parseGroupedExpression(shared_ptr<Token> currentToken, bool indirect) {
	auto expr = parseExpression(currentToken, indirect);

	if (!isNextTokenChar(')')) {
		_errorHandler.errorUnexpectedToken(*_current, ')');
	} else {
		nextToken();
	}

	return expr;
}

shared_ptr<Expression> Parser::parseIdentifierExpression(shared_ptr<Token> currentToken, bool indirect) {
	auto registerDef = lookupRegister(currentToken->content);
	if (registerDef) {
		if (indirect && !registerDef->_indirectable) {
			_errorHandler.error(currentToken->location, boost::format("Register %s can't be used in an indirection") 
				% registerDef->_name);
			return make_shared<InvalidExpression>(currentToken->location);
		}
		
		return make_shared<RegisterOperand>(currentToken->location, registerDef->_register);
	}

	return make_shared<LabelReferenceOperand>(currentToken);
}

shared_ptr<Expression> Parser::parseLabelExpression(shared_ptr<Token> currentToken) {
	if (!isNextToken(mem_fn(&Token::isIdentifier))) {
		_errorHandler.errorUnexpectedToken(*_current, "a label name");

		return make_shared<InvalidExpression>(currentToken->location);	
	}

	return make_shared<LabelReferenceOperand>(nextToken());
}

shared_ptr<Expression> Parser::parseLiteralExpression(shared_ptr<Token> currentToken) {
	shared_ptr<IntegerToken> intToken = asInteger(currentToken);
	if (intToken->overflow) {
		_errorHandler.warning(intToken->location, boost::format(
			"%s is larger than the maximum intermediary value (%d).") % intToken->content % UINT32_MAX);
	}

	return make_shared<LiteralOperand>(intToken->location, intToken->value);
}

void Parser::addLabel(const Location& location, const string &labelName) {
	_statements.push_back(make_shared<ast::Label>(location, labelName));
}

void Parser::addInstruction(const Location& location, ast::Opcode opcode, shared_ptr<ast::Argument> a,
	shared_ptr<ast::Argument> b) {

	_statements.push_back(make_shared<ast::Instruction>(location, opcode, a, b));
}

template<typename Predicate> void Parser::advanceUntil(Predicate predicate) {
	while (_current != _end) {
		if (predicate(*_current)) {
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
	return isNextToken([=](shared_ptr<Token> token) {
		return token->isCharacter(c);
	});
}

template<typename Predicate> bool Parser::isNextToken(Predicate predicate) {
	if (_current == _end) {
		return false;
	}

	return predicate(*_current);
}

shared_ptr<Token> Parser::nextToken() {
	if (_current == _end) {
		--_current;
	}

	return *_current++;
}

}}
