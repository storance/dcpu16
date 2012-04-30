#pragma once

#include <string>
#include <list>
#include <functional>

#include "Token.hpp"
#include "Types.hpp"

namespace dcpu { namespace lexer {
	class Lexer {
	protected:
		typedef std::string::const_iterator Iterator;

		Iterator current, end;
		std::string source;
		std::uint32_t line, column;

		std::string appendWhile(char, std::function<bool (char)>);
		std::string getQuotedString(const Location &location, char endQuote, bool allowEscapes);
		char processEscapeSequence();
		std::uint8_t hexDigitToInt(char c);

		char nextChar();
		void moveBack();
		void nextLine();
		bool consumeNextCharIf(char c);

		TokenPtr nextToken();
		Location makeLocation();

		static bool isWhitespace(char);
		static bool isAllowedIdentifierChar(char);
		static bool isAllowedIdentifierFirstChar(char);

		void skipWhitespaceAndComments();
		TokenPtr parseNumber(const Location&, const std::string &value);
	public:
		ErrorHandlerPtr errorHandler;
		TokenList tokens;

		Lexer(const std::string &content, const std::string &source);
		Lexer(const std::string &content, const std::string &source, ErrorHandlerPtr &errorHandler);

		void parse();
	};
}}
