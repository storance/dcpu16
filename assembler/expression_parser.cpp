#include "expression_parser.hpp"
#include "mnemonics.hpp"

#include <boost/format.hpp>

using namespace std;
using namespace std::placeholders;
using namespace dcpu::ast;
using namespace dcpu::lexer;

const string UNARY_OPERAND_NOT_LITERAL = "operand for unary '%s' is not a literal expression";
const string LEFT_OPERAND_NOT_LITERAL = "left operand of '%s' is not a literal expression";
const string RIGHT_OPERAND_NOT_LITERAL = "right operand of '%s' is not a literal expression";
const string SYMBOL_NOT_ALLOWED = "symbol not allowed in expression";
const string REGISTER_NOT_ALLOWED = "register not allowed in expression";
const string REGISTER_NOT_INDIRECTABLE = "register '%s' is not indirectable.";
const string MULTIPLE_REGISTERS = "multiple registers in expression; first register '%s' at %s";

namespace dcpu { namespace parser {

	operator_definition::operator_definition(binary_operator _operator, token_predicate is_operator)
			: _operator(_operator), is_operator(is_operator), left_literal(true), right_literal(true) {}

	operator_definition::operator_definition(ast::binary_operator _operator, token_predicate is_operator,
			bool left_literal, bool right_literal) : _operator(_operator), is_operator(is_operator),
			left_literal(left_literal), right_literal(right_literal) {}

	const operator_definition* operator_definition::lookup(initializer_list<operator_definition> definitions,
			const token &current_token) {
		for (auto& definition : definitions) {
			if (definition.is_operator(&current_token)) {
				return &definition;
			}
		}

		return nullptr;
	}

	register_location::register_location(const location_ptr &location, registers _register)
		: location(location), _register(_register) {}


	expression_parser::expression_parser(token_iterator& current, token_iterator end, error_handler_ptr& error_handler,
		bool allow_registers, bool allow_symbols, bool indirection)
		: current(current), end(end), error_handler(error_handler), allow_symbols(allow_symbols),
		  allow_registers(allow_registers), indirection(indirection), first_register() {}

	expression expression_parser::parse(const token& current_token) {
		return parse_bitwise_or(current_token);
	}

	expression expression_parser::parse_bitwise_or(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_bitwise_xor, {
			operator_definition(binary_operator::OR, bind(&token::is_character, _1, '|'))
		});
	}

	expression expression_parser::parse_bitwise_xor(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_bitwise_and, {
			operator_definition(binary_operator::XOR, bind(&token::is_character, _1, '%'))
		});
	}

	expression expression_parser::parse_bitwise_and(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_bitwise_shift, {
			operator_definition(binary_operator::AND, bind(&token::is_character, _1, '&'))
		});
	}

	expression expression_parser::parse_bitwise_shift(const token& current_token) {
		return parse_binary_operation(current_token, &expression_parser::parse_add, {
			operator_definition(binary_operator::SHIFT_LEFT,  mem_fn(&token::is_shift_left)),
			operator_definition(binary_operator::SHIFT_RIGHT, mem_fn(&token::is_shift_right)),
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
		initializer_list<operator_definition> definitions) {

		expression left = (this->*expr_parser)(current_token);

		while (true) {
			auto &operator_token = next_token();
			const operator_definition *definition = operator_definition::lookup(definitions, operator_token);
			if (definition == nullptr) {
				--current;
				return left;
			}

			expression right = (this->*expr_parser)(next_token());
			validate_expressions(definition, left, right);

			left = binary_operation(operator_token.location, definition->_operator, left, right);
		}
	}

	void expression_parser::validate_expressions(const operator_definition *definition, expression &left,
		expression &right) {

		bool left_literal = definition->left_literal || !indirection;
		bool right_literal = definition->right_literal || !indirection;

		if (left_literal && !evaluates_to_literal(left)) {
			error_handler->error(get_location(left), boost::format(LEFT_OPERAND_NOT_LITERAL) % definition->_operator);
			left = invalid_expression(get_location(left));
		}

		if (right_literal && !evaluates_to_literal(right)) {
			error_handler->error(get_location(right), boost::format(RIGHT_OPERAND_NOT_LITERAL) % definition->_operator);
			right = invalid_expression(get_location(right));
		}
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
			error_handler->error(current_token.location, boost::format(UNARY_OPERAND_NOT_LITERAL) % _operator);
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
			error_handler->error(current_token.location, boost::format("expected a primary-expression before '%s'")
				% current_token.content);
			return invalid_expression(current_token.location);
		}
	}

	expression expression_parser::parse_grouping(const token& current_token) {
		expression expr = parse(current_token);

		auto& next_tkn = next_token();
		if (!next_tkn.is_character(')')) {
			--current;
			error_handler->unexpected_token(next_tkn, ')');
		}

		return expr;
	}

	expression expression_parser::parse_register(const token& current_token) {
		auto definition = current_token.get_register();

		if (!allow_registers) {
			error_handler->error(current_token.location, boost::format(REGISTER_NOT_ALLOWED) % definition._register);

			return invalid_expression(current_token.location);
		}

		if (first_register) {
			error_handler->error(current_token.location, boost::format(MULTIPLE_REGISTERS)
					% (*first_register)._register % (*first_register).location);

			return invalid_expression(current_token.location);
		} else {
			first_register = register_location(current_token.location, definition._register);
		}

		if (indirection && !definition.indirectable) {
			error_handler->error(current_token.location, boost::format(REGISTER_NOT_INDIRECTABLE)
					% definition._register);

			return invalid_expression(current_token.location);
		}

		return register_operand(current_token.location, definition._register);
	}

	expression expression_parser::parse_symbol(const token& current_token) {
		if (!allow_symbols) {
			error_handler->error(current_token.location, boost::format(SYMBOL_NOT_ALLOWED) % current_token.content);

			return invalid_expression(current_token.location);
		}

		return symbol_operand(current_token.location, current_token.content);
	}

	expression expression_parser::parse_literal(const token& current_token) {
		return literal_operand(current_token.location, current_token.get_integer());
	}

	token& expression_parser::next_token() {
		return next(current, end);
	}
}}
