#pragma once

#include <string>
#include <list>
#include <functional>

#include "token.hpp"
#include "log.hpp"

namespace dcpu { namespace assembler {
	class lexer {
	protected:
		typedef std::string::const_iterator iterator;

		iterator current, end;
		std::string source;
		uint32_t line, column;

		std::string append_while(char, std::function<bool (char)>);
		std::string parse_quoted_string(location_ptr &location, char end_quote, bool allow_escapes);
		char parse_escape_sequence();
		uint8_t parse_hex_digit(char c);
		boost::optional<token> parse_stack_operation(location_ptr &location);

		char next_char();
		char peek_char();
		void move_back();
		void next_line();
		bool consume_next_if(char c);

		token next_token();
		location_ptr make_location();

		static bool is_whitespace(char);
		static bool is_identifier_char(char);
		static bool is_identifier_first_char(char);

		void skip();
		token parse_number(location_ptr&, const std::string&);
		token parse_identifier(location_ptr&, const std::string&);
	public:
		log &logger;
		token_list tokens;

		lexer(const std::string &content, const std::string &source, log &logger);

		void parse();
	};
}}
