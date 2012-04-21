#pragma once

#include <string>
#include <memory>
#include <list>
#include <functional>
#include <cctype>
#include <climits>
#include <cstdlib>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "Token.hpp"

namespace dcpu { namespace lexer {
	template <typename Iterator, typename Container = std::list<std::shared_ptr<Token>>>
	class Lexer {
	protected:
		Iterator current, end;
		std::string sourceName;
		std::uint32_t line, column;

		template<typename Predicate> std::string appendWhile(char, Predicate);

		char nextChar();
		void moveBack();
		void nextLine();
		bool consumeNextCharIf(char c);

		std::shared_ptr<Token> nextToken();
		Location makeLocation();

		static bool isWhitespace(char);
		static bool isAllowedIdentifierChar(char);
		static bool isAllowedIdentifierFirstChar(char);

		void skipWhitespaceAndComments();
		std::shared_ptr<Token> parseNumber(Location, std::string value);
	public:
		Container tokens;

		Lexer(Iterator current, Iterator end, std::string sourceName);

		void parse();
	};

	#include "Lexer.cpp"
} }
