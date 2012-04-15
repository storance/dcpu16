#pragma once

#include <string>
#include <memory>
#include <list>
#include <cctype>
#include <climits>

#include <boost/algorithm/string/predicate.hpp>

#include "Token.hpp"

namespace dcpu { namespace lexer {

	typedef std::shared_ptr<Token> token_type;

	template <typename Iterator, typename Container = std::list<token_type>>
	class Lexer {
	protected:
		Iterator current, end;
		std::string sourceName;
		std::uint32_t line, column;
		Container tokens;

		void skipWhitespace();
		template<typename Predicate> void skipUntil(Predicate);
		template<typename Predicate, typename Action> void processUntil(Predicate, Action);

		char nextChar();
		void moveBack();
		void nextLine();

		token_type nextToken();
		Location makeLocation();

		bool isHexDigit(char);
		bool isOperatorChar(char);
		bool isUnknownChar(char);
		bool isAllowedIdentifierChar(char);
		bool isAllowedIdentifierFirstChar(char);

		token_type parseNumber(Location, std::string value);
		token_type parseOperator(Location, std::string value);
	public:
		Lexer(Iterator current, Iterator end, std::string sourceName);

		void parse();

		Container getTokens();
	};

	#include "Lexer.cpp"
} }
