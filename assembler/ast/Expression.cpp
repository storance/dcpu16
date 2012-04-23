#include "Expression.hpp"

using namespace std;

namespace dcpu { namespace ast {
	Expression::Expression(const Location& location) : _location(location) {}

	Expression::Expression(Expression&& other) : _location(move(other._location)) {}

	Expression::~Expression() {}

	UnaryOperation::UnaryOperation(const Location& location, UnaryOperator op, ExpressionPtr& operand)
		: Expression(location), _operator(op), _operand(move(operand)) {

		_cachedEvalsToLiteral = _operand->isEvalsToLiteral();
	}

	UnaryOperation::UnaryOperation(UnaryOperation&& other)
		:  Expression(other._location), _operator(other._operator), _operand(move(other._operand)) {}

	bool UnaryOperation::isEvalsToLiteral() {
		return _cachedEvalsToLiteral;
	}
	
	bool UnaryOperation::isSimple() {
		return false;
	}

	BinaryOperation::BinaryOperation(const Location& location, BinaryOperator op, ExpressionPtr& left,
		ExpressionPtr& right) : Expression(location), _operator(op), _left(move(left)), _right(move(right)) {
		_cachedEvalsToLiteral = _left->isEvalsToLiteral() && _right->isEvalsToLiteral();
	}

	BinaryOperation::BinaryOperation(BinaryOperation&& other)
		:  Expression(other._location), _operator(other._operator), _left(move(other._left)),
		 _right(move(other._right)) {}

	bool BinaryOperation::isEvalsToLiteral() {
		return _cachedEvalsToLiteral;
	}
	
	bool BinaryOperation::isSimple() {
		return false;
	}

	RegisterOperand::RegisterOperand(const Location& location, common::Register reg) 
		: Expression(location), _register(reg) {}

	bool RegisterOperand::isEvalsToLiteral() {
		return false;
	}
	
	bool RegisterOperand::isSimple() {
		return true;
	}

	LiteralOperand::LiteralOperand(const Location& location, uint32_t value)
		: Expression(location), _value(value) {}

	bool LiteralOperand::isEvalsToLiteral() {
		return true;
	}
	
	bool LiteralOperand::isSimple() {
		return true;
	}

	LabelReferenceOperand::LabelReferenceOperand(const Location& location, const std::string& label)
		: Expression(location), _label(label) {}

	LabelReferenceOperand::LabelReferenceOperand(std::shared_ptr<Token> token)
		:Expression(token->location), _label(token->content) {}

	bool LabelReferenceOperand::isEvalsToLiteral() {
		return true;
	}
	
	bool LabelReferenceOperand::isSimple() {
		return true;
	}

	InvalidExpression::InvalidExpression(const Location& location) : Expression(location) {}

	bool InvalidExpression::isEvalsToLiteral() {
		return true;
	}
	
	bool InvalidExpression::isSimple() {
		return true;
	}

	std::string str(UnaryOperator op) {
		switch (op) {
		case UnaryOperator::PLUS:
			return "+";
		case UnaryOperator::MINUS:
			return "-";
		case UnaryOperator::NOT:
			return "~";
		}

		return "<Unknown>";
	}

	std::string str(BinaryOperator op) {
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

		return "<Unknown>";
	}
}}