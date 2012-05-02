#include "Token.hpp"

#include <cassert>

#include <boost/format.hpp>

using namespace std;

namespace dcpu { namespace lexer {

	Location::Location(const std::string &source, uint32_t line, uint32_t column)
		: source(source), line(line), column(column) {}

	Location::Location() : source(""), line(0), column(0) {}

	bool Location::operator==(const Location& other) const {
		return source == other.source && line == other.line && column == other.column;
	}

	ostream& operator<< (ostream& stream, const location_t& location) {
		return stream << boost::format("%s:%d:%d") % location->source % location->line % location->column;
	}

	bool operator==(const location_t &left, const location_t &right) {
		if (left && right) {
			return *left == *right;
		} else {
			return !left == !right;
		}
	}

	Token::Token(location_t &location, Type type, const string &content)
	    : location(location), type(type), content(content), value(0) {}

	Token::Token(location_t &location, Type type, const string &content, uint32_t value)
	    : location(location), type(type), content(content), value(value) {}

	Token::Token(location_t &location, Type type, char c)
	    : location(location), type(type), content(1, c), value(0) {}


	bool Token::isInteger() const {
		return type == Type::INTEGER;
	}

	bool Token::isInvalidInteger() const {
		return type == Type::INVALID_INTEGER;
	}

	bool Token::isIdentifier() const {
		return type == Type::IDENTIFIER;
	}

	bool Token::isIncrement() const {
		return type == Type::INCREMENT;
	}

	bool Token::isDecrement() const {
		return type == Type::DECREMENT;
	}

	bool Token::isShiftLeft() const {
		return type == Type::SHIFT_LEFT;
	}

	bool Token::isShiftRight() const {
		return type == Type::SHIFT_RIGHT;
	}

	bool Token::isCharacter() const {
		return type == Type::CHARACTER;
	}

	bool Token::isCharacter(char c) const {
		if (type == Type::CHARACTER && content.length() == 1) {
			return content[0] == c;
		}

		return false;
	}

	bool Token::isNewline() const {
		return type == Type::NEWLINE;
	}

	bool Token::isEOI() const {
		return type == Type::END_OF_INPUT;
	}

	bool Token::isStatementTerminator() const {
		return isEOI() || isNewline();
	}

	Token& next(token_iterator_t& current, token_iterator_t end) {
		if (current == end) {
			--current;
		}

		return *current++;
	}
}}
