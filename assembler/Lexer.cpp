#include <boost/algorithm/string/predicate.hpp>

#include <cctype>
#include <climits>

template<typename Iterator>
Lexer<Iterator>::Lexer(Iterator current, Iterator end, std::string sourceName)
    : current(current), end(end), sourceName(sourceName), line(1), column(0) {

}

template<typename Iterator>
void Lexer<Iterator>::skipWhitespace() {
    skipUntil([] (char c) { return c != ' ' && c != '\t' && c != '\v' && c != '\f';});
}

template<typename Iterator> template<typename Predicate>
void Lexer<Iterator>::skipUntil(Predicate predicate) {
    processUntil(predicate, [] (char ch) -> void {});
}

template<typename Iterator> template<typename Predicate, typename Action>
void Lexer<Iterator>::processUntil(Predicate predicate, Action action) {
    while (current != end) {
        char c = nextChar();
        if (predicate(c)) {
            moveBack();
            break;
        }

        action(c);
    }
}

template<typename Iterator>
Location Lexer<Iterator>::makeLocation() {
    return Location(sourceName, line, column);
}

template<typename Iterator>
char Lexer<Iterator>::nextChar() {
    char c = *current;
    ++current;
    ++column;

    return c;
}

template<typename Iterator>
void Lexer<Iterator>::moveBack() {
    --current;
}

template<typename Iterator>
void Lexer<Iterator>::nextLine() {
    line++;
    column = 0;
}

template<typename Iterator>
bool Lexer<Iterator>::isOperatorChar(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '|' || c == '&'
		|| c == '!' || c == '^' || c == '%';
}

template<typename Iterator>
bool Lexer<Iterator>::isHexDigit(char c) {
    return isdigit(c) || tolower(c) >= 'a' && tolower(c) <= 'f';
}

template<typename Iterator>
bool Lexer<Iterator>::isAllowedIdentifierChar(char c) {
    return isalnum(c) || c == '_' || c == '?' || c == '.' || c == '$' || c == '#' || c == '@';
}

template<typename Iterator>
bool Lexer<Iterator>::isAllowedIdentifierFirstChar(char c) {
    return isalpha(c) || c == '_' || c == '?' || c == '.';
}
template<typename Iterator>
bool Lexer<Iterator>::isUnknownChar(char c) {
	return !(isAllowedIdentifierFirstChar(c) || isOperatorChar(c) || isspace(c) || c == ':' || c == ',' || c == ';'
		|| c == '$' || c == '@');
}

template<typename Iterator>
typename Lexer<Iterator>::token_type Lexer<Iterator>::parseOperator(std::string value) {
	if (value == "++") {
		return std::make_shared<Token>(makeLocation(), TokenType::INCREMENT);
	} else if (value == "--") {
		return std::make_shared<Token>(makeLocation(), TokenType::DECREMENT);
	} else if (value == "+") {
		return std::make_shared<Token>(makeLocation(), TokenType::PLUS);
	} else if (value == "-") {
		return std::make_shared<Token>(makeLocation(), TokenType::MINUS);
	} else {
		return std::make_shared<UnknownToken>(makeLocation(), value);
	}
}

template<typename Iterator>
typename Lexer<Iterator>::token_type Lexer<Iterator>::parseNumber(std::string value) {
    std::string unprefixedValue;

    int base = 10;
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
    if (unprefixedValue.size() == 0) {
        return std::make_shared<InvalidNumberToken>(makeLocation(), value, base);
    }

    char *end;
    unsigned long l = strtoul(unprefixedValue.c_str(), &end, base);

    if ((errno == ERANGE && l == ULONG_MAX) || l > 0xffffffff) {
        return std::make_shared<OverflowNumberToken>(makeLocation(), value);
    } else if (*end != '\0') {
        return std::make_shared<InvalidNumberToken>(makeLocation(), value, base);
    }

    return std::make_shared<NumberToken>(makeLocation(), (std::uint32_t)l);
}

template<typename Iterator>
typename Lexer<Iterator>::token_type Lexer<Iterator>::nextToken() {
    skipWhitespace();

    if (current == end) {
        return std::make_shared<Token>(makeLocation(), TokenType::END_OF_INPUT);
    }
    char c = nextChar();

    if (c == ';') {
        skipUntil([](char ch) { return ch == '\n' || ch == '\r'; });

        return nextToken();
    } else if (c == ':') {
        return std::make_shared<Token>(makeLocation(), TokenType::COLON);
    } else if (c == ',') {
        return std::make_shared<Token>(makeLocation(), TokenType::COMMA);
    } else if (c == '$') {
        return std::make_shared<Token>(makeLocation(), TokenType::DOLLAR);
    } else if (c == '[') {
        return std::make_shared<Token>(makeLocation(), TokenType::LBRACKET);
    } else if (c == ']') {
        return std::make_shared<Token>(makeLocation(), TokenType::RBRACKET);
    } else if (c == '@') {
		return std::make_shared<Token>(makeLocation(), TokenType::AT);
    } else if (c == '(') {
        return std::make_shared<Token>(makeLocation(), TokenType::LPAREN);
    } else if (c == ')') {
        return std::make_shared<Token>(makeLocation(), TokenType::RPAREN);
    } else if (isOperatorChar(c)) {
    	std::string value(1, c);
        processUntil([this](char ch) { return !this->isOperatorChar(ch);}, [&] (char ch) { value += ch; });

        return parseOperator(value);
    } else if (c == '-') {
        char next = nextChar();
        if (next == '-') {
            return std::make_shared<Token>(makeLocation(), TokenType::DECREMENT);
        } else {
            moveBack();
            return std::make_shared<Token>(makeLocation(), TokenType::MINUS);
        }
    } else if (isAllowedIdentifierFirstChar(c)) {
        std::string name(1, c);

        processUntil([this](char ch) { return !this->isAllowedIdentifierChar(ch);}, [&] (char ch) { name += ch; });

        return std::make_shared<IdentifierToken>(makeLocation(), name);
    } else if (isdigit(c)) {
        std::string value(1, c);
        processUntil([this](char ch) { return !this->isAllowedIdentifierChar(ch); }, [&] (char ch) { value += ch; });

        return parseNumber(value);
    } else if (c == '\r') {
        if (nextChar() == '\n') {
            nextLine();

            return std::make_shared<Token>(makeLocation(), TokenType::NEWLINE);
        } else {
            moveBack();

            return std::make_shared<UnknownToken>(makeLocation(), std::string(1, c));
        }

    } else if (c == '\n') {
        nextLine();

        return std::make_shared<Token>(makeLocation(), TokenType::NEWLINE);
    } else {
		std::string value(1, c);

		processUntil([this](char ch) { return !this->isUnknownChar(ch); }, [&] (char ch) { value += ch; });

		return std::make_shared<UnknownToken>(makeLocation(), value);
    }
}
template<typename Iterator>
std::vector<typename Lexer<Iterator>::token_type> Lexer<Iterator>::parse() {
    std::vector<token_type> tokens;

    while (true) {
        token_type token = nextToken();
        tokens.push_back(token);

        if (token->type == TokenType::END_OF_INPUT) {
            break;
        }
    }

    return tokens;
}
