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

	Lexer::Lexer(const std::string &content, const std::string &source, error_handler_t &errorHandler)
		: current(content.begin()),
		  end(content.end()),
		  source(source),
		  line(1),
		  column(0),
		  errorHandler(errorHandler) {}

	void Lexer::parse() {
		while (true) {
			auto token = nextToken();
			bool isEOI = token.isEOI();

			tokens.push_back(token);
			if (isEOI) {
				break;
			}
		}
	}

	Token Lexer::nextToken() {
		skipWhitespaceAndComments();

		if (current == end) {
			auto location = makeLocation();
			return Token(location, Token::Type::END_OF_INPUT, "end of file");
		}

		char c = nextChar();
		auto start = makeLocation();

		if (c == '+' && consumeNextCharIf('+')) {
			return Token(start, Token::Type::INCREMENT, "++");
		} else if (c == '-' && consumeNextCharIf('-')) {
			return Token(start, Token::Type::DECREMENT, "--");
		} else if (c == '<' && consumeNextCharIf('<')) {
			return Token(start, Token::Type::SHIFT_LEFT, "<<");
		} else if (c == '>' && consumeNextCharIf('>')) {
			return Token(start, Token::Type::SHIFT_RIGHT, ">>");
		} else if (c == '\'') {
			string quotedString = getQuotedString(start, c, true);
			if (quotedString.length() == 0) {
				errorHandler->warning(start, "empty character literal; assuming null terminator.");

				return Token(start, Token::Type::INTEGER, "'" + quotedString + "'", 0);
			}

			if (quotedString.length() > 1) {
				errorHandler->error(start, "multi-byte character literal");
			}

			uint8_t value = quotedString[0];
			if (value & 0x80) {
				errorHandler->error(start, "invalid 7-bit ASCII character");
			}

			return Token(start, Token::Type::INTEGER, "'" + quotedString + "'", value);
		} else if (isAllowedIdentifierFirstChar(c)) {
			return Token(start, Token::Type::IDENTIFIER, appendWhile(c, &Lexer::isAllowedIdentifierChar));
		} else if (isdigit(c)) {
			return parseNumber(start, appendWhile(c, &Lexer::isAllowedIdentifierChar));
		} else if (c == '\n') {
			nextLine();

			return Token(start, Token::Type::NEWLINE, "newline");
		}

		return Token(start, Token::Type::CHARACTER, c);
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

	std::string Lexer::getQuotedString(location_t &location, char endQuote, bool allowEscapes) {

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

		throw invalid_argument(str(boost::format("'%c' is not a valid hex digit") % c)); 
	}

	location_t Lexer::makeLocation() {
		return make_shared<Location>(source, line, column);
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

	Token Lexer::parseNumber(location_t &start, const string &value) {
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
			return Token(start, Token::Type::INVALID_INTEGER, value);
		}

		size_t pos;
		try {
			unsigned long parsedValue = std::stoul(unprefixedValue, &pos, base);
			if (pos != unprefixedValue.size()) {
				return Token(start, Token::Type::INVALID_INTEGER, value);
			}

			if (parsedValue > UINT32_MAX) {
				throw out_of_range(value);
			}

			return Token(start, Token::Type::INTEGER, value, (uint32_t)parsedValue);
		} catch (invalid_argument &ia) {
			return Token(start, Token::Type::INVALID_INTEGER, value);
		} catch (out_of_range &oor) {
			errorHandler->warning(start, boost::format("integer '%s' overflows 32-bit intermediary storage") % value);

			return Token(start, Token::Type::INTEGER, value, UINT32_MAX);
		}
	}
}}
