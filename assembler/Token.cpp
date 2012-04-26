#include "Token.hpp"

#include <cassert>

using namespace std;

namespace dcpu { namespace lexer {

Location::Location(const std::string &sourceName, uint32_t line, uint32_t column)
    : sourceName(sourceName), line(line), column(column) {}

ostream& operator<< (ostream& stream, const Location& location) {
	stream << location.sourceName << ":" << location.line << ":" << location.column;
	return stream;
}

Token::Token(const Location &location, TokenType type, const string &content)
    : location(location), type(type), content(content) {}

Token::Token(const Location &location, TokenType type, char c)
    : location(location), type(type), content(1, c) {}

Token::~Token() {}

bool Token::isInteger() const {
	return type == TokenType::INTEGER;
}

bool Token::isInvalidInteger() const {
	return type == TokenType::INVALID_INTEGER;
}

bool Token::isIdentifier() const {
	return type == TokenType::IDENTIFIER;
}

bool Token::isIncrement() const {
	return type == TokenType::INCREMENT;
}

bool Token::isDecrement() const {
	return type == TokenType::DECREMENT;
}

bool Token::isShiftLeft() const {
	return type == TokenType::SHIFT_LEFT;
}

bool Token::isShiftRight() const {
	return type == TokenType::SHIFT_RIGHT;
}

bool Token::isCharacter(char c) const {
	if (type == TokenType::CHARACTER && content.length() == 1) {
		return content[0] == c;
	}

	return false;
}

bool Token::isNewline() const {
	return type == TokenType::NEWLINE;
}

bool Token::isEOI() const {
	return type == TokenType::END_OF_INPUT;
}

bool Token::isStatementTerminator() const {
	return isEOI() || isNewline();
}

IntegerToken::IntegerToken(const Location &location, const std::string &content, uint32_t value, bool overflow)
    : Token(location, TokenType::INTEGER, content), value(value), overflow(overflow) {}

InvalidIntegerToken::InvalidIntegerToken(const Location &location, const string &content, uint8_t base)
    : Token(location, TokenType::INVALID_INTEGER, content), base(base) {}

IntegerToken* asInteger(TokenPtr &token) {
	assert(token->isInteger());

	return dynamic_cast<IntegerToken*>(token.get());
}

InvalidIntegerToken* asInvalidInteger(TokenPtr &token) {
	assert(token->isInvalidInteger());

	return dynamic_cast<InvalidIntegerToken*>(token.get());
}

TokenPtr& next(TokenList::iterator& current, TokenList::iterator end) {
	if (current == end) {
		--current;
	}

	return *current++;
}

}}
