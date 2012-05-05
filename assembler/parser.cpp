#include "parser.hpp"

#include <functional>
#include <iostream>
#include <iterator>
#include <map>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "mnemonics.hpp"

using namespace std;
using namespace placeholders;
using namespace dcpu::lexer;
using namespace dcpu::ast;
using namespace boost::algorithm;

namespace dcpu { namespace parser {
	parser::parser(lexer::lexer &lex, ast::statement_list &statements) : current(lex.tokens.begin()),
			end(lex.tokens.end()), error_handler(lex.error_handler), statements(statements) {}

	void parser::parse() {
		while (current != end) {
			auto current_token = next_token();

			// we've reached the end
			if (current_token.is_eoi()) {
				break;
			}

			// skip empty lines
			if (current_token.is_newline()) {
				continue;
			}

			auto label = parse_label(current_token);
			if (label) {
				statements.push_back(*label);
				current_token = next_token();

				if (current_token.is_terminator()) {
					continue;
				}
			}

			auto data = parse_data(current_token);
			if (data) {
				statements.push_back(*data);
				continue;
			}

			auto instruction = parse_instruction(current_token);
			if (instruction) {
				statements.push_back(*instruction);
			}
		}
	}

	boost::optional<statement> parser::parse_label(const token& current_token) {
		if (!current_token.is_label()) {
			return boost::none;
		}

		return statement(label(current_token.location, current_token.content));
	}

	boost::optional<statement> parser::parse_instruction(const token& current_token) {
		if (!current_token.is_identifier()) {
			error_handler->unexpected_token(current_token, "label, instruction, or assembler directive");

			return boost::none;
		}

		const opcode_definition* definition = opcode_definition::lookup(current_token.content);

		if (!definition) {
			error_handler->error(current_token.location, boost::format("unrecognized instruction '%s'")
					% current_token.content);
			advance_until(mem_fn(&token::is_terminator));
			return boost::none;
		}

		optional_argument a, b;
		if (definition->args == 1) {
			a = parse_argument(next_token(), argument_position::A);

			if (!a) {
				advance_until(mem_fn(&token::is_terminator));
				return boost::none;
			}
		} else if (definition->args == 2) {
			b = parse_argument(next_token(), argument_position::B);

			if (!b) {
				advance_until([] (const token& token) {
					return token.is_character(',') || token.is_terminator();
				});
			}

			auto& next_tkn = next_token();
			if (!next_tkn.is_character(',')) {
				error_handler->unexpected_token(next_tkn, ',');
				move_back();
			}

			a = parse_argument(next_token(), argument_position::A);
			if (!a || !b) {
				advance_until(mem_fn(&token::is_terminator));
				return boost::none;
			}
		}

		auto& eoltoken = next_token();
		if (!eoltoken.is_terminator()) {
			error_handler->unexpected_token(eoltoken, "'newline' or 'eof'");
			advance_until(mem_fn(&token::is_terminator));
		}

		return statement(instruction(current_token.location, definition->opcode, *a, b));
	}

	boost::optional<statement> parser::parse_data(const token& current_token) {
		if (!current_token.is_identifier()) {
			return boost::none;
		}

		if (!iequals(current_token.content, "dat") && !iequals(current_token.content, "data")) {
			return boost::none;
		}

		data data_stmt(current_token.location);

		auto next_tkn = next_token();
		while (!next_tkn.is_terminator()) {
			if (next_tkn.is_quoted_string()) {
				data_stmt.append(next_tkn.content);
			} else if (next_tkn.is_integer()) {
				if (next_tkn.value > UINT16_MAX) {
					error_handler->warning(next_tkn.location, "overflow in converting to 16-bit word");
				}

				data_stmt.append(next_tkn.value);
			} else {
				break;
			}

			next_tkn = next_token();
			if (!next_tkn.is_character(',')) {
				break;
			}

			next_tkn = next_token();
		}

		if (!next_tkn.is_terminator()) {
			error_handler->unexpected_token(next_tkn, "a character, string, or integer literal");
		}

		if (data_stmt.value.size() == 0) {
			error_handler->warning(current_token.location, "empty data segment");
		}

		return statement(data_stmt);
	}

	optional_argument parser::parse_argument(const token& current_token, argument_position position) {
		if (current_token.is_character(',') || current_token.is_terminator()) {
			error_handler->unexpected_token(current_token, "an instruction argument");
			return boost::none;
		}

		if (current_token.is_character('[')) {
			auto argument = parse_indirect_stack_argument(next_token(), position);
			if (!argument) {
				move_back();
				argument = ast::argument(expression_argument(current_token.location, position,
						parse_expression(next_token(), true, true), true, false));
			}

			auto &next_tkn = next_token();
			if (!next_tkn.is_character(']')) {
				move_back();
				error_handler->unexpected_token(next_tkn, ']');
				return boost::none;
			}

			return argument;
		} else if (current_token.is_identifier()) {
			auto stackArgument = parse_mnemonic_stack_argument(current_token, position);
			if (stackArgument) {
				return stackArgument;
			}
		}

		return ast::argument(expression_argument(current_token.location, position,
				parse_expression(current_token, true, false), false, false));
	}

	optional_argument parser::parse_indirect_stack_argument(const token& current_token, argument_position position) {
		if (current_token.is_identifier() && iequals(current_token.content, "SP")) {

			auto& next_tkn = next_token();
			if (next_tkn.is_character(']')) {
				move_back();
				return argument(stack_argument(current_token.location, position, stack_operation::PEEK));
			} else if (next_tkn.is_increment()) {
				if (position != argument_position::A) {
					error_handler->error(current_token.location, "[SP++] is not allowed as argument b.");
				}

				return argument(stack_argument(current_token.location, position, stack_operation::POP));
			}

			move_back();
		} else if (current_token.is_decrement()) {
			auto& next_tkn = next_token();
			if (next_tkn.is_identifier() && iequals(next_tkn.content, "SP")) {
				if (position != argument_position::B) {
					error_handler->error(current_token.location, "[--SP] is not allowed as argument a.");
				}

				return argument(stack_argument(current_token.location, position, stack_operation::PUSH));
			}

			move_back();
		}

		return boost::none;
	}

	optional_argument parser::parse_mnemonic_stack_argument(const token& current_token, argument_position position) {
		if (iequals(current_token.content, "PUSH")) {
			if (position != argument_position::B) {
				error_handler->error(current_token.location, "PUSH is not allowed as argument a.");
			}

			return argument(stack_argument(current_token.location, position, stack_operation::PUSH));
		} else if (iequals(current_token.content, "POP")) {
			if (position != argument_position::A) {
				error_handler->error(current_token.location, "POP is not allowed as argument a.");
			}

			return argument(stack_argument(current_token.location, position, stack_operation::POP));
		} else if (iequals(current_token.content, "PEEK")) {
			return argument(stack_argument(current_token.location, position, stack_operation::PEEK));
		} else if (iequals(current_token.content, "PICK")) {
			auto pick_expr = binary_operation(current_token.location, binary_operator::PLUS,
					register_operand(current_token.location, registers::SP),
					parse_expression(next_token(), false, false));

			if (evaluatable(pick_expr)) {
				return argument(expression_argument(current_token.location, position, evaluate(pick_expr), true, false));
			} else {
				return argument(expression_argument(current_token.location, position, pick_expr, true, false));
			}
		}

		return boost::none;
	}

	expression parser::parse_expression(const token& current_token, bool allow_registers, bool indirection) {
		expression_parser expr_parser(current, end, error_handler, allow_registers, true, indirection);
		auto expr = expr_parser.parse(current_token);
		if (evaluatable(expr)) {
			return evaluate(expr);
		}

		return expr;
	}

	void parser::advance_until(function<bool (const token&)> predicate) {
		while (current != end) {
			if (predicate(*current)) {
				break;
			}

			current++;
		}
	}

	token& parser::next_token() {
		return next(current, end);
	}

	void parser::move_back() {
		--current;
	}

}}
