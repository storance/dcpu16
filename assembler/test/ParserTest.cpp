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

typedef StatementList::iterator StatementIterator;

typedef function<void (ArgumentPtr&)> ArgumentFunc;
typedef function<void (ExpressionPtr&)> ExpressionFunc;


void assertLabel(StatementIterator &it, const string &expectedName) {
	auto& statement = *it++;

	Label* label = dynamic_cast<Label*>(statement.get());
	ASSERT_TRUE(label != nullptr);
	EXPECT_EQ(expectedName, label->_name);
}

void assertInstruction(StatementIterator &it, Opcode opcode, ArgumentFunc verifyA, ArgumentFunc verifyB) {
	auto& statement = *it++;

	Instruction* instruction = dynamic_cast<ast::Instruction*>(statement.get());
	ASSERT_TRUE(instruction != nullptr);
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

ArgumentFunc assertArgumentIsExpression(ExpressionFunc assertFunc) {
	return [=] (ArgumentPtr& arg) {
		ExpressionArgument *exprArg = dynamic_cast<ExpressionArgument*>(arg.get());

		ASSERT_TRUE((bool)exprArg);
		{
			SCOPED_TRACE("Verify Expression Argument");
			assertFunc(exprArg->_expr);
		}
	};
}

ArgumentFunc assertArgumentIsIndirect(ExpressionFunc assertFunc) {
	return [=] (ArgumentPtr& arg) {
		IndirectArgument *exprArg = dynamic_cast<IndirectArgument*>(arg.get());

		ASSERT_TRUE(exprArg != nullptr);
		{
			SCOPED_TRACE("Verify Indirect Argument");
			assertFunc(exprArg->_expr);
		}
	};
}

ArgumentFunc assertArgumentIsNull() {
	return [=] (ArgumentPtr& arg) {
		EXPECT_FALSE(arg);
	};
}

ExpressionFunc assertIsBinaryOperation(BinaryOperator expectedOperation,
	ExpressionFunc assertLeft, ExpressionFunc assertRight) {

	return [=] (ExpressionPtr& expr) {
		BinaryOperation* binaryOp = dynamic_cast<BinaryOperation*>(expr.get());

		ASSERT_TRUE(binaryOp != nullptr);
		EXPECT_EQ(expectedOperation, binaryOp->_operator);
		{
			SCOPED_TRACE("Verify left-hand expression");
			assertLeft(binaryOp->_left);
		}
		{
			SCOPED_TRACE("Verify right-hand expression");
			assertRight(binaryOp->_right);
		}
	};
}

ExpressionFunc assertIsLabelRef(const string &labelName) {
	return [=] (ExpressionPtr& expr) {
		LabelReferenceOperand* labelRefOp = dynamic_cast<LabelReferenceOperand*>(expr.get());

		ASSERT_TRUE(labelRefOp != nullptr);
		EXPECT_EQ(labelName, labelRefOp->_label);
	};
}

ExpressionFunc assertIsRegister(common::Register expectedRegister) {
	return [=] (ExpressionPtr& expr) {
		RegisterOperand* registerOp = dynamic_cast<RegisterOperand*>(expr.get());

		ASSERT_TRUE(registerOp != nullptr);
		EXPECT_EQ(expectedRegister, registerOp->_register);
	};
}

ExpressionFunc assertIsLiteral(uint32_t expectedValue) {
	return [=] (ExpressionPtr& expr) {
		LiteralOperand* literalOp = dynamic_cast<LiteralOperand*>(expr.get());

		ASSERT_TRUE(literalOp != nullptr);
		EXPECT_EQ(expectedValue, literalOp->_value);
	};
}

typedef string::const_iterator lexer_iterator;

void runParser(const string &content, int expectedStatements, unique_ptr<Parser> &parser) {
	Lexer<lexer_iterator> lexer(content.begin(), content.end(), "<ParserTest>");
	lexer.parse();

	ErrorHandler errorHandler;

	parser.reset(new Parser(lexer.tokens.begin(), lexer.tokens.end(), errorHandler));
	parser->parse();

	ASSERT_EQ(expectedStatements, parser->_statements.size());
}


TEST(ParserTest, ParseInstruction) {
	unique_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("SET A, 1\n   add pc  ,\ti\n\njsr X", 3, parser));

	auto it = parser->_statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(assertIsLiteral(1)));
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::ADD,
			assertArgumentIsExpression(assertIsRegister(Register::PC)),
			assertArgumentIsExpression(assertIsRegister(Register::I)));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertInstruction(it, Opcode::JSR,
			assertArgumentIsExpression(assertIsRegister(Register::X)),
			assertArgumentIsNull());
	}
}

TEST(ParserTest, LabelTest) {
	unique_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("label1 :\n:label2\nlabel3: SET A, B\n: label4 SET A, B", 6, parser));

	auto it = parser->_statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertLabel(it, "label1");
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertLabel(it, "label2");
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertLabel(it, "label3");
	}

	{
		SCOPED_TRACE("Statement: 4"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(assertIsRegister(Register::B)));
	}

	{
		SCOPED_TRACE("Statement: 5"); 
		assertLabel(it, "label4");
	}

	{
		SCOPED_TRACE("Statement: 6"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(assertIsRegister(Register::B)));
	}
}

TEST(ParserTest, SimpleExpressionTest) {
	unique_ptr<Parser> parser;

	ASSERT_NO_FATAL_FAILURE(runParser("set 4 * 2, 1 + 2", 1, parser));
	auto it = parser->_statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::MULTIPLY, assertIsLiteral(4), assertIsLiteral(2))),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::PLUS, assertIsLiteral(1), assertIsLiteral(2))));
	}
}