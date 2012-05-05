#include "lexer.hpp"

#include <string>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <stdexcept>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

namespace dcpu { namespace lexer {
	lexer::lexer(const string &content, const string &source)
		: current(content.begin()),
		  end(content.end()),
		  source(source),
		  line(1),
		  column(0),
		  error_handler(make_shared<dcpu::error_handler>()) {}

	lexer::lexer(const string &content, const string &source, error_handler_ptr &error_handler)
		: current(content.begin()),
		  end(content.end()),
		  source(source),
		  line(1),
		  column(0),
		  error_handler(error_handler) {}

	void lexer::parse() {
		while (true) {
			auto token = next_token();
			tokens.push_back(token);

			if (token.is_eoi()) {
				break;
			}
		}
	}

	token lexer::next_token() {
		skip();

		if (current == end) {
			auto location = make_location();
			return token(location, token_type::END_OF_INPUT, "end of file");
		}

		char c = next_char();
		auto start = make_location();

		if (c == '+' && consume_next_if('+')) {
			return token(start, token_type::INCREMENT, "++");
		} else if (c == '-' && consume_next_if('-')) {
			return token(start, token_type::DECREMENT, "--");
		} else if (c == '<' && consume_next_if('<')) {
			return token(start, token_type::SHIFT_LEFT, "<<");
		} else if (c == '>' && consume_next_if('>')) {
			return token(start, token_type::SHIFT_RIGHT, ">>");
		} else if (c == '\'') {
			string quotedString = parse_quoted_string(start, c, true);
			if (quotedString.length() == 0) {
				error_handler->warning(start, "empty character literal; assuming null terminator");

				return token(start, token_type::INTEGER, "'" + quotedString + "'", 0);
			}

			if (quotedString.length() > 1) {
				error_handler->error(start, "multi-byte character literal");
			}

			uint8_t value = quotedString[0];
			if (value & 0x80) {
				error_handler->error(start, "invalid 7-bit ASCII character");
			}

			return token(start, token_type::INTEGER, "'" + quotedString + "'", value);
		} else if (c == ':' && is_identifier_first_char(peek_char())) {
			return token(start, token_type::LABEL, append_while(next_char(), &lexer::is_identifier_char));
		} else if (c == '$' && is_identifier_first_char(peek_char())) {
			return token(start, token_type::SYMBOL, append_while(next_char(), &lexer::is_identifier_char));
		} else if (is_identifier_first_char(c)) {
			string identifier = append_while(c, &lexer::is_identifier_char);
			if (consume_next_if(':')) {
				return token(start, token_type::LABEL, identifier);
			} else {
				return token(start, token_type::IDENTIFIER, identifier);
			}
		} else if (isdigit(c)) {
			return parse_number(start, append_while(c, &lexer::is_identifier_char));
		} else if (c == '\n') {
			next_line();

			return token(start, token_type::NEWLINE, "newline");
		}

		return token(start, token_type::CHARACTER, c);
	}

	string lexer::append_while(char initial, std::function<bool (char)> predicate) {
		string content;

		if (initial > 0) content += initial;

		while (current != end) {
			char c = next_char();
			if (!predicate(c)) {
				move_back();
				break;
			}

			content += c;
		}

		return content;
	}

	string lexer::parse_quoted_string(location_ptr &location, char endQuote, bool allowEscapes) {

		string quotedString;

		while (current != end) {
			char c = next_char();
			if (c == '\n') {
				break;
			}

			if (c == endQuote) {
				return quotedString;
			}

			if (c == '\\' && allowEscapes) {
				quotedString += parse_escape_sequence();
			} else {
				quotedString += c;
			}
		}

		error_handler->error(location, format("unterminated %s literal")
			% (endQuote == '\'' ? "character" : "string"));
		return quotedString;
	}

	char lexer::parse_escape_sequence() {
		char c = next_char();
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
				char firstDigit = next_char();
				if (firstDigit == '\0') {
					return firstDigit;
				}

				if (!isxdigit(firstDigit)) {
					error_handler->error(make_location(), format("invalid hex digit '%c' following hex escape")
						% firstDigit);
					move_back();
					return '\0';
				}

				uint8_t parsed_int = parse_hex_digit(firstDigit);
				// check if it's a two digit hex escape
				if (current != end && isxdigit(*current)) {
					parsed_int = (parsed_int * 16) + parse_hex_digit(next_char());
				}

				return parsed_int;
			}
		default:
			error_handler->error(make_location(), format("unrecognized escape character '%c'") % c);
			return c;
		}
	}

	uint8_t lexer::parse_hex_digit(char c) {
		if (c >= '0' && c <= '9') {
			return c - '0';
		} else if (c >= 'a' && c <= 'f') {
			return (c - 'a') + 10;
		} else if (c > 'A' && c <= 'F') {
			return (c - 'A') + 10;
		}

		throw invalid_argument(str(format("'%c' is not a valid hex digit") % c));
	}

	location_ptr lexer::make_location() {
		return make_shared<location>(source, line, column);
	}

	char lexer::peek_char() {
		if (current == end) {
			return '\0';
		}

		return *current;
	}

	char lexer::next_char() {
		char c = peek_char();
		++current;
		++column;

		return c;
	}

	bool lexer::consume_next_if(char c) {
		if (current == end) {
			return false;
		}
		
		if (next_char() == c) {
			return true;
		}

		move_back();
		return false;
	}

	void lexer::move_back() {
		--current;
		--column;
	}

	void lexer::next_line() {
		line++;
		column = 0;
	}

	bool lexer::is_whitespace(char c) {
		return isspace(c) && c != '\n';
	}

	bool lexer::is_identifier_char(char c) {
		return isalnum(c) || c == '_' || c == '?' || c == '.' || c == '$' || c == '#' || c == '@';
	}

	bool lexer::is_identifier_first_char(char c) {
		return isalpha(c) || c == '_' || c == '?' || c == '.';
	}

	void lexer::skip() {
		bool inComment = false;

		for (; current != end; current++, column++) {
			if (!inComment && *current == ';') {
				inComment = true;
			} else if (!inComment && !is_whitespace(*current)) {
				break;
			} else if (inComment && *current == '\n') {
				break;
			}
		}
	}

	token lexer::parse_number(location_ptr &start, const string &value) {
		string unprefixedValue;

		uint8_t base = 10;
		if (istarts_with(value, "0x")) {
			base = 16;
			unprefixedValue = value.substr(2);
		} else if (istarts_with(value, "0o")) {
			base = 8;
			unprefixedValue = value.substr(2);
		} else if (istarts_with(value, "0b")) {
			base = 2;
			unprefixedValue = value.substr(2);
		} else {
			unprefixedValue = value;
		}

		// prefix without an actual value
		if (unprefixedValue.length() == 0) {
			return token(start, token_type::INVALID_INTEGER, value);
		}

		size_t pos;
		try {
			unsigned long parsedValue = stoul(unprefixedValue, &pos, base);
			if (pos != unprefixedValue.size()) {
				return token(start, token_type::INVALID_INTEGER, value);
			}

			if (parsedValue > UINT32_MAX) {
				throw out_of_range(value);
			}

			return token(start, token_type::INTEGER, value, (uint32_t)parsedValue);
		} catch (invalid_argument &ia) {
			return token(start, token_type::INVALID_INTEGER, value);
		} catch (out_of_range &oor) {
			error_handler->warning(start, format("integer '%s' overflows 32-bit intermediary storage") % value);

			return token(start, token_type::INTEGER, value, UINT32_MAX);
		}
	}
}}
