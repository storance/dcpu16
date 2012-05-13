#pragma once

#include <initializer_list>
#include <functional>

#include "expression.hpp"
#include "log.hpp"

namespace dcpu { namespace parser {
	typedef std::function<bool (const lexer::token*)> token_predicate;

	struct operator_definition {
		ast::binary_operator _operator;
		token_predicate is_operator;
		bool left_literal;
		bool right_literal;

		operator_definition(ast::binary_operator, token_predicate);
		operator_definition(ast::binary_operator, token_predicate, bool, bool);

		static const operator_definition* lookup(std::initializer_list<operator_definition> definitions,
				const lexer::token &current_token);
	};

	class register_location {
	public:
		lexer::location_ptr location;
		registers _register;

		register_location(const lexer::location_ptr &location, registers _register);
	};

	class expression_parser {
	protected:
		typedef ast::expression (expression_parser::*expr_parser_t)(const lexer::token&);

		lexer::token_iterator &current, end;
		logging::log& logger;
		bool allow_symbols, allow_registers, indirection;
		boost::optional<register_location> first_register;

		void validate_expressions(const operator_definition *, ast::expression&, ast::expression&);
		ast::expression parse_binary_operation(const lexer::token&, expr_parser_t, std::initializer_list<operator_definition>);
		ast::expression parse_primary(const lexer::token&);
		ast::expression parse_grouping(const lexer::token&);
		ast::expression parse_register(const lexer::token&);
		ast::expression parse_symbol(const lexer::token&);
		ast::expression parse_literal(const lexer::token&);
		ast::expression parse_unary(const lexer::token&);
		ast::expression parse_multiply(const lexer::token&);
		ast::expression parse_add(const lexer::token&);
		ast::expression parse_bitwise_shift(const lexer::token&);
		ast::expression parse_bitwise_and(const lexer::token&);
		ast::expression parse_bitwise_xor(const lexer::token&);
		ast::expression parse_bitwise_or(const lexer::token&);
		ast::expression parse_relational_order(const lexer::token&);
		ast::expression parse_relational_equals(const lexer::token&);
		ast::expression parse_boolean_and(const lexer::token&);
		ast::expression parse_boolean_or(const lexer::token&);

		lexer::token& next_token();
	public:
		expression_parser(lexer::token_iterator&, lexer::token_iterator, logging::log&, bool, bool, bool);

		ast::expression parse(const lexer::token&);
	};

}}
