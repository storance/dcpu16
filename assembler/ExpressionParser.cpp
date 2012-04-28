#include "ExpressionParser.hpp"

#include "ast/RegisterDefinition.hpp"

using namespace std;
using namespace dcpu::ast;
using namespace dcpu::lexer;

namespace dcpu { namespace parser {

	OperatorDefinition::OperatorDefinition(ast::BinaryOperator _operator, function<bool (TokenPtr&)> isNextTokenOperator)
		: _operator(_operator),
		  isNextTokenOperator(isNextTokenOperator),
		  leftRequiresLiteral(true),
		  rightRequiresLiteral(true) {}

	OperatorDefinition::OperatorDefinition(ast::BinaryOperator _operator, function<bool (TokenPtr&)> isNextTokenOperator,
		bool leftRequiresLiteral, bool rightRequiresLiteral) :
			_operator(_operator),
			isNextTokenOperator(isNextTokenOperator),
			leftRequiresLiteral(leftRequiresLiteral),
			rightRequiresLiteral(rightRequiresLiteral) {}

	ExpressionParser::ExpressionParser(TokenIterator& current, TokenIterator end, ErrorHandlerPtr& errorHandler,
		bool insideIndirect, bool allowRegisters) 
		: _current(current),
		  _end(end),
		  _errorHandler(errorHandler),
		  _insideIndirect(insideIndirect),
		  _allowRegisters(allowRegisters),
		  _foundRegister(false) {}

	ExpressionPtr ExpressionParser::parse() {
		return parseBitwiseOrOperation();
	}

	ExpressionPtr ExpressionParser::parseBitwiseOrOperation() {
		return parseBinaryOperation(&ExpressionParser::parseBitwiseXorOperation, {
			OperatorDefinition(BinaryOperator::OR, [] (TokenPtr &token) { return token->isCharacter('|'); })
		});
	}

	ExpressionPtr ExpressionParser::parseBitwiseXorOperation() {
		return parseBinaryOperation(&ExpressionParser::parseBitwiseAndOperation, {
			OperatorDefinition(BinaryOperator::XOR, [] (TokenPtr &token) { return token->isCharacter('^'); })
		});
	}

	ExpressionPtr ExpressionParser::parseBitwiseAndOperation() {
		return parseBinaryOperation(&ExpressionParser::parseBitwiseShiftOperation, {
			OperatorDefinition(BinaryOperator::AND, [] (TokenPtr &token) { return token->isCharacter('&'); })
		});
	}

	ExpressionPtr ExpressionParser::parseBitwiseShiftOperation() {
		return parseBinaryOperation(&ExpressionParser::parseAddOperation, {
			OperatorDefinition(BinaryOperator::SHIFT_LEFT,  [] (TokenPtr &token) { return token->isShiftLeft(); }),
			OperatorDefinition(BinaryOperator::SHIFT_RIGHT, [] (TokenPtr &token) { return token->isShiftRight(); })
		});
	}

	ExpressionPtr ExpressionParser::parseAddOperation() {
		return parseBinaryOperation(&ExpressionParser::parseMultiplyOperation, {
			OperatorDefinition(BinaryOperator::PLUS,  [] (TokenPtr &token) { return token->isCharacter('+'); }, false, false),
			OperatorDefinition(BinaryOperator::MINUS, [] (TokenPtr &token) { return token->isCharacter('-'); }, false, true)
		});
	}

	ExpressionPtr ExpressionParser::parseMultiplyOperation() {
		return parseBinaryOperation(&ExpressionParser::parseUnaryOperation, {
			OperatorDefinition(BinaryOperator::MULTIPLY, [] (TokenPtr &token) { return token->isCharacter('*'); }),
			OperatorDefinition(BinaryOperator::DIVIDE,   [] (TokenPtr &token) { return token->isCharacter('/'); }),
			OperatorDefinition(BinaryOperator::MODULO,   [] (TokenPtr &token) { return token->isCharacter('%'); })
		});
	}

	ExpressionPtr ExpressionParser::parseBinaryOperation(ExpressionParserFunc parseFunc,
		initializer_list<OperatorDefinition> definitions) {

		ExpressionPtr left = (this->*parseFunc)();

		while (true) {
			auto& operatorToken = nextToken();

			const OperatorDefinition *operatorDef = nullptr;
			for (auto& definition : definitions) {
				if (definition.isNextTokenOperator(operatorToken)) {
					operatorDef = &definition;
					break;
				}
			}

			if (operatorDef == nullptr) {
				--_current;
				return move(left);
			}

			ExpressionPtr right = (this->*parseFunc)();
			checkForNonLiteralExpression(operatorDef, left, right);

			left = move(Expression::binaryOperation(operatorToken->location, operatorDef->_operator, left, right));
		}
	}

	void ExpressionParser::checkForNonLiteralExpression(const OperatorDefinition *operatorDef, ExpressionPtr &left,
		ExpressionPtr &right) {

		bool leftLiteralRequired = operatorDef->leftRequiresLiteral || !_insideIndirect;
		bool rightLiteralRequired = operatorDef->rightRequiresLiteral || !_insideIndirect;

		if (leftLiteralRequired && !left->isLiteral()) {
			_errorHandler->error(left->_location, boost::format("The left operand of '%s' must evaluate to a literal.") 
				% str(operatorDef->_operator));
			left = move(Expression::invalid(left->_location));
		}

		if (rightLiteralRequired && !right->isLiteral()) {
			_errorHandler->error(right->_location, boost::format("The right operand of '%s' must evaluate to a literal.") 
				% str(operatorDef->_operator));
			right = move(Expression::invalid(right->_location));
		}
	}

	ExpressionPtr ExpressionParser::parseUnaryOperation() {
		auto& currentToken = nextToken();

		UnaryOperator _operator;
		if (currentToken->isCharacter('+')) {
			_operator = UnaryOperator::PLUS;
		} else if (currentToken->isCharacter('-')) {
			_operator = UnaryOperator::MINUS;
		} else if (currentToken->isCharacter('~')) {
			_operator = UnaryOperator::BITWISE_NOT;
		} else if (currentToken->isCharacter('!')) {
			_operator = UnaryOperator::NOT;
		} else {
			return parsePrimaryExpression(currentToken);
		}

		ExpressionPtr operand = parseUnaryOperation();
		if (!operand->isLiteral()) {
			_errorHandler->error(currentToken->location, boost::format("The operand for unary '%s' must evaluate to "
				"a literal.") % str(_operator));
			return ExpressionPtr(new InvalidExpression(currentToken->location));
		}

		return Expression::unaryOperation(currentToken->location, _operator, operand);
	}

	ExpressionPtr ExpressionParser::parsePrimaryExpression(TokenPtr &currentToken) {
		if (currentToken->isCharacter('(')) {
			return parseGroupedExpression();
		} else if (currentToken->isIdentifier()) {
			return parseIdentifierExpression(currentToken);
		} else if (currentToken->isCharacter('$')) {
			return parseLabelExpression();
		} else if (currentToken->isInteger()) {
			return parseLiteralExpression(currentToken);
		} else {
			_errorHandler->errorUnexpectedToken(currentToken, "a label name, register, or literal");
			return Expression::invalid(currentToken->location);
		}
	}

	ExpressionPtr ExpressionParser::parseGroupedExpression() {
		ExpressionPtr expr = parse();

		auto& nextTkn = nextToken();
		if (!nextTkn->isCharacter(')')) {
			--_current;
			_errorHandler->errorUnexpectedToken(nextTkn, ')');
		}

		return expr;
	}

	ExpressionPtr ExpressionParser::parseIdentifierExpression(TokenPtr& currentToken) {
		auto registerDef = lookupRegister(currentToken->content);
		if (registerDef) {
			if (_foundRegister) {
				_errorHandler->error(currentToken->location, boost::format("Unexpected register '%s'; more than "
					"one register is not allowed in an expression.") % str(registerDef->_register));
				return Expression::invalid(currentToken->location);
			}

			_foundRegister = true;
			if (!_allowRegisters) {
				_errorHandler->error(currentToken->location, boost::format("Unexpected register '%s'; registers "
					"are not allowed here.") % str(registerDef->_register));
				return Expression::invalid(currentToken->location);
			}

			if (_insideIndirect && !registerDef->_indirectable) {
				_errorHandler->error(currentToken->location, boost::format("Unexpected register '%1%'; '%1%' is not "
					"indirectable.") % str(registerDef->_register));
				return Expression::invalid(currentToken->location);
			}

			return Expression::registerOperand(currentToken->location, registerDef->_register);
		}

		return Expression::labelOperand(currentToken->location, currentToken->content);
	}

	ExpressionPtr ExpressionParser::parseLabelExpression() {
		auto& nextTkn = nextToken();

		if (!nextTkn->isIdentifier()) {
			_errorHandler->errorUnexpectedToken(nextTkn, "a label name");

			return Expression::invalid(nextTkn->location);
		}

		return Expression::labelOperand(nextTkn->location, nextTkn->content);
	}

	ExpressionPtr ExpressionParser::parseLiteralExpression(TokenPtr& currentToken) {
		IntegerToken* intToken = asInteger(currentToken);
		if (intToken->overflow) {
			_errorHandler->warning(intToken->location, boost::format(
				"%s is larger than the maximum intermediary value (%d).") % intToken->content % UINT32_MAX);
		}

		return Expression::literalOperand(intToken->location, intToken->value);
	}

	TokenPtr& ExpressionParser::nextToken() {
		return next(_current, _end);
	}
}}