#include "Parser.hpp"

#include <functional>
#include <iostream>
#include <iterator>
#include <map>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "ast/OpcodeDefinition.hpp"
#include "ast/RegisterDefinition.hpp"
#include "SymbolTable.hpp"
#include "ExpressionParser.hpp"

using namespace std;
using namespace std::placeholders;
using namespace dcpu::lexer;
using namespace dcpu::ast;
using namespace boost::algorithm;

namespace dcpu { namespace parser {

Parser::Parser(Iterator start, Iterator end, ErrorHandler &errorHandler, dcpu::SymbolTable &symbolTable)
	: _current(start), _end(end), _errorHandler(errorHandler), _symbolTable(symbolTable), _outputPosition(0) {}

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

		StatementPtr statement;
		if (parseLabel(*currentToken, statement)) {
			addStatement(statement);
			currentToken = &nextToken();
		}

		if (parseInstruction(*currentToken, statement)) {
			addStatement(statement);
		}
	}
}

void Parser::addStatement(StatementPtr& statement) {
	std::uint16_t oldPositiion = _outputPosition;
	try {
		statement->buildSymbolTable(_symbolTable, _outputPosition);
	} catch (std::exception &e) {
		_errorHandler.error(statement->_location, e.what());
	}

	if (_outputPosition < oldPositiion) {
		_errorHandler.error(statement->_location, "binary output has exceeded the DCPU-16's memory "
			"size (65,536 words).");
	}

	statements.push_back(move(statement));
}

bool Parser::parseLabel(TokenPtr& currentToken, StatementPtr& statement) {
	if (currentToken->isCharacter(':')) {
		auto& nextTkn = nextToken();

		if (nextTkn->isIdentifier()) {
			statement = move(StatementPtr(new Label(currentToken->location, nextTkn->content)));
			return true;
		} else if (nextTkn->isStatementTerminator()) {
			_errorHandler.errorUnexpectedToken(nextTkn, "a label name");
			return false;
		} else {
			_errorHandler.errorUnexpectedToken(currentToken, "a label or instruction");
			advanceUntil(mem_fn(&Token::isStatementTerminator));
			return false;
		}
	} else if (currentToken->isIdentifier()) {
		auto& nextTkn = nextToken();
		if (nextTkn->isCharacter(':')) {
			statement = move(StatementPtr(new Label(currentToken->location, currentToken->content)));
			return true;
		} else {
			--_current;
			return false;
		}
	} else {
		_errorHandler.errorUnexpectedToken(currentToken, "a label or instruction");
		advanceUntil(mem_fn(&Token::isStatementTerminator));
		return false;
	}
}

bool Parser::parseInstruction(TokenPtr& currentToken, StatementPtr& statement) {
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
		a = move(parseArgument(nextToken(), ArgumentPosition::A));

		if (!a) {
			advanceUntil(mem_fn(&Token::isStatementTerminator));
			return false;
		}
	} else if (opcodeDef->_args == 2) {
		b = move(parseArgument(nextToken(), ArgumentPosition::B));

		if (!b) {
			advanceUntil([] (const Token& token) {
				return token.isCharacter(',') || token.isStatementTerminator();
			});
		}

		auto& nextTkn = nextToken();
		if (!nextTkn->isCharacter(',')) {
			_errorHandler.errorUnexpectedToken(nextTkn, ',');
			--_current;
		}

		a = parseArgument(nextToken(), ArgumentPosition::A);
		if (!a || !b) {
			advanceUntil(mem_fn(&Token::isStatementTerminator));
			return false;
		}
	}

	auto& eolToken = nextToken();
	if (!eolToken->isStatementTerminator()) {
		_errorHandler.errorUnexpectedToken(eolToken, "'newline' or 'eof'");
		advanceUntil(mem_fn(&Token::isStatementTerminator));
	}

	statement = move(StatementPtr(new Instruction(currentToken->location, opcodeDef->_opcode, a, b)));
	return true;
}

ArgumentPtr Parser::parseArgument(TokenPtr& currentToken, ArgumentPosition position) {
	if (currentToken->isCharacter(',') || currentToken->isStatementTerminator()) {
		_errorHandler.errorUnexpectedToken(currentToken, "an instruction argument");
		return Argument::null();
	}

	if (currentToken->isCharacter('[')) {
		auto argument = parseIndirectStackArgument(nextToken(), position);
		if (!argument) {
			--_current;
			argument = move(Argument::indirect(position, parseExpression(true, true)));
		}

		auto &nxtToken = nextToken();
		if (!nxtToken->isCharacter(']')) {
			--_current;
			_errorHandler.errorUnexpectedToken(nxtToken, ']');
			return Argument::null();
		}

		return argument;
	} else if (currentToken->isIdentifier()) {
		auto stackArgument = parseMnemonicStackArgument(currentToken, position);
		if (stackArgument) {
			return stackArgument;
		}
	}

	--_current;
	return Argument::expression(position, parseExpression(true, false));
}

ArgumentPtr Parser::parseIndirectStackArgument(TokenPtr& currentToken, ArgumentPosition position) {
	if (currentToken->isIdentifier() && iequals(currentToken->content, "SP")) {

		auto& nxtToken = nextToken();
		if (nxtToken->isCharacter(']')) {
			--_current;
			return Argument::stackPeek(currentToken->location, position);
		} else if (nxtToken->isIncrement()) {
			if (position != ArgumentPosition::A) {
				_errorHandler.error(currentToken->location, "[SP++] is not allowed as argument b.");
			}

			return Argument::stackPop(currentToken->location, position);
		}

		--_current;
	} else if (currentToken->isDecrement()) {
		auto& nxtToken = nextToken();
		if (nxtToken->isIdentifier() && iequals(nxtToken->content, "SP")) {
			if (position != ArgumentPosition::B) {
				_errorHandler.error(currentToken->location, "[--SP] is not allowed as argument a.");
			}

			return Argument::stackPush(currentToken->location, position);
		}

		--_current;
	}

	return Argument::null();
}

ArgumentPtr Parser::parseMnemonicStackArgument(TokenPtr& currentToken, ArgumentPosition position) {
	if (iequals(currentToken->content, "PUSH")) {
		if (position != ArgumentPosition::B) {
			_errorHandler.error(currentToken->location, "PUSH is not allowed as argument a.");
		}

		return Argument::stackPush(currentToken->location, position);
	} else if (iequals(currentToken->content, "POP")) {
		if (position != ArgumentPosition::A) {
			_errorHandler.error(currentToken->location, "POP is not allowed as argument a.");
		}

		return Argument::stackPop(currentToken->location, position);
	} else if (iequals(currentToken->content, "PEEK")) {
		return Argument::stackPeek(currentToken->location, position);
	} else if (iequals(currentToken->content, "PICK")) {
		auto rightOperand = parseExpression(false, false);

		if (rightOperand->isEvaluated()) {
			return Argument::indirect(position, Expression::evaluatedRegister(
				currentToken->location, Register::SP, rightOperand->getEvaluatedValue()));
		} else {
			auto leftOperand = Expression::registerOperand(currentToken->location, Register::SP);

			return Argument::indirect(position, Expression::binaryOperation(currentToken->location,
				BinaryOperator::PLUS, leftOperand, rightOperand));
		}
	}

	return Argument::null();
}

ExpressionPtr Parser::parseExpression(bool allowRegisters, bool insideIndirection) {
	ExpressionParser parser(_current, _end, _errorHandler, insideIndirection, allowRegisters);
	auto expr = parser.parse();

	if (expr->isEvaluatable() && !expr->isEvaluated()) {
		return expr->evaluate();
	}

	return expr;
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
