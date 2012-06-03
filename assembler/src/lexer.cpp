#include <string>
#include <cctype>
#include <cstdlib>
#include <climits>
#include <stdexcept>

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "lexer.hpp"

using namespace std;
using namespace boost;

namespace dcpu { namespace assembler {
	lexer::lexer(const string &content, const string &source, log &logger)
		: current(content.begin()),
		  end(content.end()),
		  source(source),
		  line(1),
		  column(0),
		  logger(logger) {}

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

		if (c == '<') {
			if (consume_next_if('<')) {
				return token(start, token_type::OPERATOR, "<<", operator_type::SHIFT_LEFT);
			} else if (consume_next_if('=')) {
				return token(start, token_type::OPERATOR, "<=", operator_type::LTE);
			} else if (consume_next_if('>')) {
				return token(start, token_type::OPERATOR, "<>", operator_type::NEQ);
			}
		} else if (c == '>') {
			if (consume_next_if('>')) {
				return token(start, token_type::OPERATOR, "<<", operator_type::SHIFT_RIGHT);
			} else if (consume_next_if('=')) {
				return token(start, token_type::OPERATOR, "<=", operator_type::GTE);
			}
		} else if (c == '=' && consume_next_if('=')) {
			return token(start, token_type::OPERATOR, "==", operator_type::EQ);
		} else if (c == '!' && consume_next_if('=')) {
			return token(start, token_type::OPERATOR, "!=", operator_type::NEQ);
		} else if (c == '&' && consume_next_if('&')) {
			return token(start, token_type::OPERATOR, "&&", operator_type::AND);
		} else if (c == '|' && consume_next_if('|')) {
			return token(start, token_type::OPERATOR, "||", operator_type::OR);
		} else if (c == '\'') {
			string quoted_string = parse_quoted_string(start, c, true);
			if (quoted_string.length() == 0) {
				logger.warning(start, "empty character literal; assuming null terminator");

				return token(start, token_type::INTEGER, quoted_string, 0);
			}

			if (quoted_string.length() > 1) {
				logger.error(start, "multi-byte character literal");
			}

			uint8_t value = quoted_string[0];
			return token(start, token_type::INTEGER, quoted_string, value);
		} else if (c == '"') {
			string quoted_string = parse_quoted_string(start, c, true);

			return token(start, token_type::QUOTED_STRING,  quoted_string, quote_type::DOUBLE_QUOTE);
		} else if (c == ':' && is_identifier_first_char(peek_char())) {
			return token(start, token_type::LABEL, append_while(next_char(), &lexer::is_identifier_char));
		} else if (c == '$' && is_identifier_first_char(peek_char())) {
			return token(start, token_type::SYMBOL, append_while(next_char(), &lexer::is_identifier_char),
					symbol_type::EXPLICIT);
		} else if (is_identifier_first_char(c)) {
			string identifier = append_while(c, &lexer::is_identifier_char);
			if (consume_next_if(':')) {
				return token(start, token_type::LABEL, identifier);
			} else {
				return parse_identifier(start, identifier);
			}
		} else if (isdigit(c)) {
			return parse_number(start, append_while(c, &lexer::is_identifier_char));
		} else if (c == '[') {
			iterator start_pos = current;
			uint32_t start_col = column;
			auto stack_op_token = parse_stack_operation(start);
			if (stack_op_token) {
				return *stack_op_token;
			} else {
				current = start_pos;
				column = start_col;
			}
		} else if (c == '\n') {
			next_line();

			return token(start, token_type::NEWLINE, "newline");
		}

		return token(start, token_type::CHARACTER, c);
	}

	token lexer::parse_identifier(location_ptr &start, const string &identifier) {
		auto _register = lookup_register(identifier);
		if (_register) {
			return token(start, token_type::REGISTER, identifier, *_register);
		}

		auto instruction = instruction_definition::lookup(identifier);
		if (instruction) {
			return token(start, token_type::INSTRUCTION, identifier, *instruction);
		}

		auto directive = lookup_directive(identifier);
		if (directive) {
			return token(start, token_type::DIRECTIVE, identifier, *directive);
		}

		auto stack_op = lookup_stack_operation(identifier);
		if (stack_op) {
			return token(start, token_type::STACK_OPERATION, identifier, *stack_op);
		}

		return token(start, token_type::SYMBOL, identifier, symbol_type::NORMAL);
	}

	boost::optional<token> lexer::parse_stack_operation(location_ptr &location) {
		string content = "[";

		while (current != end) {
			skip();
			char next = next_char();

			if (next == ']') {
				content += next;
				break;
			} else if (next == '\n') {
				break;
			} else if (next == '-' && consume_next_if('-')) {
				content += "--";
				continue;
			} else if (next == '+' && consume_next_if('+')) {
				content += "++";
				continue;
			}

			content += next;
		}

		if (algorithm::iequals(content, "[SP]")) {
			return token(location, token_type::STACK_OPERATION, content, stack_operation::PEEK);
		} else if (algorithm::iequals(content, "[--SP]")) {
			return token(location, token_type::STACK_OPERATION, content, stack_operation::PUSH);
		} else if (algorithm::iequals(content, "[SP++]")) {
			return token(location, token_type::STACK_OPERATION, content, stack_operation::POP);
		}

		return boost::none;
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

	string lexer::parse_quoted_string(location_ptr &location, char end_quote, bool allow_escapes) {

		string quoted_string;

		while (current != end) {
			char c = next_char();
			if (c == '\n') {
				break;
			}

			if (c == end_quote) {
				return quoted_string;
			}

			if (c == '\\' && allow_escapes) {
				quoted_string += parse_escape_sequence();
			} else {
				if (c & 0x80) {
					logger.warning(make_location(), boost::format("invalid 7-bit ASCII character '%c'") % c);
				}

				quoted_string += c;
			}
		}

		logger.error(location, format("unterminated %s literal")
			% (end_quote == '\'' ? "character" : "string"));
		return quoted_string;
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
					logger.error(make_location(), format("invalid hex digit '%c' following hex escape")
						% firstDigit);
					move_back();
					return '\0';
				}

				uint8_t parsed_int = parse_hex_digit(firstDigit);
				// check if it's a two digit hex escape
				if (current != end && isxdigit(*current)) {
					parsed_int = (parsed_int * 16) + parse_hex_digit(next_char());
				}

				if (parsed_int & 0x80) {
					logger.warning(make_location(), boost::format("invalid 7-bit ASCII character '%#02x'")
						% parsed_int);
				}

				return parsed_int;
			}
		default:
			logger.error(make_location(), format("unrecognized escape character '%c'") % c);
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
			unsigned long parsed_value = stoul(unprefixedValue, &pos, base);
			if (pos != unprefixedValue.size()) {
				return token(start, token_type::INVALID_INTEGER, value);
			}

			if (parsed_value > numeric_limits<uint32_t>::max()) {
				throw out_of_range(value);
			}

			return token(start, token_type::INTEGER, value, (uint32_t)parsed_value);
		} catch (invalid_argument &ia) {
			return token(start, token_type::INVALID_INTEGER, value);
		} catch (out_of_range &oor) {
			logger.warning(start, format("overflow converting '%s' to a 32-bit integer") % value);

			return token(start, token_type::INTEGER, value, numeric_limits<uint32_t>::max());
		}
	}
}}
