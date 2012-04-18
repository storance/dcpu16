#include "../Parser.hpp"

#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include <boost/bind.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::parser;
using namespace dcpu::lexer;

void verifyLabel(list<Statement>::const_iterator &it, string expectedName) {
	Statement statement = *it++;

	Label *label = boost::get<Label>(&statement);
	ASSERT_TRUE(label != nullptr);
	EXPECT_EQ(expectedName, label->name);
}

template <typename FunctionA, typename FunctionB>
void verifyInstruction(list<Statement>::iterator &it, Opcode opcode, FunctionA verifyA, FunctionB verifyB) {
	Statement statement = *it++;

	Instruction *instruction = boost::get<Instruction>(&statement);
	ASSERT_TRUE(instruction != nullptr);
	EXPECT_EQ(opcode, instruction->opcode);

	{
		SCOPED_TRACE("First Argument"); 
		verifyA(instruction->a);
	}
	{
		SCOPED_TRACE("Second Argument"); 
		verifyB(instruction->b);
	}
}

void verifyArgumentIsRegister(Register expectedRegister, Argument arg) {
	Register *type = boost::get<Register>(&arg);
	ASSERT_TRUE(type != nullptr);
	EXPECT_EQ(expectedRegister, *type);
}

void verifyArgumentIsNull(Argument arg) {
	EXPECT_EQ(0, arg.which());
}

typedef string::const_iterator lexer_iterator;

void runParser(string content, int expectedStatements, std::list<Statement> &statements) {
	Lexer<lexer_iterator> lexer(content.begin(), content.end(), "<ParserTest>");
	lexer.parse();

	ErrorHandler errorHandler;

	Parser parser(lexer.tokens.begin(), lexer.tokens.end(), errorHandler);
	parser.parse();	

	statements = parser.statements;
	ASSERT_EQ(expectedStatements, statements.size());
}

TEST(ParserTest, ParseInstruction) {
	list<Statement> statements;

	ASSERT_NO_FATAL_FAILURE(runParser("SET A, B\n   add pc  ,\ti\n\njsr X", 3, statements));

	auto it = statements.begin();
	{
		SCOPED_TRACE("First Statement"); 
		verifyInstruction(it, Opcode::SET,
			boost::bind(&verifyArgumentIsRegister, Register::A, _1),
			boost::bind(&verifyArgumentIsRegister, Register::B, _1));
	}

	{
		SCOPED_TRACE("Second Statement"); 
		verifyInstruction(it, Opcode::ADD,
			boost::bind(&verifyArgumentIsRegister, Register::PC, _1),
			boost::bind(&verifyArgumentIsRegister, Register::I, _1));
	}

	{
		SCOPED_TRACE("Third Statement"); 
		verifyInstruction(it, Opcode::JSR,
			boost::bind(&verifyArgumentIsRegister, Register::X, _1),
			&verifyArgumentIsNull);
	}
}