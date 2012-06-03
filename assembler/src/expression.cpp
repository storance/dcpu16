#include <boost/variant.hpp>
#include <boost/format.hpp>

#include <stdexcept>

#include "expression.hpp"
#include "symbol_table.hpp"

using namespace std;
using namespace boost;

namespace dcpu { namespace assembler {
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
	symbol_operand::symbol_operand(const location_ptr &location, const string &name)
		: locatable(location), name(name), resolved_symbol(nullptr) {}


	bool symbol_operand::operator==(const symbol_operand& other) const {
		return name == other.name && resolved_symbol == other.resolved_symbol;
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
	 * current_position_operand
	 *
	 *************************************************************************/

	current_position_operand::current_position_operand(const location_ptr &location)
			: locatable(location), resolved_symbol(nullptr) {}

	bool current_position_operand::operator==(const current_position_operand& other) const {
		return resolved_symbol == other.resolved_symbol;
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
		bool operator()(const evaluated_expression &) const {
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

		bool operator()(const current_position_operand &expr) const {
			if (expr.resolved_symbol != nullptr) {
				if (expr.resolved_symbol->equ_expr != nullptr) {
					return apply_visitor(*this, *expr.resolved_symbol->equ_expr);
				}

				return true;
			}

			return false;
		}

		bool operator()(const symbol_operand &expr) const {
			if (expr.resolved_symbol != nullptr) {
				if (expr.resolved_symbol->equ_expr != nullptr) {
					return apply_visitor(*this, *expr.resolved_symbol->equ_expr);
				}

				return true;
			}

			return false;
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
	 * expression_evals_to_literal
	 *
	 *************************************************************************/
	class expression_evals_to_literal : public static_visitor<bool> {
	public:
		bool operator()(const evaluated_expression &expr) const {
			return !expr._register;
		}

		bool operator()(const register_operand &) const {
			return false;
		}

		bool operator()(const literal_operand &) const {
			return true;
		}

		bool operator()(const symbol_operand &expr) const {
			return true;
		}

		bool operator()(const current_position_operand &expr) const {
			return true;
		}

		bool operator()(const binary_operation &expr) const {
			return apply_visitor(*this, expr.left) && apply_visitor(*this, expr.right);
		}

		bool operator()(const unary_operation &expr) const {
			return apply_visitor(*this, expr.operand);
		}

		bool operator()(const invalid_expression&) const {
			return true;
		}
	};

	/*************************************************************************
	 *
	 * expression_evaluator
	 *
	 *************************************************************************/
	expression_evaluator::expression_evaluator(log &logger, bool intermediary_evaluation)
		: logger(logger), intermediary_evaluation(intermediary_evaluation) {}

	evaluated_expression expression_evaluator::operator()(const evaluated_expression &expr) const {
		return expr;
	}

	evaluated_expression expression_evaluator::operator()(const register_operand &expr) const {
		return evaluated_expression(expr.location, expr._register);
	}

	evaluated_expression expression_evaluator::operator()(const literal_operand &expr) const {
		return evaluated_expression(expr.location, expr.value);
	}

	evaluated_expression expression_evaluator::operator()(const current_position_operand &expr) const {
		if (!expr.resolved_symbol) {
			throw invalid_argument("unresolved symbols");
		}

		if (!expr.resolved_symbol->equ_expr) {
			return evaluated_expression(expr.location, expr.resolved_symbol->offset);
		}

		auto evaled_expr = apply_visitor(*this, *expr.resolved_symbol->equ_expr);
		evaled_expr.location = expr.location;

		return evaled_expr;
	}

	evaluated_expression expression_evaluator::operator()(const symbol_operand &expr) const {
		if (!expr.resolved_symbol) {
			throw invalid_argument("unresolved symbols");
		}

		if (!expr.resolved_symbol->equ_expr) {
			return evaluated_expression(expr.location, expr.resolved_symbol->offset);
		}

		auto evaled_expr = apply_visitor(*this, *expr.resolved_symbol->equ_expr);
		evaled_expr.location = expr.location;

		return evaled_expr;
	}

	evaluated_expression expression_evaluator::operator()(const binary_operation &expr) const {
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
			if (rightValue == 0) {
				if (!intermediary_evaluation) {
					logger.warning(right.location, "divide by zero");
				}
				value = 0;
			} else {
				value = leftValue / rightValue;
			}
			break;
		case binary_operator::MODULO:
			if (rightValue == 0) {
				if (!intermediary_evaluation) {
					logger.warning(right.location, "modulo by zero");
				}
				value = 0;
			} else {
				value = leftValue % rightValue;
			}
			break;
		case binary_operator::SHIFT_LEFT:
			value = leftValue << rightValue;
			break;
		case binary_operator::SHIFT_RIGHT:
			value = leftValue >> rightValue;
			break;
		case binary_operator::BITWISE_AND:
			value = leftValue & rightValue;
			break;
		case binary_operator::BITWISE_OR:
			value = leftValue | rightValue;
			break;
		case binary_operator::BITWISE_XOR:
			value = leftValue ^ rightValue;
			break;
		case binary_operator::EQ:
			value = leftValue == rightValue;
			break;
		case binary_operator::NEQ:
			value = leftValue != rightValue;
			break;
		case binary_operator::GTE:
			value = leftValue >= rightValue;
			break;
		case binary_operator::GT:
			value = leftValue > rightValue;
			break;
		case binary_operator::LTE:
			value = leftValue <= rightValue;
			break;
		case binary_operator::LT:
			value = leftValue < rightValue;
			break;
		case binary_operator::OR:
			value = leftValue || rightValue;
			break;
		case binary_operator::AND:
			value = leftValue && rightValue;
			break;
		default:
			throw invalid_argument(str(boost::format("Unknown binary operator %s") % expr._operator));
		}

		if (left._register) {
			return evaluated_expression(left.location, *left._register, value);
		} else if (right._register) {
			return evaluated_expression(left.location, *right._register, value);
		} else {
			return evaluated_expression(left.location, value);
		}
	}

	evaluated_expression expression_evaluator::operator()(const unary_operation &expr) const {
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

	evaluated_expression expression_evaluator::operator()(const invalid_expression& expr) const {
		throw invalid_argument("invalid_expression found");
	}

	/*************************************************************************
	 *
	 * get_evaluated_value
	 *
	 *************************************************************************/
	int32_t get_evaluated_value::operator()(const evaluated_expression &expr) const {
		if (expr._register || !expr.value) {
			throw invalid_argument("expression is a register expression");
		}

		return *expr.value;
	}

	/*************************************************************************
	 *
	 * helper functions
	 *
	 *************************************************************************/
	bool evaluated(const expression &expr) {
		return apply_visitor(expression_evaluated(), expr);
	}

	bool evaluatable(const expression &expr) {
		return apply_visitor(expression_evaluatable(), expr);
	}

	bool evaluates_to_literal(const expression &expr) {
		return apply_visitor(expression_evals_to_literal(), expr);
	}

	evaluated_expression evaluate(log &logger, const expression &expr, bool intermediary) {
		return apply_visitor(expression_evaluator(logger, intermediary), expr);
	}

	int32_t evaluated_value(const expression &expr) {
		return apply_visitor(get_evaluated_value(), expr);
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
		case binary_operator::BITWISE_AND:
			return stream << "&";
		case binary_operator::BITWISE_OR:
			return stream << "|";
		case binary_operator::BITWISE_XOR:
			return stream << "^";
		case binary_operator::EQ:
			return stream << "==";
		case binary_operator::NEQ:
			return stream << "!=";
		case binary_operator::GTE:
			return stream << ">=";
		case binary_operator::GT:
			return stream << ">";
		case binary_operator::LTE:
			return stream << "<=";
		case binary_operator::LT:
			return stream << "<";
		case binary_operator::OR:
			return stream << "||";
		case binary_operator::AND:
			return stream << "&&";
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
		return stream << expr.name;
	}

	ostream& operator<< (ostream& stream, const literal_operand &expr) {
		return stream << expr.value;
	}

	ostream& operator<< (ostream& stream, const current_position_operand &expr) {
		return stream << "$";
	}

	ostream& operator<< (ostream& stream, const invalid_expression& expr) {
		return stream << "<invalid_expression>";
	}
}}
