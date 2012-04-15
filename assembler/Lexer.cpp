template<typename Iterator, typename Container>
Lexer<Iterator, Container>::Lexer(Iterator current, Iterator end, std::string sourceName)
    : current(current), end(end), sourceName(sourceName), line(1), column(0) {

}

template<typename Iterator, typename Container> template<typename Predicate>
std::string Lexer<Iterator, Container>::appendWhile(char initial, Predicate predicate) {
	std::string content;

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

template<typename Iterator, typename Container>
Location Lexer<Iterator, Container>::makeLocation() {
    return Location(sourceName, line, column);
}

template<typename Iterator, typename Container>
char Lexer<Iterator, Container>::nextChar() {
    char c = *current;
    ++current;
    ++column;

    return c;
}

template<typename Iterator, typename Container>
bool Lexer<Iterator, Container>::consumeNextCharIf(char c) {
	if (nextChar() == c) {
		return true;
	}

	moveBack();
	return false;
}

template<typename Iterator, typename Container>
void Lexer<Iterator, Container>::moveBack() {
    --current;
    --column;
}

template<typename Iterator, typename Container>
void Lexer<Iterator, Container>::nextLine() {
    line++;
    column = 0;
}

template<typename Iterator, typename Container>
bool Lexer<Iterator, Container>::isWhitespace(char c) {
	return std::isspace(c) && c != '\n';
}

template<typename Iterator, typename Container>
bool Lexer<Iterator, Container>::isAllowedIdentifierChar(char c) {
    return std::isalnum(c) || c == '_' || c == '?' || c == '.' || c == '$' || c == '#' || c == '@';
}

template<typename Iterator, typename Container>
bool Lexer<Iterator, Container>::isAllowedIdentifierFirstChar(char c) {
    return std::isalpha(c) || c == '_' || c == '?' || c == '.';
}

template<typename Iterator, typename Container>
token_type Lexer<Iterator, Container>::parseNumber(Location start, std::string value) {
    std::string unprefixedValue;

    std::uint8_t base = 10;
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
        return std::make_shared<InvalidIntegerToken>(start, value, base);
    }

    char *end;
    unsigned long parsedValue = std::strtoul(unprefixedValue.c_str(), &end, base);

    if ((errno == ERANGE && parsedValue == ULONG_MAX) || parsedValue > UINT32_MAX) {
        return std::make_shared<IntegerToken>(start, value, UINT32_MAX, true);
    } else if (*end != '\0') {
        return std::make_shared<InvalidIntegerToken>(start, value, base);
    }

    return std::make_shared<IntegerToken>(start, value, (std::uint32_t)parsedValue, false);
}

template<typename Iterator, typename Container>
token_type Lexer<Iterator, Container>::nextToken() {
    if (current == end) {
        return std::make_shared<Token>(makeLocation(), dcpu::TokenType::END_OF_INPUT, '\0');
    }

    char c = nextChar();
    Location start = makeLocation();

	if (isWhitespace(c)) {
		return std::make_shared<Token>(start, dcpu::TokenType::WHITESPACE,
			appendWhile(c, &Lexer<Iterator, Container>::isWhitespace));
    } else if (c == ';') {
        return std::make_shared<Token>(start, dcpu::TokenType::COMMENT,
			appendWhile(c, [](char ch) { return ch != '\n'; }));
    } else if (c == '+') {
		if (consumeNextCharIf('+')) {
			return std::make_shared<Token>(start, dcpu::TokenType::INCREMENT, "++");
		}
    } else if (c == '-') {
		if (consumeNextCharIf('-')) {
			return std::make_shared<Token>(start, dcpu::TokenType::DECREMENT, "--");
		}
    } else if (c == '<') {
		if (consumeNextCharIf('<')) {
			return std::make_shared<Token>(start, dcpu::TokenType::SHIFT_LEFT, "<<");
		}
    } else if (c == '>') {
		if (consumeNextCharIf('>')) {
			return std::make_shared<Token>(start, dcpu::TokenType::SHIFT_RIGHT, ">>");
		}
    } else if (isAllowedIdentifierFirstChar(c)) {
        return std::make_shared<Token>(start, dcpu::TokenType::IDENTIFIER,
			appendWhile(c, &Lexer<Iterator, Container>::isAllowedIdentifierChar));
    } else if (std::isdigit(c)) {
        return parseNumber(start, appendWhile(c, &Lexer<Iterator, Container>::isAllowedIdentifierChar));
    } else if (c == '\n') {
        nextLine();

        return std::make_shared<Token>(start, dcpu::TokenType::NEWLINE, '\n');
    }

    return std::make_shared<Token>(start, dcpu::TokenType::CHARACTER, c);
}
template<typename Iterator, typename Container>
void Lexer<Iterator, Container>::parse() {
    while (true) {
        token_type token = nextToken();
        tokens.push_back(token);

        if (token->type == dcpu::TokenType::END_OF_INPUT) {
            break;
        }
    }
}
