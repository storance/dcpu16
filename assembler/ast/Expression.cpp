#include "Expression.hpp"

using namespace std;

namespace dcpu { namespace ast {
	Expression::Expression(const Location& location) : _location(location) {}

	Expression::~Expression() {}

	UnaryOperation::UnaryOperation(const Location& location, UnaryOperator op, shared_ptr<Expression> operand)
		: Expression(location), _operator(op), _operand(operand) {

		_cachedEvalsToLiteral = _operand->isEvalsToLiteral();
	}

	bool UnaryOperation::isEvalsToLiteral() {
		return _cachedEvalsToLiteral;
	}
	
	ExpressionType UnaryOperation::getType() {
		return ExpressionType::UNARY_OPERATION;
	}

	BinaryOperation::BinaryOperation(const Location& location, BinaryOperator op, shared_ptr<Expression> left,
		shared_ptr<Expression> right) : Expression(location), _operator(op), _left(left), _right(right) {
		_cachedEvalsToLiteral = _left->isEvalsToLiteral() && _right->isEvalsToLiteral();
	}

	bool BinaryOperation::isEvalsToLiteral() {
		return _cachedEvalsToLiteral;
	}
	
	ExpressionType BinaryOperation::getType() {
		return ExpressionType::BINARY_OPERATION;
	}

	RegisterOperand::RegisterOperand(const Location& location, common::Register reg) 
		: Expression(location), _register(reg) {}

	bool RegisterOperand::isEvalsToLiteral() {
		return false;
	}
	
	ExpressionType RegisterOperand::getType() {
		return ExpressionType::REGISTER;
	}

	LiteralOperand::LiteralOperand(const Location& location, uint32_t value)
		: Expression(location), _value(value) {}

	bool LiteralOperand::isEvalsToLiteral() {
		return true;
	}
	
	ExpressionType LiteralOperand::getType() {
		return ExpressionType::LITERAL;
	}

	LabelReferenceOperand::LabelReferenceOperand(const Location& location, const std::string& label)
		: Expression(location), _label(label) {}

	LabelReferenceOperand::LabelReferenceOperand(std::shared_ptr<Token> token)
		:Expression(token->location), _label(token->content) {}

	bool LabelReferenceOperand::isEvalsToLiteral() {
		return true;
	}
	
	ExpressionType LabelReferenceOperand::getType() {
		return ExpressionType::LABEL;
	}

	InvalidExpression::InvalidExpression(const Location& location) : Expression(location) {}

	bool InvalidExpression::isEvalsToLiteral() {
		return true;
	}
	
	ExpressionType InvalidExpression::getType() {
		return ExpressionType::INVALID;
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