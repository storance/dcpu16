#include "../Parser.hpp"

#include <iostream>
#include <list>
#include <functional>
#include <gtest/gtest.h>

using namespace std;
using namespace std::placeholders;
using namespace dcpu;
using namespace dcpu::common;
using namespace dcpu::ast;
using namespace dcpu::parser;
using namespace dcpu::lexer;

typedef list<shared_ptr<Statement>>::iterator StatementIterator;

void verifyLabel(StatementIterator &it, const string &expectedName) {
	shared_ptr<ast::Statement> statement = *it++;

	shared_ptr<ast::Label> label = dynamic_pointer_cast<ast::Label>(statement);
	ASSERT_TRUE((bool)label);
	EXPECT_EQ(expectedName, label->_name);
}

template <typename FunctionA, typename FunctionB>
void verifyInstruction(StatementIterator &it, Opcode opcode, FunctionA verifyA, FunctionB verifyB) {
	shared_ptr<ast::Statement> statement = *it++;

	shared_ptr<ast::Instruction> instruction = dynamic_pointer_cast<ast::Instruction>(statement);
	ASSERT_TRUE((bool)instruction);
	EXPECT_EQ(opcode, instruction->_opcode);

	{
		SCOPED_TRACE("First Argument"); 
		verifyA(instruction->_a);
	}
	{
		SCOPED_TRACE("Second Argument"); 
		verifyB(instruction->_b);
	}
}

template <typename Function>
void verifyArgumentIsExpression(shared_ptr<Argument> arg, Function verifyArg) {
	shared_ptr<ExpressionArgument> exprArg = dynamic_pointer_cast<ExpressionArgument>(arg);

	ASSERT_TRUE((bool)exprArg);
	{
		SCOPED_TRACE("Verify Expression Argument");
		verifyArg(exprArg->_expr);
	}
}

void verifyExpressionIsRegister(shared_ptr<Expression> expr, common::Register expectedRegister) {
	shared_ptr<RegisterOperand> registerOp = dynamic_pointer_cast<RegisterOperand>(expr);

	ASSERT_TRUE((bool)registerOp);
	EXPECT_EQ(expectedRegister, registerOp->_register);
}

void verifyArgumentIsNull(shared_ptr<Argument> arg) {
	EXPECT_FALSE(arg);
}

typedef string::const_iterator lexer_iterator;

void runParser(string content, int expectedStatements, list<shared_ptr<Statement>> &statements) {
	Lexer<lexer_iterator> lexer(content.begin(), content.end(), "<ParserTest>");
	lexer.parse();

	ErrorHandler errorHandler;

	Parser parser(lexer.tokens.begin(), lexer.tokens.end(), errorHandler);
	parser.parse();	

	statements = parser._statements;
	ASSERT_EQ(expectedStatements, statements.size());
}

auto isRegisterA  = bind(&verifyExpressionIsRegister, _1, Register::A);
auto isRegisterB  = bind(&verifyExpressionIsRegister, _1, Register::B);
auto isRegisterC  = bind(&verifyExpressionIsRegister, _1, Register::C);
auto isRegisterX  = bind(&verifyExpressionIsRegister, _1, Register::X);
auto isRegisterY  = bind(&verifyExpressionIsRegister, _1, Register::Y);
auto isRegisterZ  = bind(&verifyExpressionIsRegister, _1, Register::Z);
auto isRegisterI  = bind(&verifyExpressionIsRegister, _1, Register::I);
auto isRegisterJ  = bind(&verifyExpressionIsRegister, _1, Register::J);
auto isRegisterPC = bind(&verifyExpressionIsRegister, _1, Register::PC);
auto isRegisterSP = bind(&verifyExpressionIsRegister, _1, Register::SP);
auto isRegisterO  = bind(&verifyExpressionIsRegister, _1, Register::O);


TEST(ParserTest, ParseInstruction) {
	list<shared_ptr<Statement>> statements;

	ASSERT_NO_FATAL_FAILURE(runParser("SET A, B\n   add pc  ,\ti\n\njsr X", 3, statements));

	auto it = statements.begin();
	{
		SCOPED_TRACE("First Statement"); 
		verifyInstruction(it, Opcode::SET,
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterA);
			},
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterB);
			});
	}

	{
		SCOPED_TRACE("Second Statement"); 
		verifyInstruction(it, Opcode::ADD,
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterPC);
			},
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterI);
			});
	}

	{
		SCOPED_TRACE("Third Statement"); 
		verifyInstruction(it, Opcode::JSR,
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterX);
			},
			&verifyArgumentIsNull);
	}
}

TEST(ParserTest, LabelTest) {
	list<shared_ptr<Statement>> statements;

	ASSERT_NO_FATAL_FAILURE(runParser("label1:\n:label2\nlabel3: SET A, B\n:label4 SET A, B", 6, statements));

	auto it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		verifyLabel(it, "label1");
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		verifyLabel(it, "label2");
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		verifyLabel(it, "label3");
	}

	{
		SCOPED_TRACE("Statement: 4"); 
		verifyInstruction(it, Opcode::SET,
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterA);
			},
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterB);
			});
	}

	{
		SCOPED_TRACE("Statement: 5"); 
		verifyLabel(it, "label4");
	}

	{
		SCOPED_TRACE("Statement: 6"); 
		verifyInstruction(it, Opcode::SET,
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterA);
			},
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterB);
			});
	}
}

TEST(ParserTest, LabelWithSpaceTest) {
	list<shared_ptr<Statement>> statements;

	ASSERT_NO_FATAL_FAILURE(runParser("label1 :\n: label2\nlabel3 : SET A, B\n: label4 SET A, B", 6, statements));

	auto it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		verifyLabel(it, "label1");
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		verifyLabel(it, "label2");
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		verifyLabel(it, "label3");
	}

	{
		SCOPED_TRACE("Statement: 4"); 
		verifyInstruction(it, Opcode::SET,
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterA);
			},
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterB);
			});
	}

	{
		SCOPED_TRACE("Statement: 5"); 
		verifyLabel(it, "label4");
	}

	{
		SCOPED_TRACE("Statement: 6"); 
		verifyInstruction(it, Opcode::SET,
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterA);
			},
			[](shared_ptr<Argument> arg) {
				verifyArgumentIsExpression(arg, isRegisterB);
			});
	}
}