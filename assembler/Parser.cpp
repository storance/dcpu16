#include "Parser.hpp"

#include <iostream>

using namespace std;

namespace dcpu { namespace parser {

Parser::Parser(std::list<lexer::token_type> tokens)
	: tokens(tokens){
	current = tokens.begin();
}

void Parser::error(Location location, std::string message) {
	++error;
	cerr << location << ": error: " << message << endl;
}

void Parser::warning(Location location, std::string message) {
	++warning;
	cerr << location << ": warning: " << message << endl;
}

boost::optional<Instruction> Parser::parseInstruction() {

}

boost::optional<Label> Parser::parseLabel() {
	token_type token = nextToken();

	if (token->type == TokenType::COLON) {
		token_type nextToken = nextToken();
		if (nextToken->type == TokenType::IDENTIFIER) {
			return boost::optional(Label())
		} else {
			// error!
		}
	} else if (token->type == TokenType::IDENTIFIER) {
		token_type nextToken = nextToken();
		if (nextToken->type == TokenType::COLON) {

		}
	}
}

Statement Parser::parserStatement() {
	token_iterator start = current;
}

lexer::token_type Parser::nextToken() {
	return *current++;
}

void Parser::parse() {
	while (current != tokens.end()) {
		statements.push_back(nextStatement());
	}
}

} }
