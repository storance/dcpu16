#pragma once

#include <string>
#include <list>
#include <functional>

#include "token.hpp"
#include "error_handler.hpp"

namespace dcpu { namespace lexer {
	class lexer {
	protected:
		typedef std::string::const_iterator iterator;

		iterator current, end;
		std::string source;
		std::uint32_t line, column;

		std::string append_while(char, std::function<bool (char)>);
		std::string parse_quoted_string(location_ptr &location, char end_quote, bool allow_escapes);
		char parse_escape_sequence();
		std::uint8_t parse_hex_digit(char c);

		char next_char();
		void move_back();
		void next_line();
		bool consume_next_if(char c);

		token next_token();
		location_ptr make_location();

		static bool is_whitespace(char);
		static bool is_identifier_char(char);
		static bool is_identifier_first_char(char);

		void skip();
		token parse_number(location_ptr&, const std::string &value);
	public:
		error_handler_ptr error_handler;
		token_list tokens;

		lexer(const std::string &content, const std::string &source);
		lexer(const std::string &content, const std::string &source, error_handler_ptr &error_handler);

		void parse();
	};
}}
