#include <boost/format.hpp>
#include <boost/variant.hpp>

#include "expression_parser.hpp"
#include "mnemonics.hpp"

using namespace std;
using namespace std::placeholders;
using namespace dcpu::ast;
using namespace dcpu::lexer;

namespace dcpu { namespace parser {
	operator_definition::operator_definition(binary_operator _operator, token_predicate is_operator,
			bool left_literal, bool right_literal) : _operator(_operator), is_operator(is_operator),
			left_literal(left_literal), right_literal(right_literal) {}

	boost::optional<operator_definition> operator_definition::lookup(const vector<operator_definition> &definitions,
			const token &current_token) {
		for (auto& definition : definitions) {
			if (definition.is_operator(&current_token)) {
				return definition;
			}
		}

		return boost::none;
	}

	register_location::register_location(const location_ptr &location, registers _register)
		: location(location), _register(_register) {}


	expression_parser::expression_parser(token_iterator& current, token_iterator end, logging::log &logger,
		uint32_t allowed_flags)
		: current(current), end(end), logger(logger), allowed_flags(allowed_flags), first_register() {}

	expression expression_parser::parse(const token& current_token) {
		return parse_boolean_or(current_token);
	}

	expression expression_parser::parse_boolean_or(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_boolean_and, {
			operator_definition(binary_operator::OR, bind(&token::is_operator, _1, operator_type::OR))
		});
	}

	expression expression_parser::parse_boolean_and(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_bitwise_or, {
			operator_definition(binary_operator::AND, bind(&token::is_operator, _1, operator_type::AND))
		});
	}

	expression expression_parser::parse_bitwise_or(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_bitwise_xor, {
			operator_definition(binary_operator::BITWISE_OR, bind(&token::is_character, _1, '|'))
		});
	}

	expression expression_parser::parse_bitwise_xor(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_bitwise_and, {
			operator_definition(binary_operator::BITWISE_XOR, bind(&token::is_character, _1, '^'))
		});
	}

	expression expression_parser::parse_bitwise_and(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_relational_equals, {
			operator_definition(binary_operator::BITWISE_AND, bind(&token::is_character, _1, '&'))
		});
	}

	expression expression_parser::parse_relational_equals(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_relational_order, {
			operator_definition(binary_operator::EQ,  bind(&token::is_operator, _1, operator_type::EQ)),
			operator_definition(binary_operator::NEQ, bind(&token::is_operator, _1, operator_type::NEQ))
		});
	}

	expression expression_parser::parse_relational_order(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_bitwise_shift, {
			operator_definition(binary_operator::GTE, bind(&token::is_operator, _1, operator_type::GTE)),
			operator_definition(binary_operator::LTE, bind(&token::is_operator, _1, operator_type::LTE)),
			operator_definition(binary_operator::GT,  bind(&token::is_character, _1, '<')),
			operator_definition(binary_operator::LT,  bind(&token::is_character, _1, '>'))
		});
	}

	expression expression_parser::parse_bitwise_shift(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_add, {
			operator_definition(binary_operator::SHIFT_LEFT,  bind(&token::is_operator, _1, operator_type::SHIFT_LEFT)),
			operator_definition(binary_operator::SHIFT_RIGHT, bind(&token::is_operator, _1, operator_type::SHIFT_RIGHT)),
		});
	}

	expression expression_parser::parse_add(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_multiply, {
			operator_definition(binary_operator::PLUS,  bind(&token::is_character, _1, '+'), false, false),
			operator_definition(binary_operator::MINUS, bind(&token::is_character, _1, '-'), false, true)
		});
	}

	expression expression_parser::parse_multiply(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_unary, {
			operator_definition(binary_operator::MULTIPLY, bind(&token::is_character, _1, '*')),
			operator_definition(binary_operator::DIVIDE,   bind(&token::is_character, _1, '/')),
			operator_definition(binary_operator::MODULO,   bind(&token::is_character, _1, '%'))
		});
	}

	expression expression_parser::parse_binary_operation(const token& current_token, expr_parser_t expr_parser,
		const vector<operator_definition> &definitions) {

		expression left = (this->*expr_parser)(current_token);

		while (true) {
			auto &operator_token = next_token();
			auto definition = operator_definition::lookup(definitions, operator_token);
			if (!definition) {
				--current;
				break;
			}

			expression right = (this->*expr_parser)(next_token());
			if (!is_expression_valid(*definition, current_token.location, left, right)) {
				left = invalid_expression(current_token.location);
			} else {
				left = binary_operation(operator_token.location, definition->_operator, left, right);
			}
		}

		return left;
	}

	bool expression_parser::is_expression_valid(const operator_definition& definition, const location_ptr& location,
			expression &left, expression &right) {

		bool left_invalid = (definition.left_literal || !is_register_in_expressions_allowed())
				&& !evaluates_to_literal(left);
		bool right_invalid = (definition.right_literal || !is_register_in_expressions_allowed())
				&& !evaluates_to_literal(right);

		if (left_invalid || right_invalid) {
			logger.error(location, boost::format("non-constant operands for operator '%s'")
					% definition._operator);
			return false;
		}

		return true;
	}

	expression expression_parser::parse_unary(const token& current_token) {
		unary_operator _operator;
		if (current_token.is_character('+')) {
			_operator = unary_operator::PLUS;
		} else if (current_token.is_character('-')) {
			_operator = unary_operator::MINUS;
		} else if (current_token.is_character('~')) {
			_operator = unary_operator::BITWISE_NOT;
		} else if (current_token.is_character('!')) {
			_operator = unary_operator::NOT;
		} else {
			return parse_primary(current_token);
		}

		expression operand = parse_unary(next_token());
		if (!evaluates_to_literal(operand)) {
			logger.error(current_token.location, boost::format("non-constant operand for unary operator '%s'")
					% _operator);
			return invalid_expression(current_token.location);
		}

		return unary_operation(current_token.location, _operator, operand);
	}

	expression expression_parser::parse_primary(const token& current_token) {
		if (current_token.is_character('(')) {
			return parse_grouping(next_token());
		} else if (current_token.is_register()) {
			return parse_register(current_token);
		} else if (current_token.is_symbol()) {
			return parse_symbol(current_token);
		} else if (current_token.is_integer()) {
			return parse_literal(current_token);
		} else if (current_token.is_character('$')) {
			return current_position_operand(current_token.location);
		} else {
			--current;
			logger.error(current_token.location, boost::format("expected a primary-expression before '%s'")
				% current_token.content);
			return invalid_expression(current_token.location);
		}
	}

	expression expression_parser::parse_grouping(const token& current_token) {
		expression expr = parse(current_token);

		auto& next_tkn = next_token();
		if (!next_tkn.is_character(')')) {
			--current;
			logger.unexpected_token(next_tkn, ')');
		}

		return expr;
	}

	expression expression_parser::parse_register(const token& current_token) {
		auto _register = current_token.get_register();

		if (!is_register_allowed(_register)) {
			logger.error(current_token.location, boost::format("register '%s' is not allowed here") % _register);

			return invalid_expression(current_token.location);
		}

		if (first_register) {
			logger.error(current_token.location, boost::format("multiple registers in expression; "
					"first register '%s' at %s") % (*first_register)._register % (*first_register).location);

			return invalid_expression(current_token.location);
		} else {
			first_register = register_location(current_token.location, _register);
		}


		return register_operand(current_token.location, _register);
	}

	expression expression_parser::parse_symbol(const token& current_token) {
		if (!is_symbols_allowed()) {
			logger.error(current_token.location, "symbols not allowed here");

			return invalid_expression(current_token.location);
		}

		return symbol_operand(current_token.location, current_token.content);
	}

	expression expression_parser::parse_literal(const token& current_token) {
		return literal_operand(current_token.location, current_token.get_integer());
	}

	bool expression_parser::is_register_allowed(registers _register) {
		switch (_register) {
		case registers::A:
			return allowed_flags & REGISTER_A;
		case registers::B:
			return allowed_flags & REGISTER_B;
		case registers::C:
			return allowed_flags & REGISTER_C;
		case registers::X:
			return allowed_flags & REGISTER_X;
		case registers::Y:
			return allowed_flags & REGISTER_Y;
		case registers::Z:
			return allowed_flags & REGISTER_Z;
		case registers::I:
			return allowed_flags & REGISTER_I;
		case registers::J:
			return allowed_flags & REGISTER_J;
		case registers::SP:
			return allowed_flags & REGISTER_SP;
		case registers::PC:
			return allowed_flags & REGISTER_PC;
		case registers::EX:
			return allowed_flags & REGISTER_EX;
		default:
			return false;
		}
	}

	bool expression_parser::is_register_in_expressions_allowed() {
		return allowed_flags & REGISTER_EXPRESSIONS;
	}

	bool expression_parser::is_symbols_allowed() {
		return allowed_flags & SYMBOL;
	}

	bool expression_parser::is_current_position_allowed() {
		return allowed_flags & CURRENT_POSITION;
	}

	token& expression_parser::next_token() {
		return next(current, end);
	}
}}
