#include "../Lexer.hpp"

#include <iostream>
#include <sstream>
#include <gtest/gtest.h>

#include <Lexer.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::lexer;

shared_ptr<Lexer> runLexer(const string &content, error_handler_t errorHandler = make_shared<ErrorHandler>()) {
	shared_ptr<Lexer> lexer = make_shared<Lexer>(content, "<Test>", errorHandler);
	lexer->parse();

	return lexer;
}

TEST(LexerTest, IdentifierStartsUnderscore) {
	shared_ptr<Lexer> lexer = runLexer("_a1_?.$#@");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("_a1_?.$#@", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, IdentifierStartsPeriod) {
	shared_ptr<Lexer> lexer = runLexer(".aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ(".aaa111", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, IdentifierStartsQuestion) {
	shared_ptr<Lexer> lexer = runLexer("?aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("?aaa111", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, IdentifierStartsLetter) {
	shared_ptr<Lexer> lexer = runLexer("aaa111");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("aaa111", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, DecimalNumber) {
	shared_ptr<Lexer> lexer = runLexer("100");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(100, it++->value);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, HexNumberLowercase) {
	shared_ptr<Lexer> lexer = runLexer("0xff");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(0xff, it++->value);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, HexNumberUppercase) {
	shared_ptr<Lexer> lexer = runLexer("0X1D");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(0x1d, it++->value);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, BinaryNumberLowercase) {
	shared_ptr<Lexer> lexer = runLexer("0b1011");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(0b1011, it++->value);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, BinaryNumberUppercase) {
	shared_ptr<Lexer> lexer = runLexer("0B10001011");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(0b10001011, it++->value);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, OctalNumberLowercase) {
	shared_ptr<Lexer> lexer = runLexer("0o32");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(032, it++->value);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, OctalNumberUppercase) {
	shared_ptr<Lexer> lexer = runLexer("0O27");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(027, it++->value);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, InvalidDecimalNumber) {
	shared_ptr<Lexer> lexer = runLexer("100a3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInvalidInteger());
	EXPECT_EQ("100a3", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, InvalidHexNumber) {
	shared_ptr<Lexer> lexer = runLexer("0X100Z3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInvalidInteger());
	EXPECT_EQ("0X100Z3", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, OctalWithInvalidNumber) {
	shared_ptr<Lexer> lexer = runLexer("0o10093");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInvalidInteger());
	EXPECT_EQ("0o10093", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, OctalWithInvalidLetter) {
	shared_ptr<Lexer> lexer = runLexer("0o100a3");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInvalidInteger());
	EXPECT_EQ("0o100a3", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, BinaryWithInvalidNumber) {
	shared_ptr<Lexer> lexer = runLexer("0b1113");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInvalidInteger());
	EXPECT_EQ("0b1113", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, BinaryWithInvalidLetter) {
	shared_ptr<Lexer> lexer = runLexer("0B111a");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInvalidInteger());
	EXPECT_EQ("0B111a", it++->content);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, OverflowNumber) {
	stringstream out;
	shared_ptr<Lexer> lexer = runLexer("4294967296", make_shared<ErrorHandler>(out));
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_TRUE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(UINT32_MAX, it++->value);

	EXPECT_TRUE(it->isEOI());

	EXPECT_EQ("<Test>:1:1: warning: integer '4294967296' overflows 32-bit intermediary storage\n", out.str());
}

TEST(LexerTest, DecimalNumberAtUint32Max) {
	shared_ptr<Lexer> lexer = runLexer("4294967295");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(UINT32_MAX, it++->value);

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, Increment) {
	shared_ptr<Lexer> lexer = runLexer("++");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isIncrement());

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, Decrement) {
	shared_ptr<Lexer> lexer = runLexer("--");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isDecrement());
	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, ShiftLeft) {
	shared_ptr<Lexer> lexer = runLexer("<<");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isShiftLeft());

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, ShiftRight) {
	shared_ptr<Lexer> lexer = runLexer(">>");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isShiftRight());

	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, Newline) {
	shared_ptr<Lexer> lexer = runLexer("\n");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isNewline());
	
	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, SingleCharacters) {
	shared_ptr<Lexer> lexer = runLexer("@");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter('@'));
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer(",");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter(','));
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("$");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter('$'));
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("[");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter('['));
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("]");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter(']'));
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("(");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter('('));
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer(")");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter(')'));
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("+");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter('+'));
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("-");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it++->isCharacter('-'));
	EXPECT_TRUE(it->isEOI());
}

TEST(LexerTest, CharacterLiterals) {
	shared_ptr<Lexer> lexer = runLexer("'a'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ('a', it++->value);	
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("'\\x4'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(0x4, it++->value);
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("'\\X42'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(0x42, it++->value);
	EXPECT_TRUE(it->isEOI());

	lexer = runLexer("'\\n'");
	ASSERT_EQ(2, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	it = lexer->tokens.begin();
	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ('\n', it++->value);
	EXPECT_TRUE(it->isEOI());
}

location_t makeLocation(uint32_t line, uint32_t column) {
	return make_shared<Location>("<Test>", line, column);
}

TEST(LexerTest, MultipleTokens) {
	shared_ptr<Lexer> lexer = runLexer("set A, b\n  set [J], 0x400\n;a test comment\nlabel: JSR label+4\n");
	ASSERT_EQ(21, lexer->tokens.size());
	EXPECT_FALSE(lexer->errorHandler->hasErrors());
	EXPECT_FALSE(lexer->errorHandler->hasWarnings());

	// Line 1
	auto it = lexer->tokens.begin();
	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("set", it->content);
	EXPECT_EQ(makeLocation(1, 1), it++->location);

	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("A", it->content);
	EXPECT_EQ(makeLocation(1, 5), it++->location);

	EXPECT_TRUE(it->isCharacter(','));
	EXPECT_EQ(makeLocation(1, 6), it++->location);

	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("b", it->content);
	EXPECT_EQ(makeLocation(1, 8), it++->location);

	EXPECT_TRUE(it->isNewline());
	EXPECT_EQ(makeLocation(1, 9), it++->location);

	// Line 2
	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("set", it->content);
	EXPECT_EQ(makeLocation(2, 3), it++->location);

	EXPECT_TRUE(it->isCharacter('['));
	EXPECT_EQ(makeLocation(2, 7), it++->location);

	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("J", it->content);
	EXPECT_EQ(makeLocation(2, 8), it++->location);

	EXPECT_TRUE(it->isCharacter(']'));
	EXPECT_EQ(makeLocation(2, 9), it++->location);

	EXPECT_TRUE(it->isCharacter(','));
	EXPECT_EQ(makeLocation(2, 10), it++->location);

	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(0x400, it->value);
	EXPECT_EQ(makeLocation(2, 12), it++->location);

	EXPECT_TRUE(it->isNewline());
	EXPECT_EQ(makeLocation(2, 17), it++->location);

	// Line 3
	EXPECT_TRUE(it->isNewline());
	EXPECT_EQ(makeLocation(3, 16), it++->location);

	// Line 4
	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("label", it->content);
	EXPECT_EQ(makeLocation(4, 1), it++->location);

	EXPECT_TRUE(it->isCharacter(':'));
	EXPECT_EQ(makeLocation(4, 6), it++->location);

	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("JSR", it->content);
	EXPECT_EQ(makeLocation(4, 8), it++->location);

	EXPECT_TRUE(it->isIdentifier());
	EXPECT_EQ("label", it->content);
	EXPECT_EQ(makeLocation(4, 12), it++->location);

	EXPECT_TRUE(it->isCharacter('+'));
	EXPECT_EQ(makeLocation(4, 17), it++->location);

	EXPECT_TRUE(it->isInteger());
	EXPECT_EQ(4, it->value);
	EXPECT_EQ(makeLocation(4, 18), it++->location);

	EXPECT_TRUE(it->isNewline());
	EXPECT_EQ(makeLocation(4, 19), it++->location);

	// Line 5
	EXPECT_TRUE(it->isEOI());
	EXPECT_EQ(makeLocation(5, 0), it->location);
}

