#include "Utils.hpp"

#include <gtest/gtest.h>

using namespace std;;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::lexer;
using namespace dcpu::parser;

void runLexer(const string &content, int expectedTokens, shared_ptr<Lexer> &lex) {
	lex = make_shared<Lexer>(content.begin(), content.end(), "<Test>");
    lex->parse();

    if (expectedTokens > -1) {
    	ASSERT_EQ(expectedTokens, lex->tokens.size());
    }
}

void runParser(const string &content, int expectedStatements, shared_ptr<Parser> &parser) {
	shared_ptr<Lexer> lexer;
	runLexer(content, -1, lexer);

	runParser(lexer, expectedStatements, parser);
}

void runParser(shared_ptr<Lexer> &lexer, int expectedStatements, shared_ptr<Parser> &parser) {
	parser = make_shared<Parser>(*lexer);
	parser->parse();

	if (expectedStatements > -1) {
    	ASSERT_EQ(expectedStatements, parser->statements.size());
    }
}

void runExpressionParser(const string &content, ExpressionPtr &expr, bool insideIndirect, bool allowRegisters) {

	shared_ptr<Lexer> lexer;
	runLexer(content, -1, lexer);

	ErrorHandlerPtr errorHandler = make_shared<ErrorHandler>();

	auto begin = lexer->tokens.begin();
	ExpressionParser parser(begin, lexer->tokens.end(), errorHandler, allowRegisters, true, insideIndirect);
	expr = move(parser.parse());
}