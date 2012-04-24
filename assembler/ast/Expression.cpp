#include "Expression.hpp"

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

	UnaryOperation::UnaryOperation(const Location& location, UnaryOperator op, ExpressionPtr& operand)
		: Expression(location), _operator(op), _operand(move(operand)) {

		_cachedEvalsToLiteral = _operand->isEvalsToLiteral();
	}

	/*************************************************************************
	 *
	 * UnaryOperation
	 *
	 *************************************************************************/

	UnaryOperation::UnaryOperation(UnaryOperation&& other)
		:  Expression(other._location), _operator(other._operator), _operand(move(other._operand)) {}

	bool UnaryOperation::isEvalsToLiteral() const {
		return _cachedEvalsToLiteral;
	}
	
	bool UnaryOperation::isSimple() const {
		return false;
	}

	std::string UnaryOperation::str() const {
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
	}

	BinaryOperation::BinaryOperation(BinaryOperation&& other)
		:  Expression(other._location), _operator(other._operator), _left(move(other._left)),
		 _right(move(other._right)) {}

	bool BinaryOperation::isEvalsToLiteral() const {
		return _cachedEvalsToLiteral;
	}
	
	bool BinaryOperation::isSimple() const {
		return false;
	}

	std::string BinaryOperation::str() const {
		return (boost::format("(%s %s %s)") % _left->str() % ast::str(_operator) % _right->str()).str();
	}

	/*************************************************************************
	 *
	 * RegisterOperand
	 *
	 *************************************************************************/

	RegisterOperand::RegisterOperand(const Location& location, common::Register reg) 
		: Expression(location), _register(reg) {}

	bool RegisterOperand::isEvalsToLiteral() const {
		return false;
	}
	
	bool RegisterOperand::isSimple() const {
		return true;
	}

	std::string RegisterOperand::str() const {
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
	
	bool LiteralOperand::isSimple() const {
		return true;
	}

	std::string LiteralOperand::str() const {
		return (boost::format("%d") % _value).str();
	}

	/*************************************************************************
	 *
	 * LabelReferenceOperand
	 *
	 *************************************************************************/

	LabelReferenceOperand::LabelReferenceOperand(const Location& location, const std::string& label)
		: Expression(location), _label(label) {}

	LabelReferenceOperand::LabelReferenceOperand(TokenPtr& token)
		:Expression(token->location), _label(token->content) {}

	bool LabelReferenceOperand::isEvalsToLiteral() const {
		return true;
	}
	
	bool LabelReferenceOperand::isSimple() const {
		return true;
	}

	std::string LabelReferenceOperand::str() const {
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
	
	bool InvalidExpression::isSimple() const {
		return true;
	}

	std::string InvalidExpression::str() const {
		return "<Invalid Expression>";
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

	std::string str(common::Register _register) {
		switch (_register) {
		case common::Register::A:
			return "A";
		case common::Register::B:
			return "B";
		case common::Register::C:
			return "C";
		case common::Register::X:
			return "X";
		case common::Register::Y:
			return "Y";
		case common::Register::Z:
			return "Z";
		case common::Register::I:
			return "I";
		case common::Register::J:
			return "J";
		case common::Register::PC:
			return "PC";
		case common::Register::SP:
			return "SP";
		case common::Register::O:
			return "O";
		}
	}

	std::string str(Expression& expr) {
		return expr.str();
	}

	std::string str(ExpressionPtr& expr) {
		return expr->str();
	}
}}