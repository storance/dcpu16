#include "Token.hpp"

#include <cassert>

using namespace std;

namespace dcpu {

Location::Location(std::string sourceName, uint32_t line, uint32_t column)
    : sourceName(sourceName), line(line), column(column) {}

Token::Token(Location location, TokenType type, string content)
    : location(location), type(type), content(content) {}

Token::Token(Location location, TokenType type, char c)
    : location(location), type(type), content(1, c) {}

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

bool Token::isWhitespace() const {
	return type == TokenType::WHITESPACE;
}

bool Token::isNewline() const {
	return type == TokenType::NEWLINE;
}

bool Token::isEOI() const {
	return type == TokenType::END_OF_INPUT;
}

bool Token::isComment() const {
	return type == TokenType::COMMENT;
}

IntegerToken::IntegerToken(Location location, std::string content, uint32_t value, bool overflow)
    : Token(location, TokenType::INTEGER, content), value(value), overflow(overflow) {}

InvalidIntegerToken::InvalidIntegerToken(Location location, string content, uint8_t base)
    : Token(location, TokenType::INVALID_INTEGER, content), base(base) {}

shared_ptr<IntegerToken> asInteger(shared_ptr<Token> token) {
	assert(token->isInteger());
	return shared_ptr<IntegerToken>(token, (IntegerToken*)token.get());
}

shared_ptr<InvalidIntegerToken> asInvalidInteger(shared_ptr<Token> token) {
	assert(token->isInvalidInteger());

	return shared_ptr<InvalidIntegerToken>(token, (InvalidIntegerToken*)token.get());
}

}
