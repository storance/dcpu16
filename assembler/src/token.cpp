#include <boost/variant.hpp>
#include <boost/format.hpp>

#include "token.hpp"

using namespace std;

namespace dcpu { namespace lexer {

	location::location(const std::string &source, uint32_t line, uint32_t column)
		: source(source), line(line), column(column) {}

	location::location() : source(""), line(0), column(0) {}

	bool location::operator==(const location& other) const {
		return source == other.source && line == other.line && column == other.column;
	}

	ostream& operator<< (ostream& stream, const location_ptr& location) {
		return stream << boost::format("%s:%d:%d") % location->source % location->line % location->column;
	}

	bool operator==(const location_ptr &left, const location_ptr &right) {
		if (left && right) {
			return *left == *right;
		} else {
			return !left == !right;
		}
	}

	token::token(location_ptr &location, token_type type, const string &content)
			: location(location), type(type), content(content), data(nullptr) {}

	token::token(location_ptr &location, token_type type, const string &content, token_data data)
			: location(location), type(type), content(content), data(data) {}

	token::token(location_ptr &location, token_type type, char c)
	    : location(location), type(type), content(1, c), data(nullptr) {}

	bool token::is_integer() const {
		return type == token_type::INTEGER;
	}

	bool token::is_invalid_integer() const {
		return type == token_type::INVALID_INTEGER;
	}

	bool token::is_label() const {
		return type == token_type::LABEL;
	}

	bool token::is_symbol() const {
		return type == token_type::SYMBOL;
	}

	bool token::is_operator(operator_type op_type) const {
		if (type == token_type::OPERATOR) {
			return get_operator_type() == op_type;
		}

		return false;
	}

	bool token::is_character(char c) const {
		if (type == token_type::CHARACTER && content.length() == 1) {
			return content[0] == c;
		}

		return false;
	}

	bool token::is_newline() const {
		return type == token_type::NEWLINE;
	}

	bool token::is_eoi() const {
		return type == token_type::END_OF_INPUT;
	}

	bool token::is_terminator() const {
		return is_eoi() || is_newline();
	}

	bool token::is_quoted_string() const {
		return type == token_type::QUOTED_STRING;
	}

	bool token::is_directive() const {
		return type == token_type::DIRECTIVE;
	}

	bool token::is_directive(directives directive) const {
		if (is_directive()) {
			return get_directive() == directive;
		}

		return false;
	}

	bool token::is_stack_operation() const {
			return type == token_type::STACK_OPERATION;
		}

	bool token::is_stack_operation(stack_operation operation) const {
		if (is_stack_operation()) {
			return get_stack_operation() == operation;
		}

		return false;
	}

	bool token::is_instruction() const {
		return type == token_type::INSTRUCTION;
	}

	bool token::is_instruction(opcodes opcode) const {
		if (is_instruction()) {
			return get_instruction().opcode == opcode;
		}

		return false;
	}

	bool token::is_register() const {
		return type == token_type::REGISTER;
	}

	bool token::is_register(registers _register) const {
		if (is_register()) {
			return get_register()._register == _register;
		}

		return false;
	}

	uint32_t token::get_integer() const {
		return boost::get<uint32_t>(data);
	}

	instruction_definition token::get_instruction() const {
		return boost::get<instruction_definition>(data);
	}

	register_definition token::get_register() const {
		return boost::get<register_definition>(data);
	}

	directives token::get_directive() const {
		return boost::get<directives>(data);
	}

	quote_type token::get_quote_type() const {
		return boost::get<quote_type>(data);
	}

	stack_operation token::get_stack_operation() const {
		return boost::get<stack_operation>(data);
	}

	symbol_type token::get_symbol_type() const {
		return boost::get<symbol_type>(data);
	}

	operator_type token::get_operator_type() const {
		return boost::get<operator_type>(data);
	}

	token& next(token_iterator& current, token_iterator end) {
		if (current == end) {
			--current;
		}

		return *current++;
	}
}}
