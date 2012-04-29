#include "Expression.hpp"

#include <stdexcept>

#include <boost/format.hpp>

using namespace std;
using namespace dcpu::lexer;

namespace dcpu { namespace ast {
	/*************************************************************************
	 *
	 * CompileFlags
	 *
	 *************************************************************************/

	CompileFlags::CompileFlags(ArgumentPosition position, bool indirection, bool forceNextWord) 
		: position(position), indirection(indirection), forceNextWord(forceNextWord) {}

	bool CompileFlags::isArgumentA() const {
		return position == ArgumentPosition::A;
	}

	bool CompileFlags::isArgumentB() const {
		return position == ArgumentPosition::B;
	}

	bool CompileFlags::isIndirection() const {
		return indirection;
	}

	bool CompileFlags::isForceNextWord() const {
		return forceNextWord;
	}

	/*************************************************************************
	 *
	 * Expression
	 *
	 *************************************************************************/
	Expression::Expression(const Location& location, bool literal, bool evaluatable, bool evaluated) 
		: literal(literal), evaluatable(evaluatable), evaluated(evaluated), location(location) {}

	bool Expression::isNextWordRequired(const CompileFlags& flags) const {
		return true;
	}

	CompileResult Expression::compile(const CompileFlags& flags) const {
		throw logic_error("Expression must be evaluated before it can be compiled");
	}

	bool Expression::isEvaluated() {
		return evaluated;
	}

	bool Expression::isLiteral() {
		return literal;
	}

	bool Expression::isEvaluatable() {
		return evaluatable;
	}

	int32_t Expression::getEvaluatedValue() {
		throw logic_error("Expression must be evaluated before it's value can be retrieved");
	}

	void Expression::setEvaluatedValue(int32_t newValue) {
		throw logic_error("Expression must be evaluated before it's value can be updated");
	}

	ExpressionPtr Expression::unaryOperation(const lexer::Location& location, UnaryOperator _operator,
		ExpressionPtr& operand) {
		return ExpressionPtr(new UnaryOperation(location, _operator, operand));
	}

	ExpressionPtr Expression::unaryOperation(const lexer::Location& location, UnaryOperator _operator,
		ExpressionPtr&& operand) {
		return ExpressionPtr(new UnaryOperation(location, _operator, operand));
	}

	ExpressionPtr Expression::binaryOperation(const lexer::Location& location, BinaryOperator _operator,
		ExpressionPtr& left, ExpressionPtr& right) {
		return ExpressionPtr(new BinaryOperation(location, _operator, left, right));
	}

	ExpressionPtr Expression::binaryOperation(const lexer::Location& location, BinaryOperator _operator,
		ExpressionPtr&& left, ExpressionPtr&& right) {
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
		return ExpressionPtr(new EvaluatedRegister(location, _register, false, 0));
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
		: Expression(location, false, false, false), _operator(op), operand(move(operand)) {

		literal = this->operand->isLiteral();
		evaluatable = this->operand->isEvaluatable();
	}

	ExpressionPtr UnaryOperation::evaluate() const {
		if (!evaluatable) {
			throw logic_error("Expression is not evaluatable");
		}

		auto evaluatedOperand = operand->evaluate();
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

		evaluatedOperand->setEvaluatedValue(value);
		return evaluatedOperand;
	}

	string UnaryOperation::str() const {
		return (boost::format("%s(%s)") % _operator % operand).str();
	}

	bool UnaryOperation::operator==(const Expression& other) const {
		const UnaryOperation *otherUnary = dynamic_cast<const UnaryOperation*>(&other);

		if (!otherUnary) return false;

		return _operator == otherUnary->_operator 
			&& operand == otherUnary->operand;
	}

	/*************************************************************************
	 *
	 * BinaryOperation
	 *
	 *************************************************************************/

	BinaryOperation::BinaryOperation(const Location& location, BinaryOperator op, ExpressionPtr& left,
		ExpressionPtr& right) 
		: Expression(location, false, false, false),
		_operator(op),
		left(move(left)),
		right(move(right)) {

		literal = this->left->isLiteral() && this->right->isLiteral();
		evaluatable = this->left->isEvaluatable() && this->right->isEvaluatable();
	}

	ExpressionPtr BinaryOperation::evaluate() const {
		if (!evaluatable) {
			throw logic_error("Expression is not evaluatable");
		}

		auto evaluatedLeft = left->evaluate();
		auto evaluatedRight = right->evaluate();

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
			evaluatedRight->setEvaluatedValue(resultValue);

			return evaluatedRight;
		} else {
			evaluatedLeft->setEvaluatedValue(resultValue);

			return evaluatedLeft;
		}
	}

	string BinaryOperation::str() const {
		return (boost::format("(%s %s %s)") % left % _operator % right).str();
	}

	bool BinaryOperation::operator==(const Expression& other) const {
		const BinaryOperation *otherBinary = dynamic_cast<const BinaryOperation*>(&other);

		if (!otherBinary) return false;

		return _operator == otherBinary->_operator 
			&& left == otherBinary->left 
			&& right == otherBinary->right;
	}

	/*************************************************************************
	 *
	 * RegisterOperand
	 *
	 *************************************************************************/

	RegisterOperand::RegisterOperand(const Location& location, Register reg) 
		: Expression(location, false, true, false), _register(reg) {}

	ExpressionPtr RegisterOperand::evaluate() const {
		return ExpressionPtr(new EvaluatedRegister(location, _register, false, 0));
	}

	string RegisterOperand::str() const {
		return (boost::format("%s") % _register).str();
	}

	bool RegisterOperand::operator==(const Expression& other) const {
		const RegisterOperand *otherRegister = dynamic_cast<const RegisterOperand*>(&other);

		if (!otherRegister) return false;

		return _register == otherRegister->_register;
	}

	/*************************************************************************
	 *
	 * LiteralOperand
	 *
	 *************************************************************************/

	LiteralOperand::LiteralOperand(const Location& location, uint32_t value)
		: Expression(location, true, true, false), value(value) {}

	ExpressionPtr LiteralOperand::evaluate() const {
		return ExpressionPtr(new EvaluatedLiteral(location, value));
	}

	string LiteralOperand::str() const {
		return (boost::format("%d") % value).str();
	}

	bool LiteralOperand::operator==(const Expression& other) const {
		const LiteralOperand *otherLiteral = dynamic_cast<const LiteralOperand*>(&other);

		if (!otherLiteral) return false;

		return value == otherLiteral->value;
	}

	/*************************************************************************
	 *
	 * LabelOperand
	 *
	 *************************************************************************/

	LabelOperand::LabelOperand(const Location& location, const string& label)
		: Expression(location, true, false, false), label(label), position(nullptr) {}

	ExpressionPtr LabelOperand::evaluate() const {
		if (position == nullptr) {
			throw new logic_error("Labels must be resolved before a LabelOperand can be evaluated");
		}

		return ExpressionPtr(new EvaluatedLiteral(location, *position));
	}

	string LabelOperand::str() const {
		return label;
	}

	bool LabelOperand::operator==(const Expression& other) const {
		const LabelOperand *otherLabel = dynamic_cast<const LabelOperand*>(&other);

		if (!otherLabel) return false;

		return label == otherLabel->label && position == otherLabel->position;
	}

	/*************************************************************************
	 *
	 * InvalidExpression
	 *
	 *************************************************************************/

	InvalidExpression::InvalidExpression(const Location& location) : Expression(location, true, false, false) {}

	ExpressionPtr InvalidExpression::evaluate() const {
		throw new logic_error("InvalidExpression can not be evaluated");
	}

	string InvalidExpression::str() const {
		return "<Invalid Expression>";
	}

	bool InvalidExpression::operator==(const Expression& other) const {
		const InvalidExpression *otherInvalid = dynamic_cast<const InvalidExpression*>(&other);

		return otherInvalid != nullptr;
	}

	/*************************************************************************
	 *
	 * EvaluatedLiteral
	 *
	 *************************************************************************/

	EvaluatedLiteral::EvaluatedLiteral(const lexer::Location& location, int32_t value) 
		: Expression(location, true, true, true), value(value) {}


	int32_t EvaluatedLiteral::getEvaluatedValue() {
		return value;
	}

	void EvaluatedLiteral::setEvaluatedValue(int32_t newValue) {
		value = newValue;
	}

	bool EvaluatedLiteral::isNextWordRequired(const CompileFlags& flags) const {
		if (flags.isForceNextWord() || flags.isIndirection() || flags.isArgumentB()) {
			return true;
		}

		return value < -1 || value > 30;
	}

	ExpressionPtr EvaluatedLiteral::evaluate() const {
		return ExpressionPtr(new EvaluatedLiteral(location, value));
	}

	CompileResult EvaluatedLiteral::compile(const CompileFlags& flags) const {
		if (flags.isIndirection()) {
			return CompileResult(0x1e, value);
		} else {
			if (flags.isArgumentB() || flags.isForceNextWord() || value < -1 || value > 30) {
				return CompileResult(0x20 + value, boost::none);
			} else {
				return CompileResult(0x1f, value);
			}
		}
	}

	string EvaluatedLiteral::str() const {
		return (boost::format("%d") % value).str();
	}

	bool EvaluatedLiteral::operator==(const Expression& other) const {
		const EvaluatedLiteral *otherLiteral = dynamic_cast<const EvaluatedLiteral*>(&other);

		if (!otherLiteral) return false;

		return value == otherLiteral->value;
	}

	/*************************************************************************
	 *
	 * EvaluatedRegister
	 *
	 *************************************************************************/
	EvaluatedRegister::EvaluatedRegister(const lexer::Location& location, Register _register, bool hasOffset,
		int32_t offset) 
		: Expression(location, false, true, true),
		_register(_register),
		hasOffset(hasOffset),
		offset(offset) {}

	int32_t EvaluatedRegister::getEvaluatedValue() {
		return offset;
	}

	void EvaluatedRegister::setEvaluatedValue(int32_t newValue) {
		hasOffset = true;
		offset = newValue;
	}

	bool EvaluatedRegister::isNextWordRequired(const CompileFlags& flags) const {
		if (!hasOffset) {
			return false;
		}

		return offset > 0 || flags.isForceNextWord();
	}

	ExpressionPtr EvaluatedRegister::evaluate() const {
		return ExpressionPtr(new EvaluatedRegister(location, _register, hasOffset, offset));
	}

	CompileResult EvaluatedRegister::compileRegister(const CompileFlags& flags, uint8_t noIndirection,
		uint8_t indirectNoOffset, uint8_t indirectWithOffset) const {

		if (!flags.isIndirection()) {
			return CompileResult(noIndirection, boost::none);
		}

		if (hasOffset && (flags.isForceNextWord() || offset != 0)) {
			return CompileResult(indirectWithOffset, offset);
		} else {
			return CompileResult(indirectNoOffset, offset);
		}
	}

	CompileResult EvaluatedRegister::compile(const CompileFlags& flags) const {
		switch (_register) {
		case Register::A:
			return compileRegister(flags, 0x0, 0x8, 0x10);
		case Register::B:
			return compileRegister(flags, 0x1, 0x9, 0x11);
		case Register::C:
			return compileRegister(flags, 0x2, 0xa, 0x12);
		case Register::X:
			return compileRegister(flags, 0x3, 0xb, 0x13);
		case Register::Y:
			return compileRegister(flags, 0x4, 0xc, 0x14);
		case Register::Z:
			return compileRegister(flags, 0x5, 0xd, 0x15);
		case Register::I:
			return compileRegister(flags, 0x6, 0xe, 0x16);
		case Register::J:
			return compileRegister(flags, 0x7, 0xf, 0x17);
		case Register::SP:
			return compileRegister(flags, 0x1b, 0x19, 0x1a);
		case Register::PC:
			return CompileResult(0x1c, boost::none);
		case Register::EX:
			return CompileResult(0x1d, boost::none);
		default:
			throw logic_error((boost::format("%s") % _register ).str());
		}
	}

	string EvaluatedRegister::str() const {
		if (hasOffset) {
			return (boost::format("%s + %d") % _register % offset).str();
		} else {
			return (boost::format("%s") % _register).str();
		}
	}

	bool EvaluatedRegister::operator==(const Expression& other) const {
		const EvaluatedRegister *otherRegister = dynamic_cast<const EvaluatedRegister*>(&other);

		if (!otherRegister) return false;

		return _register == otherRegister->_register 
			&& hasOffset == otherRegister->hasOffset 
			&& offset == otherRegister->offset;
	}

	/*************************************************************************
	 *
	 * Operators
	 *
	 *************************************************************************/

	bool operator==(const ExpressionPtr &left, const ExpressionPtr &right) {
		if (left && right) {
			return *left == *right;
		} else {
			return !left == !right;
		}
	}

	std::ostream& operator<< (std::ostream& stream, const ExpressionPtr& expr) {
		return stream << expr->str();
	}
}}