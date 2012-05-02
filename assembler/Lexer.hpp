#pragma once

#include <string>
#include <list>
#include <functional>

#include "Token.hpp"
#include "ErrorHandler.hpp"

namespace dcpu { namespace lexer {
	class Lexer {
	protected:
		typedef std::string::const_iterator iterator;

		iterator current, end;
		std::string source;
		std::uint32_t line, column;

		std::string appendWhile(char, std::function<bool (char)>);
		std::string getQuotedString(location_t &location, char endQuote, bool allowEscapes);
		char processEscapeSequence();
		std::uint8_t hexDigitToInt(char c);

		char nextChar();
		void moveBack();
		void nextLine();
		bool consumeNextCharIf(char c);

		Token nextToken();
		location_t makeLocation();

		static bool isWhitespace(char);
		static bool isAllowedIdentifierChar(char);
		static bool isAllowedIdentifierFirstChar(char);

		void skipWhitespaceAndComments();
		Token parseNumber(location_t&, const std::string &value);
	public:
		error_handler_t errorHandler;
		token_list_t tokens;

		Lexer(const std::string &content, const std::string &source);
		Lexer(const std::string &content, const std::string &source, error_handler_t &errorHandler);

		void parse();
	};
}}
