#pragma once

#include <string>
#include <memory>
#include <cstdint>

namespace dcpu {

	enum class TokenType {
		IDENTIFIER,
		INTEGER,
		INVALID_INTEGER,
		INCREMENT,
		DECREMENT,
		SHIFT_LEFT,
		SHIFT_RIGHT,
		CHARACTER,
		COMMENT,
		WHITESPACE,
		NEWLINE,
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
		std::string content;

		Token(Location, TokenType, std::string);
		Token(Location, TokenType, char c);

		bool isInteger() const;
		bool isInvalidInteger() const;
		bool isIdentifier() const;
		bool isIncrement() const;
		bool isDecrement() const;
		bool isShiftLeft() const;
		bool isShiftRight() const;
		bool isCharacter(char c) const;
		bool isWhitespace() const;
		bool isNewline() const;
		bool isEOI() const;
		bool isComment() const;
	};

	class IntegerToken : public Token {
	public:
		std::uint32_t value;
		bool overflow;

		IntegerToken(Location, std::string, std::uint32_t, bool overflow);
	};

	class InvalidIntegerToken : public Token {
	public:
		std::string value;
		std::uint8_t base;

		InvalidIntegerToken(Location, std::string, std::uint8_t);
	};


	std::shared_ptr<IntegerToken> asInteger(std::shared_ptr<Token> token);
	std::shared_ptr<InvalidIntegerToken> asInvalidInteger(std::shared_ptr<Token> token);
}
