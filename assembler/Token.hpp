#pragma once

#include <ostream>
#include <string>
#include <memory>
#include <cstdint>
#include <list>

namespace dcpu { namespace lexer {
	struct Location {
		std::string source;
		std::uint32_t line;
		std::uint32_t column;

		Location();
		Location(const std::string&, std::uint32_t, std::uint32_t);

		bool operator==(const Location& other) const;
	};

	typedef std::shared_ptr<Location> location_t;
	std::ostream& operator<< (std::ostream& stream, const location_t& location);
	bool operator==(const location_t &, const location_t &);

	class Token {
	public:
		enum class Type {
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

		std::shared_ptr<Location> location;
		Type type;
		std::string content;
		std::uint32_t value;

		Token(location_t&, Type, const std::string&);
		Token(location_t&, Type, const std::string&, std::uint32_t);
		Token(location_t&, Type, char c);

		bool isInteger() const;
		bool isInvalidInteger() const;
		bool isIdentifier() const;
		bool isIncrement() const;
		bool isDecrement() const;
		bool isShiftLeft() const;
		bool isShiftRight() const;
		bool isCharacter() const;
		bool isCharacter(char c) const;
		bool isNewline() const;
		bool isEOI() const;
		bool isStatementTerminator() const;
	};

	typedef std::list<Token> token_list_t;
	typedef token_list_t::iterator token_iterator_t;

	Token& next(token_iterator_t&, token_iterator_t);
}}
