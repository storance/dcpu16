#pragma once

#include <string>
#include <list>
#include <functional>

#include "Token.hpp"

namespace dcpu { namespace lexer {
	class Lexer {
	protected:
		typedef std::string::const_iterator Iterator;

		Iterator _current, _end;
		std::string _sourceName;
		std::uint32_t _line, _column;

		std::string appendWhile(char, std::function<bool (char)>);

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
		TokenPtr parseNumber(Location, const std::string &value);
	public:
		TokenList tokens;

		Lexer(Iterator current, Iterator end, const std::string &sourceName);

		void parse();
	};
}}
