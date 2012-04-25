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

void assertInstruction(StatementIterator &it, Opcode opcode, ArgumentFunc verifyA) {
	auto& statement = *it++;

	Instruction* instruction = dynamic_cast<ast::Instruction*>(statement.get());
	ASSERT_TRUE(instruction != nullptr);
	EXPECT_EQ(opcode, instruction->_opcode);

	{
		SCOPED_TRACE("Argument b"); 
		EXPECT_FALSE(instruction->_b);
	}
	{
		SCOPED_TRACE("Argument a"); 
		verifyA(instruction->_a);
	}
}

void assertInstruction(StatementIterator &it, Opcode opcode, ArgumentFunc verifyB, ArgumentFunc verifyA) {
	auto& statement = *it++;

	Instruction* instruction = dynamic_cast<ast::Instruction*>(statement.get());
	ASSERT_TRUE(instruction != nullptr);
	EXPECT_EQ(opcode, instruction->_opcode);

	{
		SCOPED_TRACE("Argument b"); 
		verifyB(instruction->_b);
	}
	{
		SCOPED_TRACE("Argument a"); 
		verifyA(instruction->_a);
	}
}

ArgumentFunc assertArgumentIsExpression(ExpressionFunc assertFunc) {
	return [=] (ArgumentPtr& arg) {
		ExpressionArgument *exprArg = dynamic_cast<ExpressionArgument*>(arg.get());
		ASSERT_TRUE(exprArg != nullptr);
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

ArgumentFunc assertArgumentIsStack(StackOperation operation) {
	return [=] (ArgumentPtr& arg) {
		StackArgument *stackArg = dynamic_cast<StackArgument*>(arg.get());

		ASSERT_TRUE(stackArg != nullptr);
		EXPECT_EQ(operation, stackArg->_operation);
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

ExpressionFunc assertIsUnaryOperation(UnaryOperator expectedOperation, ExpressionFunc assertOperand) {

	return [=] (ExpressionPtr& expr) {
		UnaryOperation* unaryOp = dynamic_cast<UnaryOperation*>(expr.get());

		ASSERT_TRUE(unaryOp != nullptr);
		EXPECT_EQ(expectedOperation, unaryOp->_operator);
		{
			SCOPED_TRACE("Verify operand");
			assertOperand(unaryOp->_operand);
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

void runParser(const string &content, int expectedStatements, StatementList &statements) {
	Lexer lexer(content.begin(), content.end(), "<ParserTest>");
	lexer.parse();

	ErrorHandler errorHandler;

	Parser parser(lexer.tokens.begin(), lexer.tokens.end(), errorHandler);
	parser.parse();

	statements = move(parser.statements);

	ASSERT_EQ(expectedStatements, statements.size());
}


TEST(ParserTest, ParseInstruction) {
	StatementList statements;

	ASSERT_NO_FATAL_FAILURE(runParser("SET A, 1\n   add pc  ,\ti\n\njsr X", 3, statements));

	auto it = statements.begin();
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
			assertArgumentIsExpression(assertIsRegister(Register::X)));
	}
}

TEST(ParserTest, LabelTest) {
	StatementList statements;

	ASSERT_NO_FATAL_FAILURE(runParser("label1 :\n:label2\nlabel3: SET A, B\n: label4 SET A, B", 6, statements));

	auto it = statements.begin();
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
	StatementList statements;

	ASSERT_NO_FATAL_FAILURE(runParser("set 4 * 2, 1 + 2", 1, statements));
	auto it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::MULTIPLY, assertIsLiteral(4), assertIsLiteral(2))),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::PLUS, assertIsLiteral(1), assertIsLiteral(2))));
	}
}

TEST(ParserTest, IndirectionTest) {
	StatementList statements;

	ASSERT_NO_FATAL_FAILURE(runParser("set [A], [1 * 2]", 1, statements));
	auto it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsIndirect(assertIsRegister(Register::A)),
			assertArgumentIsIndirect(
				assertIsBinaryOperation(BinaryOperator::MULTIPLY, assertIsLiteral(1), assertIsLiteral(2))));
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set [B - 4], [5 + J]", 1, statements));
	it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsIndirect(
				assertIsBinaryOperation(BinaryOperator::MINUS, assertIsRegister(Register::B), assertIsLiteral(4))),
			assertArgumentIsIndirect(
				assertIsBinaryOperation(BinaryOperator::PLUS, assertIsLiteral(5), assertIsRegister(Register::J))));
	}
}

TEST(ParserTest, OperatorPrecedenceTest) {
	StatementList statements;

	ASSERT_NO_FATAL_FAILURE(runParser("set A, 1 + 2 * 3", 1, statements));
	auto it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::PLUS,
					assertIsLiteral(1),
					assertIsBinaryOperation(BinaryOperator::MULTIPLY, 
						assertIsLiteral(2),
						assertIsLiteral(3)
					)
				)
			)
		);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set A, 1 & 2 | 3 ^ 4", 1, statements));
	it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::OR,
					assertIsBinaryOperation(BinaryOperator::AND,
						assertIsLiteral(1),
						assertIsLiteral(2)
					), assertIsBinaryOperation(BinaryOperator::XOR, 
						assertIsLiteral(3),
						assertIsLiteral(4)
					)
				)
			)
		);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set A, 1 << 4 & 2 >> 3", 1, statements));
	it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::AND,
					assertIsBinaryOperation(BinaryOperator::SHIFT_LEFT,
						assertIsLiteral(1),
						assertIsLiteral(4)
					), assertIsBinaryOperation(BinaryOperator::SHIFT_RIGHT, 
						assertIsLiteral(2),
						assertIsLiteral(3)
					)
				)
			)
		);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set A, 1 + 2 << 3 >> 4 - 5", 1, statements));
	it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::SHIFT_RIGHT,
					assertIsBinaryOperation(BinaryOperator::SHIFT_LEFT,
						assertIsBinaryOperation(BinaryOperator::PLUS,
							assertIsLiteral(1),
							assertIsLiteral(2)
						), assertIsLiteral(3)
					), assertIsBinaryOperation(BinaryOperator::MINUS,
						assertIsLiteral(4),
						assertIsLiteral(5)
					)
				)
			)
		);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set A, 1 * 2 + 3 / 4 % 5", 1, statements));
	it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::PLUS,
					assertIsBinaryOperation(BinaryOperator::MULTIPLY,
						assertIsLiteral(1),
						assertIsLiteral(2)
					), assertIsBinaryOperation(BinaryOperator::MODULO,
						assertIsBinaryOperation(BinaryOperator::DIVIDE,
							assertIsLiteral(3),
							assertIsLiteral(4)
						),
						assertIsLiteral(5)
					)
				)
			)
		);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set A, -1 * 2 / +3 % ~4", 1, statements));
	it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::MODULO,
					assertIsBinaryOperation(BinaryOperator::DIVIDE,
						assertIsBinaryOperation(BinaryOperator::MULTIPLY,
							assertIsUnaryOperation(UnaryOperator::MINUS, assertIsLiteral(1)),
							assertIsLiteral(2)
						), assertIsUnaryOperation(UnaryOperator::PLUS, assertIsLiteral(3))
					), assertIsUnaryOperation(UnaryOperator::NOT, assertIsLiteral(4))
				)
			)
		);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set A, -(1 + 2) * (3 + 4)", 1, statements));
	it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(
				assertIsBinaryOperation(BinaryOperator::MULTIPLY,
					assertIsUnaryOperation(UnaryOperator::MINUS,
						assertIsBinaryOperation(BinaryOperator::PLUS,
							assertIsLiteral(1),
							assertIsLiteral(2)
						)
					), assertIsBinaryOperation(BinaryOperator::PLUS,
						assertIsLiteral(3),
						assertIsLiteral(4)
					)
				)
			)
		);
	}
}

TEST(ParserTest, LabelReferencesTest) {
	StatementList statements;

	ASSERT_NO_FATAL_FAILURE(runParser("label: set label, [label * 2]\n:a SET a, $a", 4, statements));
	auto it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertLabel(it, "label");
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsLabelRef("label")),
			assertArgumentIsIndirect(
				assertIsBinaryOperation(BinaryOperator::MULTIPLY, assertIsLabelRef("label"), assertIsLiteral(2))));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertLabel(it, "a");
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsExpression(assertIsLabelRef("a")));
	}
}

TEST(ParserTest, StackArgumentsTest) {
	StatementList statements;

	ASSERT_NO_FATAL_FAILURE(runParser("set A, [SP]\nset B, [SP++]\nset [--SP] , C", 3, statements));
	auto it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsStack(StackOperation::PEEK));
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::B)),
			assertArgumentIsStack(StackOperation::POP));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsStack(StackOperation::PUSH),
			assertArgumentIsExpression(assertIsRegister(Register::C)));
	}

	ASSERT_NO_FATAL_FAILURE(runParser("set A, PEEK\nset B, POP\nset PUSH , C", 3, statements));
	it = statements.begin();
	{
		SCOPED_TRACE("Statement: 1"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::A)),
			assertArgumentIsStack(StackOperation::PEEK));
	}

	{
		SCOPED_TRACE("Statement: 2"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsExpression(assertIsRegister(Register::B)),
			assertArgumentIsStack(StackOperation::POP));
	}

	{
		SCOPED_TRACE("Statement: 3"); 
		assertInstruction(it, Opcode::SET,
			assertArgumentIsStack(StackOperation::PUSH),
			assertArgumentIsExpression(assertIsRegister(Register::C)));
	}
}