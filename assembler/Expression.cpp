#include "Expression.hpp"

#include <stdexcept>
#include <boost/format.hpp>

using namespace std;
using namespace dcpu::lexer;

namespace dcpu { namespace ast {
	Locatable::Locatable(location_t &location) : location(location) {}


	LiteralOperand::LiteralOperand(location_t &location, uint32_t value)
		: Locatable(location), value(value) {}

	bool LiteralOperand::operator==(const LiteralOperand& other) const {
		return value == other.value;
	}

	LabelOperand::LabelOperand(location_t &location, const string &label) 
		: Locatable(location), label(label), pc() {}

	LabelOperand::LabelOperand(location_t &location, const string &label, uint16_t pc)
		: Locatable(location), label(label), pc(pc) {}

	bool LabelOperand::operator==(const LabelOperand& other) const {
		return label == other.label && pc == other.pc;
	}

	RegisterOperand::RegisterOperand(location_t &location, Register _register)
		: Locatable(location), _register(_register) {}

	bool RegisterOperand::operator==(const RegisterOperand& other) const {
		return _register == other._register;
	}

	EvaluatedExpression::EvaluatedExpression(location_t &location, Register _register)
		: Locatable(location), _register(_register), value() {}

	EvaluatedExpression::EvaluatedExpression(location_t &location, Register _register, int32_t value)
		: Locatable(location), _register(_register), value(value) {}

	EvaluatedExpression::EvaluatedExpression(location_t &location, int32_t value)
		: Locatable(location), _register(), value(value) {}

	bool EvaluatedExpression::operator==(const EvaluatedExpression& other) const {
		return _register == other._register  && value == other.value;
	}

	UnaryOperation::UnaryOperation(location_t &location, UnaryOperator _operator, const Expression &operand)
		: Locatable(location), _operator(_operator), operand(operand) {}

	bool UnaryOperation::operator==(const UnaryOperation& other) const {
		return _operator == other._operator && operand == other.operand;
	}

	BinaryOperation::BinaryOperation(location_t &location, BinaryOperator _operator, const Expression &left,
		const Expression &right) : Locatable(location), _operator(_operator), left(left), right(right) {}

	bool BinaryOperation::operator==(const BinaryOperation& other) const {
		return _operator == _operator && left == left && right == right;
	}

	class EvaluatedVisitor : public boost::static_visitor<bool> {
	public:
		bool operator()(const EvaluatedExpression &evaluatedExpr) const {
			return true;
		}

		template <typename T>
	    bool operator()( const T &) const {
	        return false;
	    }
	};

	class EvaluatableVisitor : public boost::static_visitor<bool> {
	public:
		bool operator()(const EvaluatedExpression &) const {
			return true;
		}

		bool operator()(const RegisterOperand &) const {
			return true;
		}

		bool operator()(const LiteralOperand &) const {
			return true;
		}

		bool operator()(const LabelOperand &expr) const {
			return expr.pc;
		}

		bool operator()(const BinaryOperation &expr) const {
			return boost::apply_visitor(*this, expr.left) && boost::apply_visitor(*this, expr.right);
		}

		bool operator()(const UnaryOperation &expr) const {
			return boost::apply_visitor(*this, expr.operand);
		}
	};

	class EvaluateVisitor : public boost::static_visitor<EvaluatedExpression> {
	public:
		EvaluatedExpression operator()(EvaluatedExpression &expr) {
			return expr;
		}

		EvaluatedExpression operator()(RegisterOperand &expr) {
			return EvaluatedExpression(expr.location, expr._register);
		}

		EvaluatedExpression operator()(LiteralOperand &expr) {
			return EvaluatedExpression(expr.location, expr.value);
		}

		EvaluatedExpression operator()(LabelOperand &expr) {
			if (!expr.pc) {
				throw invalid_argument("Labels must be resolved before this expression can be evaluated");
			}

			return EvaluatedExpression(expr.location, *expr.pc);
		}

		EvaluatedExpression operator()(BinaryOperation &expr) {
			auto left = boost::apply_visitor(*this, expr.left);
			auto right = boost::apply_visitor(*this, expr.right);

			if (left._register && right._register) {
				throw invalid_argument("A register may only appear once in an expression");
			}

			int32_t value;

			if (expr._operator != BinaryOperator::PLUS && expr._operator != BinaryOperator::MINUS && left._register) {
				throw invalid_argument(str(boost::format("The left operand of '%s' must be a literal") 
					% expr._operator));
			}

			if (expr._operator != BinaryOperator::PLUS && right._register) {
				throw invalid_argument(str(boost::format("The right operand of '%s' must be a literal") 
					% expr._operator));
			}

			int32_t leftValue = 0, rightValue = 0;
			if (left.value) {
				leftValue = *left.value;
			}

			if (right.value) {
				rightValue = *right.value;
			}

			switch (expr._operator) {
			case BinaryOperator::PLUS:
				value = leftValue + rightValue;
				break;
			case BinaryOperator::MINUS:
				value = leftValue - rightValue;
				break;
			case BinaryOperator::MULTIPLY:
				value = leftValue * rightValue;
				break;
			case BinaryOperator::DIVIDE:
				value = leftValue / rightValue;
				break;
			case BinaryOperator::MODULO:
				value = leftValue % rightValue;
				break;
			case BinaryOperator::SHIFT_LEFT:
				value = leftValue << rightValue;
				break;
			case BinaryOperator::SHIFT_RIGHT:
				value = leftValue >> rightValue;
				break;
			case BinaryOperator::AND:
				value = leftValue & rightValue;
				break;
			case BinaryOperator::OR:
				value = leftValue | rightValue;
				break;
			case BinaryOperator::XOR:
				value = leftValue ^ rightValue;
				break;
			}

			if (left._register) {
				return EvaluatedExpression(left.location, *left._register, value);
			} else if (right._register) {
				return EvaluatedExpression(left.location, *right._register, value);
			} else {
				return EvaluatedExpression(left.location, value);
			}
		}

		EvaluatedExpression operator()(UnaryOperation &expr) {
			auto operand = boost::apply_visitor(*this, expr.operand);

			if (operand._register) {
				throw invalid_argument(str(boost::format("The operand of unary '%s' must be a literal") 
					% expr._operator));
			}

			int32_t value = operand.value ? *operand.value : 0;

			switch (expr._operator) {
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

			return EvaluatedExpression(operand.location, value);
		}
	};

	bool evaluated(const Expression &expr) {
		return boost::apply_visitor(EvaluatedVisitor(), expr);
	}

	bool evaluatable(const Expression &expr) {
		return boost::apply_visitor(EvaluatableVisitor(), expr);
	}

	Expression evaluate(Expression &expr) {
		auto visitor = EvaluateVisitor();
		return boost::apply_visitor(visitor, expr);
	}

	ostream& operator<< (ostream& stream, UnaryOperator op) {
		switch (op) {
		case UnaryOperator::PLUS:
			return stream << "+";
		case UnaryOperator::MINUS:
			return stream << "-";
		case UnaryOperator::NOT:
			return stream << "!";
		case UnaryOperator::BITWISE_NOT:
			return stream << "~";
		default:
			return stream << "<Unknown UnaryOperator " << static_cast<int>(op) << ">";
		}
	}

	ostream& operator<< (ostream& stream, BinaryOperator op) {
		switch (op) {
		case BinaryOperator::PLUS:
			return stream << "+";
		case BinaryOperator::MINUS:
			return stream << "-";
		case BinaryOperator::MULTIPLY:
			return stream << "*";
		case BinaryOperator::DIVIDE:
			return stream << "/";
		case BinaryOperator::MODULO:
			return stream << "%";
		case BinaryOperator::SHIFT_LEFT:
			return stream << "<<";
		case BinaryOperator::SHIFT_RIGHT:
			return stream << ">>";
		case BinaryOperator::AND:
			return stream << "&";
		case BinaryOperator::OR:
			return stream << "|";
		case BinaryOperator::XOR:
			return stream << "^";
		default:
			return stream << "<Unknown BinaryOperator " << static_cast<int>(op) << ">";
		}
	}

	ostream& operator<< (ostream& stream, const UnaryOperation &expr) {
		return stream << expr._operator << "(" << expr.operand << ")";
	}

	ostream& operator<< (ostream& stream, const BinaryOperation &expr) {
		return stream << "(" << expr.left << " " << expr._operator << " " << expr.right << ")";
	}

	ostream& operator<< (ostream& stream, const EvaluatedExpression &expr) {
		if (expr._register) {
			stream << *expr._register;

			if (expr.value) {
				stream << " + " << *expr.value;
			}
		} else if (expr.value) {
			stream << *expr.value;
		}

		return stream;
	}

	ostream& operator<< (ostream& stream, const RegisterOperand &expr) {
		return stream << expr._register;
	}

	ostream& operator<< (ostream& stream, const LabelOperand &expr) {
		return stream << expr.label;
	}

	ostream& operator<< (ostream& stream, const LiteralOperand &expr) {
		return stream << expr.value;
	}
}}