#include "token.hpp"

#include <cassert>

#include <boost/format.hpp>

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
	    : location(location), type(type), content(content), value(0) {}

	token::token(location_ptr &location, token_type type, const string &content, uint32_t value)
	    : location(location), type(type), content(content), value(value) {}

	token::token(location_ptr &location, token_type type, char c)
	    : location(location), type(type), content(1, c), value(0) {}

	bool token::is_integer() const {
		return type == token_type::INTEGER;
	}

	bool token::is_invalid_integer() const {
		return type == token_type::INVALID_INTEGER;
	}

	bool token::is_identifier() const {
		return type == token_type::IDENTIFIER;
	}

	bool token::is_increment() const {
		return type == token_type::INCREMENT;
	}

	bool token::is_decrement() const {
		return type == token_type::DECREMENT;
	}

	bool token::is_shift_left() const {
		return type == token_type::SHIFT_LEFT;
	}

	bool token::is_shift_right() const {
		return type == token_type::SHIFT_RIGHT;
	}

	bool token::is_character() const {
		return type == token_type::CHARACTER;
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

	token& next(token_iterator& current, token_iterator end) {
		if (current == end) {
			--current;
		}

		return *current++;
	}
}}
