#pragma once

#include <string>
#include <memory>
#include <list>
#include <cctype>
#include <climits>
#include <cstdlib>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/bind.hpp>

#include "Token.hpp"

namespace dcpu { namespace lexer {

	typedef std::shared_ptr<Token> token_type;

	template <typename Iterator, typename Container = std::list<token_type>>
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

		token_type nextToken();
		Location makeLocation();

		static bool isWhitespace(char);
		static bool isAllowedIdentifierChar(char);
		static bool isAllowedIdentifierFirstChar(char);

		token_type parseNumber(Location, std::string value);
	public:
		Container tokens;

		Lexer(Iterator current, Iterator end, std::string sourceName);

		void parse();
	};

	#include "Lexer.cpp"
} }
