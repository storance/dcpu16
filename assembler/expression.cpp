#include "expression.hpp"

#include <stdexcept>
#include <boost/format.hpp>

using namespace std;
using namespace dcpu::lexer;
using namespace boost;

namespace dcpu { namespace ast {
	/*************************************************************************
	 *
	 * locatable
	 *
	 *************************************************************************/
	locatable::locatable(const location_ptr &location) : location(location) {}

	/*************************************************************************
	 *
	 * literal_operand
	 *
	 *************************************************************************/
	literal_operand::literal_operand(const location_ptr &location, uint32_t value)
		: locatable(location), value(value) {}

	bool literal_operand::operator==(const literal_operand& other) const {
		return value == other.value;
	}

	/*************************************************************************
	 *
	 * symbol_operand
	 *
	 *************************************************************************/
	symbol_operand::symbol_operand(const location_ptr &location, const string &label)
		: locatable(location), label(label), pc(nullptr) {}

	symbol_operand::symbol_operand(const location_ptr &location, const string &label, uint16_t &pc)
		: locatable(location), label(label), pc(&pc) {}

	bool symbol_operand::operator==(const symbol_operand& other) const {
		if (label != other.label) {
			return false;
		}

		if (pc && other.pc) {
			return *pc == *other.pc;
		} else {
			return pc == other.pc;
		}
	}

	/*************************************************************************
	 *
	 * register_operand
	 *
	 *************************************************************************/
	register_operand::register_operand(const location_ptr &location, registers _register)
		: locatable(location), _register(_register) {}

	bool register_operand::operator==(const register_operand& other) const {
		return _register == other._register;
	}

	/*************************************************************************
	 *
	 * invalid_expression
	 *
	 *************************************************************************/
	invalid_expression::invalid_expression(const location_ptr &location) : locatable(location) {}

	bool invalid_expression::operator==(const invalid_expression& other) const {
		return true;
	}

	/*************************************************************************
	 *
	 * evaluated_expression
	 *
	 *************************************************************************/
	evaluated_expression::evaluated_expression(const location_ptr &location, registers _register)
		: locatable(location), _register(_register), value() {}

	evaluated_expression::evaluated_expression(const location_ptr &location, registers _register, int32_t value)
		: locatable(location), _register(_register), value(value) {}

	evaluated_expression::evaluated_expression(const location_ptr &location, int32_t value)
		: locatable(location), _register(), value(value) {}

	bool evaluated_expression::operator==(const evaluated_expression& other) const {
		return _register == other._register  && value == other.value;
	}

	/*************************************************************************
	 *
	 * unary_operation
	 *
	 *************************************************************************/
	unary_operation::unary_operation(const location_ptr &location, unary_operator _operator, const expression &operand)
		: locatable(location), _operator(_operator), operand(operand) {}

	bool unary_operation::operator==(const unary_operation& other) const {
		return _operator == other._operator && operand == other.operand;
	}

	/*************************************************************************
	 *
	 * binary_operation
	 *
	 *************************************************************************/

	binary_operation::binary_operation(const location_ptr &location, binary_operator _operator, const expression &left,
		const expression &right) : locatable(location), _operator(_operator), left(left), right(right) {}

	bool binary_operation::operator==(const binary_operation& other) const {
		return _operator == _operator && left == left && right == right;
	}

	/*************************************************************************
	 *
	 * expression_evaluated
	 *
	 *************************************************************************/
	class expression_evaluated : public static_visitor<bool> {
	public:
		bool operator()(const evaluated_expression &evaluatedExpr) const {
			return true;
		}

		template <typename T>
	    bool operator()( const T &) const {
	        return false;
	    }
	};

	/*************************************************************************
	 *
	 * expression_evaluatable
	 *
	 *************************************************************************/
	class expression_evaluatable : public static_visitor<bool> {
	public:
		bool operator()(const evaluated_expression &) const {
			return true;
		}

		bool operator()(const register_operand &) const {
			return true;
		}

		bool operator()(const literal_operand &) const {
			return true;
		}

		bool operator()(const symbol_operand &expr) const {
			return expr.pc != nullptr;
		}

		bool operator()(const binary_operation &expr) const {
			return apply_visitor(*this, expr.left) && apply_visitor(*this, expr.right);
		}

		bool operator()(const unary_operation &expr) const {
			return apply_visitor(*this, expr.operand);
		}

		bool operator()(const invalid_expression&) const {
			return false;
		}
	};

	/*************************************************************************
	 *
	 * expression_evaluator
	 *
	 *************************************************************************/
	class expression_evaluator : public static_visitor<evaluated_expression> {
	public:
		evaluated_expression operator()(const evaluated_expression &expr) const {
			return expr;
		}

		evaluated_expression operator()(const register_operand &expr) const {
			return evaluated_expression(expr.location, expr._register);
		}

		evaluated_expression operator()(const literal_operand &expr) const {
			return evaluated_expression(expr.location, expr.value);
		}

		evaluated_expression operator()(const symbol_operand &expr) const {
			if (!expr.pc) {
				throw invalid_argument("unresolved labels");
			}

			return evaluated_expression(expr.location, *expr.pc);
		}

		evaluated_expression operator()(const binary_operation &expr) const {
			auto left = apply_visitor(*this, expr.left);
			auto right = apply_visitor(*this, expr.right);

			if (left._register && right._register) {
				throw invalid_argument("multiple register expression");
			}

			int32_t value;

			if (expr._operator != binary_operator::PLUS && expr._operator != binary_operator::MINUS && left._register) {
				throw invalid_argument(str(format("the left operand of '%s' is not a literal")
					% expr._operator));
			}

			if (expr._operator != binary_operator::PLUS && right._register) {
				throw invalid_argument(str(format("the right operand of '%s' is not a literal")
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
			case binary_operator::PLUS:
				value = leftValue + rightValue;
				break;
			case binary_operator::MINUS:
				value = leftValue - rightValue;
				break;
			case binary_operator::MULTIPLY:
				value = leftValue * rightValue;
				break;
			case binary_operator::DIVIDE:
				value = leftValue / rightValue;
				break;
			case binary_operator::MODULO:
				value = leftValue % rightValue;
				break;
			case binary_operator::SHIFT_LEFT:
				value = leftValue << rightValue;
				break;
			case binary_operator::SHIFT_RIGHT:
				value = leftValue >> rightValue;
				break;
			case binary_operator::AND:
				value = leftValue & rightValue;
				break;
			case binary_operator::OR:
				value = leftValue | rightValue;
				break;
			case binary_operator::XOR:
				value = leftValue ^ rightValue;
				break;
			}

			if (left._register) {
				return evaluated_expression(left.location, *left._register, value);
			} else if (right._register) {
				return evaluated_expression(left.location, *right._register, value);
			} else {
				return evaluated_expression(left.location, value);
			}
		}

		evaluated_expression operator()(const unary_operation &expr) const {
			auto operand = apply_visitor(*this, expr.operand);

			if (operand._register) {
				throw invalid_argument(str(format("the operand of unary '%s' is not a literal")
					% expr._operator));
			}

			int32_t value = operand.value ? *operand.value : 0;

			switch (expr._operator) {
			case unary_operator::PLUS:
				value = +value;
				break;
			case unary_operator::MINUS:
				value = -value;
				break;
			case unary_operator::NOT:
				value = !value;
				break;
			case unary_operator::BITWISE_NOT:
				value = ~value;
				break;
			}

			return evaluated_expression(operand.location, value);
		}

		evaluated_expression operator()(const invalid_expression& expr) const {
			throw invalid_argument("invalid_expression found");
		}
	};

	/*************************************************************************
	 *
	 * evaluated function
	 *
	 *************************************************************************/
	bool evaluated(const expression &expr) {
		return apply_visitor(expression_evaluated(), expr);
	}

	/*************************************************************************
	 *
	 * evaluatable function
	 *
	 *************************************************************************/
	bool evaluatable(const expression &expr) {
		return apply_visitor(expression_evaluatable(), expr);
	}

	/*************************************************************************
	 *
	 * evaluate function
	 *
	 *************************************************************************/
	expression evaluate(const expression &expr) {
		return apply_visitor(expression_evaluator(), expr);
	}

	/*************************************************************************
	 *
	 * Stream operators
	 *
	 *************************************************************************/

	ostream& operator<< (ostream& stream, unary_operator op) {
		switch (op) {
		case unary_operator::PLUS:
			return stream << "+";
		case unary_operator::MINUS:
			return stream << "-";
		case unary_operator::NOT:
			return stream << "!";
		case unary_operator::BITWISE_NOT:
			return stream << "~";
		default:
			return stream << "<Unknown unary_operator " << static_cast<int>(op) << ">";
		}
	}

	ostream& operator<< (ostream& stream, binary_operator op) {
		switch (op) {
		case binary_operator::PLUS:
			return stream << "+";
		case binary_operator::MINUS:
			return stream << "-";
		case binary_operator::MULTIPLY:
			return stream << "*";
		case binary_operator::DIVIDE:
			return stream << "/";
		case binary_operator::MODULO:
			return stream << "%";
		case binary_operator::SHIFT_LEFT:
			return stream << "<<";
		case binary_operator::SHIFT_RIGHT:
			return stream << ">>";
		case binary_operator::AND:
			return stream << "&";
		case binary_operator::OR:
			return stream << "|";
		case binary_operator::XOR:
			return stream << "^";
		default:
			return stream << "<Unknown binary_operator " << static_cast<int>(op) << ">";
		}
	}

	ostream& operator<< (ostream& stream, const unary_operation &expr) {
		return stream << expr._operator << "(" << expr.operand << ")";
	}

	ostream& operator<< (ostream& stream, const binary_operation &expr) {
		return stream << "(" << expr.left << " " << expr._operator << " " << expr.right << ")";
	}

	ostream& operator<< (ostream& stream, const evaluated_expression &expr) {
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

	ostream& operator<< (ostream& stream, const register_operand &expr) {
		return stream << expr._register;
	}

	ostream& operator<< (ostream& stream, const symbol_operand &expr) {
		return stream << expr.label;
	}

	ostream& operator<< (ostream& stream, const literal_operand &expr) {
		return stream << expr.value;
	}

	ostream& operator<< (ostream& stream, const invalid_expression& expr) {
		return stream << "<invalid_expression>";
	}
}}
