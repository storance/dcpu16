#include "Lexer.hpp"

#include <string>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "ErrorHandler.hpp"

using namespace std;

namespace dcpu { namespace lexer {
	Lexer::Lexer(const std::string &content, const std::string &source)
		: current(content.begin()),
		  end(content.end()),
		  source(source),
		  line(1),
		  column(0),
		  errorHandler(make_shared<ErrorHandler>()) {}

	Lexer::Lexer(const std::string &content, const std::string &source, ErrorHandlerPtr &errorHandler)
		: current(content.begin()),
		  end(content.end()),
		  source(source),
		  line(1),
		  column(0),
		  errorHandler(errorHandler) {}

	void Lexer::parse() {
		while (true) {
			TokenPtr token = nextToken();
			bool isEOI = token->isEOI();

			tokens.push_back(move(token));
			if (isEOI) {
				break;
			}
		}
	}

	TokenPtr Lexer::nextToken() {
		skipWhitespaceAndComments();

		if (current == end) {
			return TokenPtr(new Token(makeLocation(), TokenType::END_OF_INPUT, "end of file"));
		}

		char c = nextChar();
		Location start = makeLocation();

		if (c == '+' && consumeNextCharIf('+')) {
			return TokenPtr(new Token(start, TokenType::INCREMENT, "++"));
		} else if (c == '-' && consumeNextCharIf('-')) {
			return TokenPtr(new Token(start, TokenType::DECREMENT, "--"));
		} else if (c == '<' && consumeNextCharIf('<')) {
			return TokenPtr(new Token(start, TokenType::SHIFT_LEFT, "<<"));
		} else if (c == '>' && consumeNextCharIf('>')) {
			return TokenPtr(new Token(start, TokenType::SHIFT_RIGHT, ">>"));
		} else if (c == '\'') {
			string quotedString = getQuotedString(start, c, true);
			if (quotedString.length() == 0) {
				errorHandler->warning(start, "empty character literal; assuming null terminator.");

				return TokenPtr(new IntegerToken(start, "'" + quotedString + "'", 0, false));
			}

			if (quotedString.length() > 1) {
				errorHandler->error(start, "character literal is too long");
			}

			uint8_t value = quotedString[0];
			if (value & 0x80) {
				errorHandler->error(start, "invalid 7-bit ASCII character");
			}

			return TokenPtr(new IntegerToken(start, "'" + quotedString + "'", value, false));
		} else if (isAllowedIdentifierFirstChar(c)) {
			return TokenPtr(new Token(start, TokenType::IDENTIFIER, appendWhile(c, &Lexer::isAllowedIdentifierChar)));
		} else if (isdigit(c)) {
			return parseNumber(start, appendWhile(c, &Lexer::isAllowedIdentifierChar));
		} else if (c == '\n') {
			nextLine();

			return TokenPtr(new Token(start, TokenType::NEWLINE, "newline"));
		}

		return TokenPtr(new Token(start, TokenType::CHARACTER, c));
	}

	string Lexer::appendWhile(char initial, function<bool (char)> predicate) {
		string content;

		if (initial > 0) content += initial;

		while (current != end) {
			char c = nextChar();
			if (!predicate(c)) {
				moveBack();
				break;
			}

			content += c;
		}

		return content;
	}

	std::string Lexer::getQuotedString(const Location &location, char endQuote, bool allowEscapes) {

		string quotedString;

		while (current != end) {
			char c = nextChar();
			if (c == '\n') {
				break;
			}

			if (c == endQuote) {
				return quotedString;
			}

			if (c == '\\' && allowEscapes) {
				quotedString += processEscapeSequence();
			} else {
				quotedString += c;
			}
		}

		errorHandler->error(location, boost::format("unterminated %s literal") 
			% (endQuote == '\'' ? "character" : "string"));
		return quotedString;
	}

	char Lexer::processEscapeSequence() {
		char c = nextChar();
		if (c == '\0') {
			return c;
		}

		switch (c) {
		case '\'':
			return '\'';
		case '"':
			return '"';
		case '?':
			return '?';
		case '\\':
			return '\\';
		case '0':
			return '\0';
		case 'a':
			return '\a';
		case 'b':
			return '\b';
		case 'f':
			return '\f';
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 't':
			return '\t';
		case 'v':
			return '\v';
		case 'x':
		case 'X':
			{
				char firstDigit = nextChar();
				if (firstDigit == '\0') {
					return firstDigit;
				}

				if (!isxdigit(firstDigit)) {
					errorHandler->error(makeLocation(), boost::format("invalid hex digit '%c' following hex escape") 
						% firstDigit);
					moveBack();
					return '\0';
				}

				uint8_t parsedInt = hexDigitToInt(firstDigit);
				// check if it's a two digit hex escape
				if (current != end && isxdigit(*current)) {
					parsedInt = (parsedInt * 16) + hexDigitToInt(nextChar());
				}

				return parsedInt;
			}
		default:
			errorHandler->error(makeLocation(), boost::format("unrecognized escape character '%c'") % c);
			return c;
		}
	}

	uint8_t Lexer::hexDigitToInt(char c) {
		if (c >= '0' && c <= '9') {
			return c - '0';
		} else if (c >= 'a' && c <= 'f') {
			return (c - 'a') + 10;
		} else if (c > 'A' && c <= 'F') {
			return (c - 'A') + 10;
		}

		throw new invalid_argument(str(boost::format("'%c' is not a valid hex digit") % c)); 
	}

	Location Lexer::makeLocation() {
		return Location(source, line, column);
	}

	char Lexer::nextChar() {
		if (current == end) {
			return '\0';
		}

		char c = *current++;
		++column;

		return c;
	}

	bool Lexer::consumeNextCharIf(char c) {
		if (current == end) {
			return false;
		}
		
		if (nextChar() == c) {
			return true;
		}

		moveBack();
		return false;
	}

	void Lexer::moveBack() {
		--current;
		--column;
	}

	void Lexer::nextLine() {
		line++;
		column = 0;
	}

	bool Lexer::isWhitespace(char c) {
		return isspace(c) && c != '\n';
	}

	bool Lexer::isAllowedIdentifierChar(char c) {
		return isalnum(c) || c == '_' || c == '?' || c == '.' || c == '$' || c == '#' || c == '@';
	}

	bool Lexer::isAllowedIdentifierFirstChar(char c) {
		return isalpha(c) || c == '_' || c == '?' || c == '.';
	}

	void Lexer::skipWhitespaceAndComments() {
		bool inComment = false;

		for (; current != end; current++, column++) {
			if (!inComment && *current == ';') {
				inComment = true;
			} else if (!inComment && !isWhitespace(*current)) {
				break;
			} else if (inComment && *current == '\n') {
				break;
			}
		}
	}

	TokenPtr Lexer::parseNumber(const Location &start, const string &value) {
		string unprefixedValue;

		uint8_t base = 10;
		if (boost::istarts_with(value, "0x")) {
			base = 16;
			unprefixedValue = value.substr(2);
		} else if (boost::istarts_with(value, "0o")) {
			base = 8;
			unprefixedValue = value.substr(2);
		} else if (boost::istarts_with(value, "0b")) {
			base = 2;
			unprefixedValue = value.substr(2);
		} else {
			unprefixedValue = value;
		}

		// prefix without an actual value
		if (unprefixedValue.length() == 0) {
			return TokenPtr(new InvalidIntegerToken(start, value, base));
		}

		size_t pos;
		try {
			unsigned long parsedValue = std::stoul(unprefixedValue, &pos, base);
			if (pos != unprefixedValue.size()) {
				return TokenPtr(new InvalidIntegerToken(start, value, base));
			}

			if (parsedValue > UINT32_MAX) {
				return TokenPtr(new IntegerToken(start, value, UINT32_MAX, true));
			}

			return TokenPtr(new IntegerToken(start, value, (uint32_t)parsedValue, false));
		} catch (invalid_argument &ia) {
			return TokenPtr(new InvalidIntegerToken(start, value, base));
		} catch (out_of_range &oor) {
			return TokenPtr(new IntegerToken(start, value, UINT32_MAX, true));
		}
	}
}}
