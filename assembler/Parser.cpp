#include "Parser.hpp"

#include <functional>
#include <iostream>
#include <iterator>
#include <map>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "ast/OpcodeDefinition.hpp"
#include "ast/RegisterDefinition.hpp"
#include "ExpressionParser.hpp"

using namespace std;
using namespace std::placeholders;
using namespace dcpu::lexer;
using namespace dcpu::ast;

namespace dcpu { namespace parser {

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
	if (opcodeDef->_args == 1) {
		if (!parseArgument(nextToken(), a, ArgumentPosition::A)) {
			advanceUntil(mem_fn(&Token::isStatementTerminator));
			return false;
		}
	} else if (opcodeDef->_args == 2) {
		bool anyFailure = false;
		if (!parseArgument(nextToken(), b, ArgumentPosition::B)) {
			advanceUntil([] (const Token& token) {
				return token.isCharacter(',') || token.isStatementTerminator();
			});
			anyFailure = true;
		}

		auto& nextTkn = nextToken();
		if (!nextTkn->isCharacter(',')) {
			_errorHandler.errorUnexpectedToken(nextTkn, ',');
			--_current;
			anyFailure = true;
		}

		if (!parseArgument(nextToken(), a, ArgumentPosition::A)) {
			advanceUntil(mem_fn(&Token::isStatementTerminator));
			anyFailure = true;
		}

		if (anyFailure) {
			return false;
		}
	}

	auto& eolToken = nextToken();
	if (!eolToken->isStatementTerminator()) {
		_errorHandler.errorUnexpectedToken(eolToken, "'newline' or 'eof'");
		advanceUntil(mem_fn(&Token::isStatementTerminator));
	}

	addInstruction(currentToken->location, opcodeDef->_opcode, a, b);
	return true;
}

bool Parser::parseArgument(TokenPtr& currentToken, ArgumentPtr& arg, ArgumentPosition position) {
	if (currentToken->isCharacter(',') || currentToken->isStatementTerminator()) {
		_errorHandler.errorUnexpectedToken(currentToken, "an instruction argument");
		return false;
	}

	if (currentToken->isCharacter('[')) {
		auto startPos = _current;
		if (!parseIndirectStackArgument(nextToken(), arg, position)) {
			_current = startPos;
			arg = move(ArgumentPtr(new IndirectArgument(position, parseExpression(true, true))));
		}

		if (!isNextTokenChar(']')) {
			_errorHandler.errorUnexpectedToken(*_current, ']');
			return false;
		}

		nextToken();
		return true;
	}

	if (parseMnemonicStackArgument(currentToken, arg, position)) {
		return true;
	}

	--_current;
	ExpressionPtr expr = move(parseExpression(true, false));
	arg = move(ArgumentPtr(new ExpressionArgument(position, expr)));
	return true;
}

bool Parser::parseIndirectStackArgument(TokenPtr& currentToken, ArgumentPtr& arg, ArgumentPosition position) {
	if (currentToken->isIdentifier()) {
		if (!boost::algorithm::iequals(currentToken->content, "SP")) {
			return false;
		}

		if (isNextTokenChar(']')) {
			arg = move(ArgumentPtr(new StackArgument(currentToken->location, position, StackOperation::PEEK)));
			return true;
		} else if (isNextToken(mem_fn(&Token::isIncrement))) {
			if (position != ArgumentPosition::A) {
				_errorHandler.error(currentToken->location, boost::format("[SP++] is not allowed as argument %s.")
					% str(position));
			}

			nextToken();
			arg = move(ArgumentPtr(new StackArgument(currentToken->location, position, StackOperation::POP)));
			return true;
		}

		return false;
	} else if (currentToken->isDecrement()) {
		auto& nxtToken = nextToken();
		if (nxtToken->isIdentifier() && boost::algorithm::iequals(nxtToken->content, "SP")) {
			if (position != ArgumentPosition::B) {
				_errorHandler.error(currentToken->location, boost::format("[--SP] is not allowed as argument %s.")
					% str(position));
			}

			arg = move(ArgumentPtr(new StackArgument(currentToken->location, position, StackOperation::PUSH)));
			return true;
		}

		return false;
	}

	return false;
}

bool Parser::parseMnemonicStackArgument(TokenPtr& currentToken, ArgumentPtr& arg, ArgumentPosition position) {
	if (!currentToken->isIdentifier()) {
		return false;
	}

	if (boost::algorithm::iequals(currentToken->content, "PUSH")) {
		if (position != ArgumentPosition::B) {
			_errorHandler.error(currentToken->location, boost::format("PUSH is not allowed as argument %s.")
				% str(position));
		}
		arg = move(ArgumentPtr(new StackArgument(currentToken->location, position, StackOperation::PUSH)));
		return true;
	} else if (boost::algorithm::iequals(currentToken->content, "POP")) {
		if (position != ArgumentPosition::A) {
			_errorHandler.error(currentToken->location, boost::format("POP is not allowed as argument %s.")
				% str(position));
		}
		arg = move(ArgumentPtr(new StackArgument(currentToken->location, position, StackOperation::POP)));
		return true;
	} else if (boost::algorithm::iequals(currentToken->content, "PEEK")) {
		arg = move(ArgumentPtr(new StackArgument(currentToken->location, position, StackOperation::PEEK)));
		return true;
	} else if (boost::algorithm::iequals(currentToken->content, "PICK")) {
		auto leftOperand = ExpressionPtr(new RegisterOperand(currentToken->location, Register::SP));
		auto rightOperand = parseExpression(false, false);

		arg = move(ArgumentPtr(new IndirectArgument(position,
			ExpressionPtr(new BinaryOperation(
				currentToken->location, BinaryOperator::PLUS, leftOperand, rightOperand)
			)
		)));
		return true;
	}

	return false;
}

ExpressionPtr Parser::parseExpression(bool allowRegisters, bool insideIndirection) {
	ExpressionParser parser(_current, _end, _errorHandler, insideIndirection, allowRegisters);
	return parser.parse();
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
	return next(_current, _end);
}

}}
