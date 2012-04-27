#pragma once

#include <ostream>
#include <string>
#include <memory>
#include <cstdint>
#include <list>

namespace dcpu { namespace lexer {

	enum class TokenType {
		IDENTIFIER,
		INTEGER,
		INVALID_INTEGER,
		INCREMENT,
		DECREMENT,
		SHIFT_LEFT,
		SHIFT_RIGHT,
		CHARACTER,
		NEWLINE,
		END_OF_INPUT
	};

	struct Location {
		std::string sourceName;
		std::uint32_t line;
		std::uint32_t column;

		Location(const std::string&, std::uint32_t, std::uint32_t);
	};

	std::ostream& operator<< (std::ostream& stream, const Location& location);
	std::string str(const Location& location);

	class Token {
	public:
		Location location;
		TokenType type;
		std::string content;

		Token(const Location&, TokenType, const std::string&);
		Token(const Location&, TokenType, char c);
		virtual ~Token();

		bool isInteger() const;
		bool isInvalidInteger() const;
		bool isIdentifier() const;
		bool isIncrement() const;
		bool isDecrement() const;
		bool isShiftLeft() const;
		bool isShiftRight() const;
		bool isCharacter(char c) const;
		bool isNewline() const;
		bool isEOI() const;
		bool isStatementTerminator() const;
	};

	class IntegerToken : public Token {
	public:
		std::uint32_t value;
		bool overflow;

		IntegerToken(const Location&, const std::string&, std::uint32_t, bool overflow);
	};

	class InvalidIntegerToken : public Token {
	public:
		std::string value;
		std::uint8_t base;

		InvalidIntegerToken(const Location&, const std::string&, std::uint8_t);
	};


	typedef std::unique_ptr<Token> TokenPtr;
	typedef std::list<TokenPtr> TokenList;

	IntegerToken* asInteger(TokenPtr& token);
	InvalidIntegerToken* asInvalidInteger(TokenPtr& token);

	TokenPtr& next(TokenList::iterator&, TokenList::iterator);
}}
