#include "../Lexer.hpp"

#include <iostream>
#include <gtest/gtest.h>

#include "Utils.hpp"

using namespace std;
using namespace dcpu;
using namespace dcpu::lexer;

void assertInvalidInteger(TokenPtr &token, const std::string &expectedContent, uint8_t expectedBase) {
	ASSERT_TRUE(token->isInvalidInteger());

	InvalidIntegerToken* integerToken = asInvalidInteger(token);
	EXPECT_EQ(expectedContent, integerToken->content);
	EXPECT_EQ(expectedBase, integerToken->base);
}

void assertInteger(TokenPtr &token, uint32_t expectedValue, bool expectedOverflow) {
	ASSERT_TRUE(token->isInteger());

	IntegerToken *integerToken = asInteger(token);
	EXPECT_EQ(expectedValue, integerToken->value); 
	if (expectedOverflow)  {
		EXPECT_TRUE(integerToken->overflow);
	} else {
		EXPECT_FALSE(integerToken->overflow);
	}
}

void assertLocation(TokenPtr &token, uint32_t expectedLine, uint32_t expectedColumn) {
	EXPECT_EQ("<Test>", token->location.sourceName);
	EXPECT_EQ(expectedLine, token->location.line);
	EXPECT_EQ(expectedColumn, token->location.column);
}

void assertIdentifier(TokenPtr &token, const string &expectedContent) {
	ASSERT_TRUE(token->isIdentifier());
	EXPECT_EQ(expectedContent, token->content);
}

void assertIncrement(TokenPtr &token) {
	ASSERT_TRUE(token->isIncrement());
	EXPECT_EQ("++", token->content);
}

void assertDecrement(TokenPtr &token) {
	ASSERT_TRUE(token->isDecrement());
	EXPECT_EQ("--", token->content);
}

void assertShiftLeft(TokenPtr &token) {
	ASSERT_TRUE(token->isShiftLeft());
	EXPECT_EQ("<<", token->content);
}

void assertShiftRight(TokenPtr &token) {
	ASSERT_TRUE(token->isShiftRight());
	EXPECT_EQ(">>", token->content);
}

void assertCharacter(TokenPtr &token, char c) {
	ASSERT_TRUE(token->isCharacter(c));
}

void assertNewline(TokenPtr &token) {
	ASSERT_TRUE(token->isNewline());
	EXPECT_EQ("newline", token->content);
}

void assertEOI(TokenPtr &token) {
	ASSERT_TRUE(token->isEOI());
	EXPECT_EQ("end of file", token->content);
}

TEST(LexerTest, IdentifierStartsUnderscore) {
    shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("_a1_?.$#@", 2, lexer));

	auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertIdentifier(*it++, "_a1_?.$#@");
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, IdentifierStartsPeriod) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer(".aaa111", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertIdentifier(*it++, ".aaa111");
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, IdentifierStartsQuestion) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("?aaa111", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertIdentifier(*it++, "?aaa111");
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, IdentifierStartsLetter) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("aaa111", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertIdentifier(*it++, "aaa111");
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, DecimalNumber) {
    shared_ptr<Lexer> lexer;

    ASSERT_NO_FATAL_FAILURE(runLexer("100", 2, lexer));

	auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, 100, false);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, HexNumberLowercase) {
    shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0xff", 2, lexer));

	auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, 0xff, false);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, HexNumberUppercase) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0X1D", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, 0x1d, false);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, BinaryNumberLowercase) {
    shared_ptr<Lexer> lexer;

    ASSERT_NO_FATAL_FAILURE(runLexer("0b1011", 2, lexer));

	auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, 0b1011, false);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, BinaryNumberUppercase) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0B10001011", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, 0b10001011, false);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, OctalNumberLowercase) {
    shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0o32", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, 032, false);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, OctalNumberUppercase) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0O27", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, 027, false);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, InvalidDecimalNumber) {
    shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("100a3", 2, lexer));

	auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInvalidInteger(*it++, "100a3", 10);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, InvalidHexNumber) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0X100Z3", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInvalidInteger(*it++, "0X100Z3", 16);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, OctalWithInvalidNumber) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0o10093", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInvalidInteger(*it++, "0o10093", 8);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, OctalWithInvalidLetter) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0o100a3", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInvalidInteger(*it++, "0o100a3", 8);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, BinaryWithInvalidNumber) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0b1113", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInvalidInteger(*it++, "0b1113", 2);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, BinaryWithInvalidLetter) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("0B111a", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInvalidInteger(*it++, "0B111a", 2);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, OverflowNumber) {
    shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("4294967296", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, UINT32_MAX, true);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, DecimalNumberAtUint32Max) {
    shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("4294967295", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertInteger(*it++, UINT32_MAX, false);
	}

    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, Increment) {
    shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("++", 2, lexer));

	auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertIncrement(*it++);
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, Decrement) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("--", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertDecrement(*it++);
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, ShiftLeft) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("<<", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertShiftLeft(*it++);
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, ShiftRight) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer(">>", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertShiftRight(*it++);
	}

	{
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, Newline) {
	shared_ptr<Lexer> lexer;
	ASSERT_NO_FATAL_FAILURE(runLexer("\n", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertNewline(*it++);
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}

TEST(LexerTest, SingleCharacters) {
	shared_ptr<Lexer> lexer;
    ASSERT_NO_FATAL_FAILURE(runLexer("@", 2, lexer));

    auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, '@');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}

    ASSERT_NO_FATAL_FAILURE(runLexer(",", 2, lexer));

    it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, ',');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}

	ASSERT_NO_FATAL_FAILURE(runLexer("$", 2, lexer));

    it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, '$');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}

    ASSERT_NO_FATAL_FAILURE(runLexer("[", 2, lexer));

    it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, '[');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}

    ASSERT_NO_FATAL_FAILURE(runLexer("]", 2, lexer));

    it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, ']');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}

    ASSERT_NO_FATAL_FAILURE(runLexer("(", 2, lexer));

    it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, '(');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}

    ASSERT_NO_FATAL_FAILURE(runLexer(")", 2, lexer));

    it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, ')');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}

    ASSERT_NO_FATAL_FAILURE(runLexer("+", 2, lexer));

    it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, '+');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}

    ASSERT_NO_FATAL_FAILURE(runLexer("-", 2, lexer));

    it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertCharacter(*it++, '-');
	}
    
    {
		SCOPED_TRACE("Token: 2");
		assertEOI(*it++);
	}
}


TEST(LexerTest, MultipleTokens) {
	shared_ptr<Lexer> lexer;
	ASSERT_NO_FATAL_FAILURE(runLexer("set A, b\n  set [J], 0x400\n;a test comment\nlabel: JSR label+4\n", 21, lexer));

	// Line 1
	auto it = lexer->tokens.begin();
	{
		SCOPED_TRACE("Token: 1");
		assertIdentifier(*it, "set");
		assertLocation(*it++, 1, 1);
	}

	{
		SCOPED_TRACE("Token: 2");
		assertIdentifier(*it, "A");
		assertLocation(*it++, 1, 5);
	}

	{
		SCOPED_TRACE("Token: 3");
		assertCharacter(*it, ',');
		assertLocation(*it++, 1, 6);
	}

	{
		SCOPED_TRACE("Token: 4");
		assertIdentifier(*it, "b");
		assertLocation(*it++, 1, 8);
	}

	{
		SCOPED_TRACE("Token: 5");
		assertNewline(*it);
		assertLocation(*it++, 1, 9);
	}

	// Line 2
	{
		SCOPED_TRACE("Token: 6");
		assertIdentifier(*it, "set");
		assertLocation(*it++, 2, 3);
	}

	{
		SCOPED_TRACE("Token: 7");
		assertCharacter(*it, '[');
		assertLocation(*it++, 2, 7);
	}

	{
		SCOPED_TRACE("Token: 8");
		assertIdentifier(*it, "J");
		assertLocation(*it++, 2, 8);
	}

	{
		SCOPED_TRACE("Token: 9");
		assertCharacter(*it, ']');
		assertLocation(*it++, 2, 9);
	}

	{
		SCOPED_TRACE("Token: 10");
		assertCharacter(*it, ',');
		assertLocation(*it++, 2, 10);
	}

	{
		SCOPED_TRACE("Token: 11");
		assertInteger(*it, 0x400, false);
		assertLocation(*it++, 2, 12);
	}

	{
		SCOPED_TRACE("Token: 12");
		assertNewline(*it);
		assertLocation(*it++, 2, 17);
	}

	// Line 3
	{
		SCOPED_TRACE("Token: 13");
		assertNewline(*it);
		assertLocation(*it++, 3, 16);
	}

	// Line 4
	{
		SCOPED_TRACE("Token: 14");
		assertIdentifier(*it, "label");
		assertLocation(*it++, 4, 1);
	}

	{
		SCOPED_TRACE("Token: 15");
		assertCharacter(*it, ':');
		assertLocation(*it++, 4, 6);
	}

	{
		SCOPED_TRACE("Token: 16");
		assertIdentifier(*it, "JSR");
		assertLocation(*it++, 4, 8);
	}

	{
		SCOPED_TRACE("Token: 17");
		assertIdentifier(*it, "label");
		assertLocation(*it++, 4, 12);
	}

	{
		SCOPED_TRACE("Token: 18");
		assertCharacter(*it, '+');
		assertLocation(*it++, 4, 17);
	}

	{
		SCOPED_TRACE("Token: 19");
		assertInteger(*it, 4, false);
		assertLocation(*it++, 4, 18);
	}

	{
		SCOPED_TRACE("Token: 20");
		assertNewline(*it);
		assertLocation(*it++, 4, 19);
	}

	// Line 5
	{
		SCOPED_TRACE("Token: 21");
		assertEOI(*it);
		assertLocation(*it++, 5, 0);
	}
}
