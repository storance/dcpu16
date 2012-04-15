#pragma once

#include <string>
#include <cstdint>

namespace dcpu {

	enum class TokenType {
		NUMBER,
		INVALID_NUMBER,
		OVERFLOW_NUMBER,
		IDENTIFIER,
		INCREMENT,
		DECREMENT,
		PLUS,
		MINUS,
		COMMA,
		COLON,
		DOLLAR,
		AT,
		LPAREN,
		RPAREN,
		LBRACKET,
		RBRACKET,
		NEWLINE,
		QUOTED_STRING,
		UNKNOWN,
		END_OF_INPUT
	};

	struct Location {
		std::string sourceName;
		std::uint32_t line;
		std::uint32_t column;

		Location(std::string, std::uint32_t, std::uint32_t);
	};

	class Token {
	public:
		Location location;
		TokenType type;

		Token(Location, TokenType);
	};

	class UnknownToken : public Token {
	public:
		std::string value;

		UnknownToken(Location, std::string);
	};

	class NumberToken : public Token {
	protected:
		NumberToken(Location, TokenType, std::uint32_t);
	public:
		std::uint32_t value;

		NumberToken(Location, std::uint32_t);
	};

	class OverflowNumberToken : public NumberToken {
	public:
		std::string rawValue;

		OverflowNumberToken(Location, std::string);
	};

	class InvalidNumberToken : public Token {
	public:
		std::string value;
		int base;

		InvalidNumberToken(Location, std::string, int);
	};

	class IdentifierToken : public Token {
	public:
		std::string name;

		IdentifierToken(Location, std::string);
	};
}
