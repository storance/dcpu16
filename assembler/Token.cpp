#include "Token.hpp"

using namespace std;

Location::Location(std::string sourceName, uint32_t line, uint32_t column)
    : sourceName(sourceName), line(line), column(column) {}

Token::Token(Location location, TokenType type)
    : location(location), type(type) {}

UnknownToken::UnknownToken(Location location, std::string value)
    : Token(location, TokenType::UNKNOWN), value(value) {}

NumberToken::NumberToken(Location location, TokenType type, uint32_t value)
    : Token(location, type), value(value) {}

NumberToken::NumberToken(Location location, uint32_t value)
    : Token(location, TokenType::NUMBER), value(value) {}

OverflowNumberToken::OverflowNumberToken(Location location, std::string rawValue)
    : NumberToken(location, TokenType::OVERFLOW_NUMBER, 0xffffffff), rawValue(rawValue) {}

InvalidNumberToken::InvalidNumberToken(Location location, string value, int base)
    : Token(location, TokenType::INVALID_NUMBER), value(value), base(base) {}

IdentifierToken::IdentifierToken(Location location, std::string name)
    : Token(location, TokenType::IDENTIFIER), name(name) {}

