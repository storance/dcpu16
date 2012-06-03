#pragma once

#include <vector>
#include <functional>

#include "expression.hpp"
#include "log.hpp"

namespace dcpu { namespace assembler {
	typedef std::function<bool (const token*)> token_predicate;

	struct operator_definition {
		binary_operator _operator;
		token_predicate is_operator;
		bool left_literal;
		bool right_literal;

		operator_definition(binary_operator _operator, token_predicate is_operator, bool left_literal=true,
				bool right_literal=true);

		static boost::optional<operator_definition> lookup(const std::vector<operator_definition> &definitions,
				const token &current_token);
	};

	class register_location {
	public:
		location_ptr location;
		registers _register;

		register_location(const location_ptr &location, registers _register);
	};

	class expression_parser {
	protected:
		typedef expression (expression_parser::*expr_parser_t)(const token&);

		token_iterator &current, end;
		log& logger;
		uint32_t allowed_flags;
		boost::optional<register_location> first_register;

		bool is_expression_valid(const operator_definition &defintion, const location_ptr &location,
				expression &left, expression &right);
		expression parse_binary_operation(const token&, expr_parser_t,
				const std::vector<operator_definition>&);
		expression parse_primary(const token&);
		expression parse_grouping(const token&);
		expression parse_register(const token&);
		expression parse_symbol(const token&);
		expression parse_literal(const token&);
		expression parse_unary(const token&);
		expression parse_multiply(const token&);
		expression parse_add(const token&);
		expression parse_bitwise_shift(const token&);
		expression parse_bitwise_and(const token&);
		expression parse_bitwise_xor(const token&);
		expression parse_bitwise_or(const token&);
		expression parse_relational_order(const token&);
		expression parse_relational_equals(const token&);
		expression parse_boolean_and(const token&);
		expression parse_boolean_or(const token&);

		token& next_token();

		bool is_register_allowed(registers _register);
		bool is_register_in_expressions_allowed();
		bool is_symbols_allowed();
		bool is_current_position_allowed();
	public:
		expression_parser(token_iterator&, token_iterator, log&, uint32_t allowed_flags);

		expression parse(const token&);

		enum {
			REGISTER_A=1 << 0,
			REGISTER_B=1 << 1,
			REGISTER_C=1 << 2,
			REGISTER_X=1 << 3,
			REGISTER_Y=1 << 4,
			REGISTER_Z=1 << 5,
			REGISTER_I=1 << 6,
			REGISTER_J=1 << 7,
			REGISTER_SP=1 << 8,
			REGISTER_PC=1 << 9,
			REGISTER_EX=1 << 10,
			REGISTER_EXPRESSIONS=1 << 11,
			SYMBOL=1 << 12,
			CURRENT_POSITION=1 << 13,

			SCALAR = 0,
			CONSTANT = SYMBOL | CURRENT_POSITION,
			INDIRECT = REGISTER_A | REGISTER_B | REGISTER_C | REGISTER_X | REGISTER_Y | REGISTER_Z | REGISTER_I |
					   REGISTER_J | REGISTER_SP | REGISTER_EXPRESSIONS | CONSTANT,
			DIRECT = REGISTER_A | REGISTER_B | REGISTER_C | REGISTER_X | REGISTER_Y | REGISTER_Z | REGISTER_I |
					 REGISTER_J | REGISTER_SP | REGISTER_PC | REGISTER_EX | CONSTANT,

		};
	};

}}
