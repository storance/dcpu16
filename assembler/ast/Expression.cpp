#include "Expression.hpp"

using namespace std;

namespace dcpu { namespace ast {
	Expression::Expression(const Location& location) : _location(location) {}

	Expression::~Expression() {}

	UnaryOperation::UnaryOperation(const Location& location, UnaryOperator op, shared_ptr<Expression> operand)
		: Expression(location), _operator(op), _operand(operand) {}

	BinaryOperation::BinaryOperation(const Location& location, BinaryOperator op, shared_ptr<Expression> left,
		shared_ptr<Expression> right) : Expression(location), _operator(op), _left(left), _right(right) {}

	RegisterOperand::RegisterOperand(const Location& location, common::Register reg) 
		: Expression(location), _register(reg) {}

	LiteralOperand::LiteralOperand(const Location& location, uint32_t value)
		: Expression(location), _value(value) {}

	LabelReferenceOperand::LabelReferenceOperand(const Location& location, const std::string& label)
		: Expression(location), _label(label) {}
}}