#include "ExpressionParser.hpp"

#include "ast/RegisterDefinition.hpp"

using namespace std;
using namespace dcpu::ast;
using namespace dcpu::lexer;
using namespace boost;

const string UNARY_OPERAND_NOT_LITERAL = "The operand for unary '%s' must evaluate to a literal";
const string LEFT_OPERAND_NOT_LITERAL = "The left operand of '%s' must evaluate to a literal";
const string RIGHT_OPERAND_NOT_LITERAL = "The right operand of '%s' must evaluate to a literal";
const string LABEL_NOT_ALLOWED = "Unexpected label reference '%s'; label references are not allowed here";
const string REGISTER_NOT_ALLOWED = "Unexpected register '%s'; registers are not allowed here.";
const string REGISTER_NOT_INDIRECTABLE = "Unexpected register '%1%'; '%1%' is not indirectable.";
const string MULTIPLE_REGISTERS = "Unexpected register '%s'; register '%s' was already used in this expression at %s";
const string LITERAL_OVERFLOW = "literal %s is larger than the maximum intermediary value (%d).";

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


	FoundRegister::FoundRegister() : found(false) {}

	void FoundRegister::set(Register _register, const Location &location) {
		found = true;
		this->_register = _register;
		this->location = location;
	}

	FoundRegister::operator bool() {
		return found;
	}

	ExpressionParser::ExpressionParser(TokenIterator& current, TokenIterator end, ErrorHandlerPtr& errorHandler,
		bool registersAllowed, bool labelsAllowed, bool indirection) 
		: current(current),
		  end(end),
		  errorHandler(errorHandler),
		  labelsAllowed(labelsAllowed),
		  registersAllowed(registersAllowed),
		  indirection(indirection),
		  foundRegister() {}

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

			const OperatorDefinition *operatorDefinition = nullptr;
			for (auto& definition : definitions) {
				if (definition.isNextTokenOperator(operatorToken)) {
					operatorDefinition = &definition;
					break;
				}
			}

			if (operatorDefinition == nullptr) {
				--current;
				return move(left);
			}

			ExpressionPtr right = (this->*parseFunc)();
			checkForNonLiteralExpression(operatorDefinition, left, right);

			left = move(Expression::binaryOperation(operatorToken->location, operatorDefinition->_operator,
				left, right));
		}
	}

	void ExpressionParser::checkForNonLiteralExpression(const OperatorDefinition *definition, ExpressionPtr &left,
		ExpressionPtr &right) {

		bool leftLiteralRequired = definition->leftRequiresLiteral || !indirection;
		bool rightLiteralRequired = definition->rightRequiresLiteral || !indirection;

		if (leftLiteralRequired && !left->isLiteral()) {
			errorHandler->error(left->_location, format(LEFT_OPERAND_NOT_LITERAL) % str(definition->_operator));
			left = move(Expression::invalid(left->_location));
		}

		if (rightLiteralRequired && !right->isLiteral()) {
			errorHandler->error(right->_location, format(RIGHT_OPERAND_NOT_LITERAL) % str(definition->_operator));
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
			errorHandler->error(currentToken->location, format(UNARY_OPERAND_NOT_LITERAL) % str(_operator));
			return Expression::invalid(currentToken->location);
		}

		return Expression::unaryOperation(currentToken->location, _operator, operand);
	}

	ExpressionPtr ExpressionParser::parsePrimaryExpression(TokenPtr &currentToken) {
		if (currentToken->isCharacter('(')) {
			return parseGroupedExpression();
		} else if (currentToken->isIdentifier()) {
			return parseIdentifierExpression(currentToken);
		} else if (currentToken->isCharacter('$')) {
			return parseLabelExpression(nextToken());
		} else if (currentToken->isInteger()) {
			return parseLiteralExpression(currentToken);
		} else {
			errorHandler->errorUnexpectedToken(currentToken, "a label name, register, or literal");
			return Expression::invalid(currentToken->location);
		}
	}

	ExpressionPtr ExpressionParser::parseGroupedExpression() {
		ExpressionPtr expr = parse();

		auto& nextTkn = nextToken();
		if (!nextTkn->isCharacter(')')) {
			--current;
			errorHandler->errorUnexpectedToken(nextTkn, ')');
		}

		return expr;
	}

	ExpressionPtr ExpressionParser::parseIdentifierExpression(TokenPtr& currentToken) {
		auto registerDef = lookupRegister(currentToken->content);
		if (registerDef) {
			if (foundRegister) {
				errorHandler->error(currentToken->location, format(MULTIPLE_REGISTERS) 
					% str(registerDef->_register)
					% str(foundRegister._register)
					% str(foundRegister.location));
				return Expression::invalid(currentToken->location);
			}

			foundRegister.set(registerDef->_register, currentToken->location);
			if (!registersAllowed) {
				errorHandler->error(currentToken->location, format(REGISTER_NOT_ALLOWED) % str(registerDef->_register));
				return Expression::invalid(currentToken->location);
			}

			if (indirection && !registerDef->_indirectable) {
				errorHandler->error(currentToken->location, format(REGISTER_NOT_INDIRECTABLE) 
					% str(registerDef->_register));
				return Expression::invalid(currentToken->location);
			}

			return Expression::registerOperand(currentToken->location, registerDef->_register);
		}

		return parseLabelExpression(currentToken);
	}

	ExpressionPtr ExpressionParser::parseLabelExpression(TokenPtr& currentToken) {
		if (!labelsAllowed) {
			errorHandler->error(currentToken->location, format(LABEL_NOT_ALLOWED) % currentToken->content);
			return Expression::invalid(currentToken->location);
		}

		if (!currentToken->isIdentifier()) {
			errorHandler->errorUnexpectedToken(currentToken, "a label name");

			return Expression::invalid(currentToken->location);
		}

		return Expression::labelOperand(currentToken->location, currentToken->content);
	}

	ExpressionPtr ExpressionParser::parseLiteralExpression(TokenPtr& currentToken) {
		IntegerToken* intToken = asInteger(currentToken);
		if (intToken->overflow) {
			errorHandler->warning(intToken->location, format(LITERAL_OVERFLOW) % intToken->content % UINT32_MAX);
		}

		return Expression::literalOperand(intToken->location, intToken->value);
	}

	TokenPtr& ExpressionParser::nextToken() {
		return next(current, end);
	}
}}