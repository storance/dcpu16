#pragma once

#include <ostream>
#include <string>
#include <memory>
#include <cstdint>
#include <list>

namespace dcpu { namespace lexer {
	struct location {
		std::string source;
		std::uint32_t line;
		std::uint32_t column;

		location();
		location(const std::string&, std::uint32_t, std::uint32_t);

		bool operator==(const location& other) const;
	};

	typedef std::shared_ptr<location> location_ptr;
	std::ostream& operator<< (std::ostream& stream, const location_ptr& location);
	bool operator==(const location_ptr &, const location_ptr &);

	enum class token_type {
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

	class token {
	public:


		location_ptr location;
		token_type type;
		std::string content;
		std::uint32_t value;

		token(location_ptr&, token_type, const std::string&);
		token(location_ptr&, token_type, const std::string&, std::uint32_t);
		token(location_ptr&, token_type, char c);

		bool is_integer() const;
		bool is_invalid_integer() const;
		bool is_identifier() const;
		bool is_increment() const;
		bool is_decrement() const;
		bool is_shift_left() const;
		bool is_shift_right() const;
		bool is_character(char c) const;
		bool is_newline() const;
		bool is_eoi() const;
		bool is_terminator() const;
	};

	typedef std::list<token> token_list;
	typedef token_list::iterator token_iterator;

	token& next(token_iterator&, token_iterator);
}}
