#include <boost/format.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

#include <climits>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>

#include "parser.hpp"

using namespace std;
using namespace placeholders;
using namespace dcpu::lexer;
using namespace dcpu::ast;

namespace dcpu { namespace parser {
	parser::parser(lexer::lexer &lex, ast::statement_list &statements) : current(lex.tokens.begin()),
			end(lex.tokens.end()), logger(lex.logger), statements(statements),
			instructions_found(0), labels_found(0) {}

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
				labels_found++;
				statements.push_back(*label);
				current_token = next_token();

				if (current_token.is_terminator()) {
					continue;
				}
			}

			auto directive = parse_directive(current_token);
			if (directive) {
				statements.push_back(*directive);
				continue;
			}

			auto instruction = parse_instruction(current_token);
			if (instruction) {
				instructions_found++;
				statements.push_back(*instruction);
				continue;
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
		if (!current_token.is_instruction()) {
			logger.unexpected_token(current_token, "a label, instruction, or directive");
			return boost::none;
		}

		auto definition = current_token.get_instruction();

		optional_argument a, b;
		if (definition.args == 1) {
			a = parse_argument(next_token(), argument_position::A);

			if (!a) {
				advance_until(mem_fn(&token::is_terminator));
				return boost::none;
			}
		} else if (definition.args == 2) {
			b = parse_argument(next_token(), argument_position::B);

			if (!b) {
				advance_until([] (const token& token) {
					return token.is_character(',') || token.is_terminator();
				});
			}

			auto& next_tkn = next_token();
			if (!next_tkn.is_character(',')) {
				logger.unexpected_token(next_tkn, ',');
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
			logger.unexpected_token(eoltoken, "'newline' or 'eof'");
			advance_until(mem_fn(&token::is_terminator));
		}

		return statement(instruction(current_token.location, definition.opcode, *a, b));
	}

	boost::optional<statement> parser::parse_directive(const token& current_token) {
		if (!current_token.is_directive()) {
			return boost::none;
		}

		boost::optional<statement> result;
		auto directive = current_token.get_directive();
		switch (directive) {
		case directives::DW:
			result = statement(parse_data_word(current_token));
			break;
		case directives::DB:
			result = statement(parse_data_byte(current_token));
			break;
		case directives::ORG:
			result = statement(parse_org(current_token));
			break;
		case directives::EQU:
			result = statement(parse_equ(current_token));
			break;
		case directives::FILL:
			result = statement(parse_fill(current_token));
			break;
		case directives::ALIGN:
			result = statement(parse_align(current_token));
			break;
		default:
			logger.warning(current_token.location, boost::format("directive '%s' is not yet supported")
					% directive);
			return boost::none;
		}

		auto next_tkn = next_token();
		if (!next_tkn.is_terminator()) {
			logger.unexpected_token(next_tkn, "a 'newline'");
			advance_until(mem_fn(&token::is_terminator));
		}

		return result;
	}

	data_directive parser::parse_data_word(const token& current_token) {
		data_directive data(current_token.location);
		parse_data(next_token(), data.value, false);

		return data;
	}

	data_directive parser::parse_data_byte(const token& current_token) {
		data_directive data(current_token.location);

		vector<uint16_t> words;
		parse_data(next_token(), words, true);

		for (auto it = words.begin(); it != words.end(); it++) {
			uint16_t result = ((uint8_t)*it) << 8;

			if (++it == words.end()) {
				data.value.push_back(result);
				break;
			} else {
				result |= (uint8_t)*it;
				data.value.push_back(result);
			}
		}

		return data;
	}

	void parser::parse_data(const token& token, vector<uint16_t> &output, bool packed) {

		auto current_token = token;
		while (!current_token.is_terminator()) {
			if (current_token.is_quoted_string()) {
				copy (current_token.content.begin(), current_token.content.end(), back_inserter(output));
			} else if (current_token.is_integer()) {
				uint32_t value = current_token.get_integer();
				if (!packed && value > numeric_limits<uint16_t>::max()) {
					logger.warning(current_token.location, "overflow in converting to 16-bit integer");
				} else if (packed && value > numeric_limits<uint8_t>::max()) {
					logger.warning(current_token.location, "overflow in converting to 8-bit integer");
				}

				output.push_back(value);
			} else {
				break;
			}

			current_token = next_token();
			if (!current_token.is_character(',')) {
				break;
			}

			current_token = next_token();
		}

		move_back();

		if (output.size() == 0) {
			logger.warning(current_token.location, "empty data segment");
		}
	}

	org_directive parser::parse_org(const token& current_token) {
		if (instructions_found || labels_found) {
			logger.error(current_token.location, ".ORG must occur before all labels and instructions");
		}

		auto expr = parse_expression(next_token(), expression_parser::SCALAR);
		if (!evaluated(expr)) {
			// the only way this can happen is if we failed to parse the expression
			return org_directive(current_token.location, 0);
		}

		int32_t value = evaluated_value(expr);
		if (value < 0) {
			logger.error(current_token.location, "negative org value");
		}

		return org_directive(current_token.location, value);
	}

	equ_directive parser::parse_equ(const lexer::token &current_token) {
		auto expr = parse_expression(next_token(), expression_parser::CONSTANT);

		if (statements.empty() || !boost::get<label>(&statements.front())) {
			logger.error(current_token.location, ".EQU must be preceded by a label");
		}

		return equ_directive(current_token.location, expr);
	}

	fill_directive parser::parse_fill(const lexer::token &current_token) {
		auto count_expr = parse_expression(next_token(), expression_parser::CONSTANT);

		auto next_tkn = next_token();
		if (next_tkn.is_terminator()) {
			move_back();
			return fill_directive(current_token.location, count_expr, evaluated_expression(next_tkn.location, 0));
		} else if (next_tkn.is_character(',')) {
			return fill_directive(current_token.location, count_expr, parse_expression(next_token(),
					expression_parser::CONSTANT));
		} else {
			logger.unexpected_token(next_tkn, "',' or 'newline'");
			return fill_directive(current_token.location, count_expr, invalid_expression(next_tkn.location));
		}
	}

	align_directive parser::parse_align(const lexer::token &current_token) {
		auto expr = parse_expression(next_token(), expression_parser::SCALAR);
		if (!evaluated(expr)) {
			// the only way this can happen is if we failed to parse the expression
			return align_directive(current_token.location, 1);
		}

		int32_t value = evaluated_value(expr);
		if (value <= 0) {
			logger.error(current_token.location, "align boundary must be greater than zero");
		} else if (value == 1) {
			logger.warning(current_token.location, "align boundary of one has no effect");
		}

		return align_directive(current_token.location, value);
	}

	optional_argument parser::parse_argument(const token& current_token, argument_position position) {
		if (current_token.is_character(',') || current_token.is_terminator()) {
			logger.unexpected_token(current_token, "an instruction argument");
			return boost::none;
		}

		if (current_token.is_character('[')) {
			return parse_indirect_argument(next_token(), position);
		} else if (current_token.is_stack_operation()) {
			return  parse_stack_argument(current_token, position);
		}

		return argument(expression_argument(current_token.location, position,
				parse_expression(current_token, expression_parser::DIRECT), false, false));
	}

	optional_argument parser::parse_indirect_argument(const token& current_token, argument_position position) {
		argument arg(expression_argument(current_token.location, position,
				parse_expression(current_token, expression_parser::INDIRECT), true, false));

		auto &end_token = next_token();
		if (!end_token.is_character(']')) {
			move_back();
			logger.unexpected_token(end_token, ']');
			return boost::none;
		}

		return arg;
	}

	optional_argument parser::parse_stack_argument(const token& current_token, argument_position position) {
		stack_operation operation = current_token.get_stack_operation();

		if (operation == stack_operation::PICK) {
			auto pick_expr = binary_operation(current_token.location, binary_operator::PLUS,
					register_operand(current_token.location, registers::SP),
					parse_expression(next_token(), expression_parser::CONSTANT));

			if (evaluatable(pick_expr)) {
				return argument(expression_argument(current_token.location, position, evaluate(logger, pick_expr),
						true, false));
			} else {
				return argument(expression_argument(current_token.location, position, pick_expr, true, false));
			}
		}

		if (operation == stack_operation::PUSH && position == argument_position::A) {
			logger.error(current_token.location, "[--SP] / PUSH is not allowed as argument a");
		} else if (operation == stack_operation::POP && position == argument_position::B) {
			logger.error(current_token.location, "[SP++] / POP is not allowed as argument b");
		}

		return argument(stack_argument(current_token.location, position, operation));
	}

	expression parser::parse_expression(const token& current_token, uint32_t flags) {
		expression_parser expr_parser(current, end, logger, flags);
		auto expr = expr_parser.parse(current_token);
		if (evaluatable(expr)) {
			auto evaled_expr = evaluate(logger, expr);

			if (evaled_expr.value) {
				if (evaled_expr.value > (int32_t)numeric_limits<uint16_t>::max()) {
					logger.warning(evaled_expr.location, "overflow in converting to 16-bit integer");
				} else if (evaled_expr.value < (int32_t)numeric_limits<int16_t>::min()) {
					logger.warning(evaled_expr.location, "underflow in converting to 16-bit integer");
				}
			}

			return evaled_expr;
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
