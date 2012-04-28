#include "Expression.hpp"

#include <stdexcept>

#include <boost/format.hpp>

using namespace std;
using namespace dcpu::lexer;

namespace dcpu { namespace ast {
	/*************************************************************************
	 *
	 * Expression
	 *
	 *************************************************************************/
	Expression::Expression(const Location& location) : _location(location) {}

	Expression::Expression(Expression&& other) : _location(move(other._location)) {}

	Expression::~Expression() {}

	uint8_t Expression::compile(std::vector<std::uint16_t> &output, ArgumentPosition position, bool indirect,
			bool forceNextWord) {
		throw logic_error("Expression must be evaluated before it can be compiled");
	}

	/*************************************************************************
	 *
	 * UnaryOperation
	 *
	 *************************************************************************/

	UnaryOperation::UnaryOperation(const Location& location, UnaryOperator op, ExpressionPtr& operand)
		: Expression(location), _operator(op), _operand(move(operand)) {

		_cachedEvalsToLiteral = _operand->isEvalsToLiteral();
		_cachedEvaluatable = _operand->isEvaluatable();
	}

	bool UnaryOperation::isEvalsToLiteral() const {
		return _cachedEvalsToLiteral;
	}

	bool UnaryOperation::isEvaluatable() const {
		return _cachedEvaluatable;
	}
	
	bool UnaryOperation::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return _operand->isNextWordRequired(position, forceNextWord);
	}

	EvaluatedExpressionPtr UnaryOperation::evaluate() const {
		auto evaluatedOperand = _operand->evaluate();
		if (evaluatedOperand->isRegister()) {
			throw new logic_error(boost::str(boost::format("Unary '%s' not supported on a register") 
				% ast::str(_operator)));
		}
		
		switch (_operator) {
		case UnaryOperator::PLUS:
			evaluatedOperand->setValue(+evaluatedOperand->getValue());
			break;
		case UnaryOperator::MINUS:
			evaluatedOperand->setValue(-evaluatedOperand->getValue());
			break;
		case UnaryOperator::NOT:
			evaluatedOperand->setValue(~evaluatedOperand->getValue());
			break;
		case UnaryOperator::BITWISE_NOT:
			evaluatedOperand->setValue(~evaluatedOperand->getValue());
			break;
		}

		return evaluatedOperand;
	}

	string UnaryOperation::str() const {
		return (boost::format("%s(%s)") % ast::str(_operator) % _operand->str()).str();
	}

	/*************************************************************************
	 *
	 * BinaryOperation
	 *
	 *************************************************************************/

	BinaryOperation::BinaryOperation(const Location& location, BinaryOperator op, ExpressionPtr& left,
		ExpressionPtr& right) : Expression(location), _operator(op), _left(move(left)), _right(move(right)) {
		_cachedEvalsToLiteral = _left->isEvalsToLiteral() && _right->isEvalsToLiteral();
		_cachedEvaluatable = _left->isEvaluatable() && _right->isEvaluatable();
	}

	bool BinaryOperation::isEvalsToLiteral() const {
		return _cachedEvalsToLiteral;
	}

	bool BinaryOperation::isEvaluatable() const {
		return _cachedEvaluatable;
	}
	
	bool BinaryOperation::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		// until we can evaluate the expression, assume we have to use the next word
		return true;
	}

	EvaluatedExpressionPtr BinaryOperation::evaluate() const {
		auto evaluatedLeft = _left->evaluate();
		auto evaluatedRight = _right->evaluate();

		int32_t leftValue = evaluatedLeft->getValue();
		int32_t rightValue = evaluatedRight->getValue();
		int32_t resultValue = 0;

		if (_operator != BinaryOperator::PLUS && _operator != BinaryOperator::MINUS && evaluatedLeft->isRegister()) {
			throw new logic_error(boost::str(boost::format("The left operand of '%s' must not be a register") 
				% ast::str(_operator)));
		}

		if (_operator != BinaryOperator::PLUS && evaluatedRight->isRegister()) {
			throw new logic_error(boost::str(boost::format("The right operand of '%s' must not be a register") 
				% ast::str(_operator)));
		}

		switch (_operator) {
		case BinaryOperator::PLUS:
			resultValue = leftValue + rightValue;
			break;
		case BinaryOperator::MINUS:
			resultValue = leftValue - rightValue;
			break;
		case BinaryOperator::MULTIPLY:
			resultValue = leftValue * rightValue;
			break;
		case BinaryOperator::DIVIDE:
			resultValue = leftValue / rightValue;
			break;
		case BinaryOperator::MODULO:
			resultValue = leftValue % rightValue;
			break;
		case BinaryOperator::SHIFT_LEFT:
			resultValue = leftValue << rightValue;
			break;
		case BinaryOperator::SHIFT_RIGHT:
			resultValue = leftValue >> rightValue;
			break;
		case BinaryOperator::AND:
			resultValue = leftValue & rightValue;
			break;
		case BinaryOperator::OR:
			resultValue = leftValue | rightValue;
			break;
		case BinaryOperator::XOR:
			resultValue = leftValue ^ rightValue;
			break;
		}

		if (evaluatedRight->isRegister()) {
			evaluatedRight->setValue(resultValue);

			return evaluatedRight;
		} else {
			evaluatedLeft->setValue(resultValue);

			return evaluatedLeft;
		}
	}

	string BinaryOperation::str() const {
		return (boost::format("(%s %s %s)") % _left->str() % ast::str(_operator) % _right->str()).str();
	}

	/*************************************************************************
	 *
	 * RegisterOperand
	 *
	 *************************************************************************/

	RegisterOperand::RegisterOperand(const Location& location, Register reg) 
		: Expression(location), _register(reg) {}

	bool RegisterOperand::isEvalsToLiteral() const {
		return false;
	}

	bool RegisterOperand::isEvaluatable() const {
		return true;
	}
	
	bool RegisterOperand::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return false;
	}

	EvaluatedExpressionPtr RegisterOperand::evaluate() const {
		return EvaluatedExpressionPtr(new EvaluatedRegister(_location, _register, false, 0));
	}

	string RegisterOperand::str() const {
		return ast::str(_register);
	}

	/*************************************************************************
	 *
	 * LiteralOperand
	 *
	 *************************************************************************/

	LiteralOperand::LiteralOperand(const Location& location, uint32_t value)
		: Expression(location), _value(value) {}

	bool LiteralOperand::isEvalsToLiteral() const {
		return true;
	}

	bool LiteralOperand::isEvaluatable() const {
		return true;
	}
	
	bool LiteralOperand::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		if (position == ArgumentPosition::B || forceNextWord) {
			return true;
		} else if ((int32_t)_value == -1 || _value <= 0x1e) {
			return false;
		}
		return true;
	}

	EvaluatedExpressionPtr LiteralOperand::evaluate() const {
		return EvaluatedExpressionPtr(new EvaluatedLiteral(_location, _value));
	}

	string LiteralOperand::str() const {
		return (boost::format("%d") % _value).str();
	}

	/*************************************************************************
	 *
	 * LabelReferenceOperand
	 *
	 *************************************************************************/

	LabelReferenceOperand::LabelReferenceOperand(const Location& location, const string& label)
		: Expression(location), _label(label), _position(nullptr) {}

	LabelReferenceOperand::LabelReferenceOperand(TokenPtr& token)
		:Expression(token->location), _label(token->content), _position(nullptr) {}

	bool LabelReferenceOperand::isEvalsToLiteral() const {
		return true;
	}

	bool LabelReferenceOperand::isEvaluatable() const {
		return _position != nullptr;
	}
	
	bool LabelReferenceOperand::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return true;
	}

	EvaluatedExpressionPtr LabelReferenceOperand::evaluate() const {
		if (_position != nullptr) {
			return EvaluatedExpressionPtr(new EvaluatedLiteral(_location, *_position));
		} else {
			return EvaluatedExpressionPtr();
		}
	}

	string LabelReferenceOperand::str() const {
		return _label;
	}

	/*************************************************************************
	 *
	 * InvalidExpression
	 *
	 *************************************************************************/

	InvalidExpression::InvalidExpression(const Location& location) : Expression(location) {}

	bool InvalidExpression::isEvalsToLiteral() const {
		return true;
	}

	bool InvalidExpression::isEvaluatable() const {
		return false;
	}
	
	bool InvalidExpression::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return false;
	}

	EvaluatedExpressionPtr InvalidExpression::evaluate() const {
		return EvaluatedExpressionPtr();
	}

	string InvalidExpression::str() const {
		return "<Invalid Expression>";
	}

	/*************************************************************************
	 *
	 * EvaluatedExpression
	 *
	 *************************************************************************/

	EvaluatedExpression::EvaluatedExpression(const lexer::Location& location, int32_t value) 
		: Expression(location), _value(value) {}

	bool EvaluatedExpression::isEvaluatable() const {
		return true;
	}

	int32_t EvaluatedExpression::getValue() {
		return _value;
	}

	void EvaluatedExpression::setValue(int32_t newValue) {
		_value = newValue;
	}

	/*************************************************************************
	 *
	 * EvaluatedLiteral
	 *
	 *************************************************************************/

	EvaluatedLiteral::EvaluatedLiteral(const lexer::Location& location, int32_t value) 
		: EvaluatedExpression(location, value) {}

	bool EvaluatedLiteral::isRegister() const {
		return false;
	}

	bool EvaluatedLiteral::isEvalsToLiteral() const {
		return true;
	}

	EvaluatedExpressionPtr EvaluatedLiteral::evaluate() const {
		return EvaluatedExpressionPtr(new EvaluatedLiteral(_location, _value));
	}

	uint8_t EvaluatedLiteral::compile(std::vector<std::uint16_t> &output, ArgumentPosition position, bool indirect,
			bool forceNextWord) {
		if (!indirect && !forceNextWord && _value >= -1 && _value <= 0x1e) {
			return (uint8_t)(_value + 0x20);
		}

		output.push_back((uint16_t)_value);
		if (indirect) {
			return 0x1e;
		} else {
			return 0x1f;
		}
	}

	bool EvaluatedLiteral::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		if (position == ArgumentPosition::B || forceNextWord) {
			return true;
		}

		return _value < -1 || _value > 0x1e;
	}

	string EvaluatedLiteral::str() const {
		return (boost::format("%d") % _value).str();
	}

	/*************************************************************************
	 *
	 * EvaluatedRegister
	 *
	 *************************************************************************/

	EvaluatedRegister::EvaluatedRegister(const lexer::Location& location, Register _register, bool hasOffset,
		int32_t offset) : EvaluatedExpression(location, offset), _register(_register), _hasOffset(hasOffset) {}

	void EvaluatedRegister::setValue(int32_t newValue) {
		_hasOffset = true;
		_value = newValue;
	}

	bool EvaluatedRegister::isRegister() const {
		return true;
	}

	bool EvaluatedRegister::isEvalsToLiteral() const {
		return false;
	}

	uint8_t EvaluatedRegister::compile(std::vector<std::uint16_t> &output, ArgumentPosition position, bool indirect,
			bool forceNextWord) {
		// these errors should have been detected earlier, but better safe than sorry
		if (indirect && (_register == Register::PC || _register == Register::EX)) {
			throw invalid_argument(boost::str(boost::format("Register '%s' is not allowed in an indirection") 
				% ast::str(_register)));
		}

		if (!indirect && _hasOffset) {
			throw invalid_argument("Registers can not be involved in expressions outside of an indirection");
		}

		if (!indirect && _hasOffset && (forceNextWord || _value != 0)) {
			output.push_back((uint16_t)_value);
		}

		if (!indirect) {
			if (_register == Register::SP) {
				return 0x1b;
			} else if (_register == Register::PC) {
				return 0x1c;
			} else if (_register == Register::EX) {
				return 0x1d;
			} else {
				return static_cast<int>(_register);
			}
		} else if (_hasOffset) {
			if (_register == Register::SP) {
				return 0x1a;
			} else {
				return 0x10 + static_cast<int>(_register);
			}
		} else {
			if (_register == Register::SP) {
				return 0x19;
			} else {
				return 0x08 + static_cast<int>(_register);
			}
		}
	}

	EvaluatedExpressionPtr EvaluatedRegister::evaluate() const {
		return EvaluatedExpressionPtr(new EvaluatedRegister(_location, _register, _hasOffset, _value));
	}

	bool EvaluatedRegister::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return _hasOffset && (_value != 0 || forceNextWord);
	}

	string EvaluatedRegister::str() const {
		if (_hasOffset) {
			return (boost::format("%s + %d") % ast::str(_register) % _value).str();
		} else {
			return ast::str(_register);
		}
	}

	/*************************************************************************
	 *
	 * Pretty Printers
	 *
	 *************************************************************************/

	string str(UnaryOperator op) {
		switch (op) {
		case UnaryOperator::PLUS:
			return "+";
		case UnaryOperator::MINUS:
			return "-";
		case UnaryOperator::NOT:
			return "!";
		case UnaryOperator::BITWISE_NOT:
			return "~";
		}

		return "<Unknown UnaryOperator>";
	}

	string str(BinaryOperator op) {
		switch (op) {
		case BinaryOperator::PLUS:
			return "+";
		case BinaryOperator::MINUS:
			return "-";
		case BinaryOperator::MULTIPLY:
			return "*";
		case BinaryOperator::DIVIDE:
			return "/";
		case BinaryOperator::MODULO:
			return "%";
		case BinaryOperator::SHIFT_LEFT:
			return "<<";
		case BinaryOperator::SHIFT_RIGHT:
			return ">>";
		case BinaryOperator::AND:
			return "&";
		case BinaryOperator::OR:
			return "|";
		case BinaryOperator::XOR:
			return "^";
		}

		return "<Unknown BinaryOperator>";
	}

	string str(const Expression& expr) {
		return expr.str();
	}

	string str(const ExpressionPtr& expr) {
		return expr->str();
	}
}}