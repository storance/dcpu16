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

	bool Expression::isEvaluated() {
		return false;
	}

	int32_t Expression::getEvaluatedValue() {
		throw logic_error("Expression must be evaluated before it's value can be retrieved");
	}

	void Expression::updateEvaluatedValue(int32_t newValue) {
		throw logic_error("Expression must be evaluated before it's value can be updated");
	}

	ExpressionPtr Expression::unaryOperation(const lexer::Location& location, UnaryOperator _operator,
		ExpressionPtr& operand) {
		return ExpressionPtr(new UnaryOperation(location, _operator, operand));
	}

	ExpressionPtr Expression::binaryOperation(const lexer::Location& location, BinaryOperator _operator,
		ExpressionPtr& left, ExpressionPtr& right) {
		return ExpressionPtr(new BinaryOperation(location, _operator, left, right));
	}

	ExpressionPtr Expression::literalOperand(const lexer::Location& location, uint32_t value) {
		return ExpressionPtr(new LiteralOperand(location, value));
	}

	ExpressionPtr Expression::labelOperand(const lexer::Location& location, const std::string& name) {
		return ExpressionPtr(new LabelOperand(location, name));
	}

	ExpressionPtr Expression::registerOperand(const lexer::Location& location, Register _register) {
		return ExpressionPtr(new RegisterOperand(location, _register));
	}

	ExpressionPtr Expression::invalid(const lexer::Location& location) {
		return ExpressionPtr(new InvalidExpression(location));
	}

	ExpressionPtr Expression::evaluatedRegister(const lexer::Location& location, Register _register) {
		return ExpressionPtr(new EvaluatedRegister(location, _register));
	}

	ExpressionPtr Expression::evaluatedRegister(const lexer::Location& location, Register _register, int32_t offset) {
		return ExpressionPtr(new EvaluatedRegister(location, _register, true, offset));
	}

	ExpressionPtr Expression::evaluatedLiteral(const lexer::Location& location, int32_t value) {
		return ExpressionPtr(new EvaluatedLiteral(location, value));
	}

	/*************************************************************************
	 *
	 * UnaryOperation
	 *
	 *************************************************************************/

	UnaryOperation::UnaryOperation(const Location& location, UnaryOperator op, ExpressionPtr& operand)
		: Expression(location), _operator(op), _operand(move(operand)) {

		updateCache();
	}

	void UnaryOperation::updateCache() {
		_cachedIsLiteral = _operand->isLiteral();
		_cachedIsEvaluatable = _operand->isEvaluatable();
	}

	bool UnaryOperation::isLiteral() const {
		return _cachedIsLiteral;
	}

	bool UnaryOperation::isEvaluatable() const {
		return _cachedIsEvaluatable;
	}
	
	bool UnaryOperation::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return _operand->isNextWordRequired(position, forceNextWord);
	}

	ExpressionPtr UnaryOperation::evaluate() const {
		if (!isEvaluatable()) {
			throw logic_error("Expression is not evaluatable");
		}

		auto evaluatedOperand = _operand->evaluate();
		if (!evaluatedOperand->isLiteral()) {
			throw logic_error(boost::str(boost::format("The operand for unary '%s' must be a literal") 
				% ast::str(_operator)));
		}
		
		int32_t value = evaluatedOperand->getEvaluatedValue();

		switch (_operator) {
		case UnaryOperator::PLUS:
			value = +value;
			break;
		case UnaryOperator::MINUS:
			value = -value;
			break;
		case UnaryOperator::NOT:
			value = !value;
			break;
		case UnaryOperator::BITWISE_NOT:
			value = ~value;
			break;
		}

		evaluatedOperand->updateEvaluatedValue(value);
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
		
		updateCache();
	}

	void BinaryOperation::updateCache() {
		_cachedIsLiteral = _left->isLiteral() && _right->isLiteral();
		_cachedIsEvaluatable = _left->isEvaluatable() && _right->isEvaluatable();
	}

	bool BinaryOperation::isLiteral() const {
		return _cachedIsLiteral;
	}

	bool BinaryOperation::isEvaluatable() const {
		return _cachedIsEvaluatable;
	}
	
	bool BinaryOperation::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		// until we can evaluate the expression, assume we have to use the next word
		return true;
	}

	ExpressionPtr BinaryOperation::evaluate() const {
		if (!isEvaluatable()) {
			throw logic_error("Expression is not evaluatable");
		}

		auto evaluatedLeft = _left->evaluate();
		auto evaluatedRight = _right->evaluate();

		if (_operator != BinaryOperator::PLUS && _operator != BinaryOperator::MINUS && !evaluatedLeft->isLiteral()) {
			throw logic_error(boost::str(boost::format("The left operand of '%s' must be a literal") 
				% ast::str(_operator)));
		}

		if (_operator != BinaryOperator::PLUS && !evaluatedRight->isLiteral()) {
			throw logic_error(boost::str(boost::format("The right operand of '%s' must be a literal") 
				% ast::str(_operator)));
		}

		int32_t leftValue = evaluatedLeft->getEvaluatedValue();
		int32_t rightValue = evaluatedRight->getEvaluatedValue();
		int32_t resultValue = 0;

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

		if (!evaluatedRight->isLiteral()) {
			evaluatedRight->updateEvaluatedValue(resultValue);

			return evaluatedRight;
		} else {
			evaluatedLeft->updateEvaluatedValue(resultValue);

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

	bool RegisterOperand::isLiteral() const {
		return false;
	}

	bool RegisterOperand::isEvaluatable() const {
		return true;
	}
	
	bool RegisterOperand::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return false;
	}

	ExpressionPtr RegisterOperand::evaluate() const {
		return ExpressionPtr(new EvaluatedRegister(_location, _register, false, 0));
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

	bool LiteralOperand::isLiteral() const {
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

	ExpressionPtr LiteralOperand::evaluate() const {
		return ExpressionPtr(new EvaluatedLiteral(_location, _value));
	}

	string LiteralOperand::str() const {
		return (boost::format("%d") % _value).str();
	}

	/*************************************************************************
	 *
	 * LabelOperand
	 *
	 *************************************************************************/

	LabelOperand::LabelOperand(const Location& location, const string& label)
		: Expression(location), _label(label), _position(nullptr) {}

	bool LabelOperand::isLiteral() const {
		return true;
	}

	bool LabelOperand::isEvaluatable() const {
		return _position != nullptr;
	}
	
	bool LabelOperand::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return true;
	}

	ExpressionPtr LabelOperand::evaluate() const {
		if (_position != nullptr) {
			return ExpressionPtr(new EvaluatedLiteral(_location, *_position));
		} else {
			return ExpressionPtr();
		}
	}

	string LabelOperand::str() const {
		return _label;
	}

	/*************************************************************************
	 *
	 * InvalidExpression
	 *
	 *************************************************************************/

	InvalidExpression::InvalidExpression(const Location& location) : Expression(location) {}

	bool InvalidExpression::isLiteral() const {
		return true;
	}

	bool InvalidExpression::isEvaluatable() const {
		return false;
	}
	
	bool InvalidExpression::isNextWordRequired(ArgumentPosition position, bool forceNextWord) const {
		return false;
	}

	ExpressionPtr InvalidExpression::evaluate() const {
		return ExpressionPtr();
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
		return false;
	}

	bool EvaluatedExpression::isEvaluated() {
		return true;
	}

	int32_t EvaluatedExpression::getEvaluatedValue() {
		return _value;
	}

	void EvaluatedExpression::updateEvaluatedValue(int32_t newValue) {
		_value = newValue;
	}

	

	/*************************************************************************
	 *
	 * EvaluatedLiteral
	 *
	 *************************************************************************/

	EvaluatedLiteral::EvaluatedLiteral(const lexer::Location& location, int32_t value) 
		: EvaluatedExpression(location, value) {}

	bool EvaluatedLiteral::isLiteral() const {
		return true;
	}

	ExpressionPtr EvaluatedLiteral::evaluate() const {
		return ExpressionPtr(new EvaluatedLiteral(_location, _value));
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

	EvaluatedRegister::EvaluatedRegister(const lexer::Location& location, ast::Register _register)
		: EvaluatedExpression(location, 0), _register(_register), _hasOffset(false) {}

	EvaluatedRegister::EvaluatedRegister(const lexer::Location& location, Register _register, bool hasOffset,
		int32_t offset) : EvaluatedExpression(location, offset), _register(_register), _hasOffset(hasOffset) {}

	void EvaluatedRegister::updateEvaluatedValue(int32_t newValue) {
		_hasOffset = true;
		_value = newValue;
	}

	bool EvaluatedRegister::isLiteral() const {
		return false;
	}

	ExpressionPtr EvaluatedRegister::evaluate() const {
		return ExpressionPtr(new EvaluatedRegister(_location, _register, _hasOffset, _value));
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

	string str(const Expression& expr) {
		return expr.str();
	}

	string str(const ExpressionPtr& expr) {
		return expr->str();
	}
}}