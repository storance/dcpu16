#include <gtest/gtest.h>

#include <Expression.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::lexer;
using namespace dcpu::ast;

static location_t location = make_shared<Location>("<Test>", 1, 1);

TEST(ExpressionEvalTest, AddTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::PLUS,
		LiteralOperand(location, 4),
		LiteralOperand(location, 9)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 13)), evaluate(expr));
}

TEST(ExpressionEvalTest, SubtractTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::MINUS,
		LiteralOperand(location, 4),
		LiteralOperand(location, 9)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, -5)), evaluate(expr));
}

TEST(ExpressionEvalTest, MultiplyTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::MULTIPLY,
		LiteralOperand(location, 2),
		LiteralOperand(location, 4)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 8)), evaluate(expr));
}

TEST(ExpressionEvalTest, DivideTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::DIVIDE,
		LiteralOperand(location, 8),
		LiteralOperand(location, 2)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 4)), evaluate(expr));

	auto expr2 = Expression(BinaryOperation(location, BinaryOperator::DIVIDE,
		LiteralOperand(location, 15),
		LiteralOperand(location, 4)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 3)), evaluate(expr2));
}

TEST(ExpressionEvalTest, ModuloTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::MODULO,
		LiteralOperand(location, 19),
		LiteralOperand(location, 5)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 4)), evaluate(expr));
}

TEST(ExpressionEvalTest, ShiftLeftTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::SHIFT_LEFT,
		LiteralOperand(location, 1),
		LiteralOperand(location, 4)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 16)), evaluate(expr));
}

TEST(ExpressionEvalTest, ShiftRightTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::SHIFT_RIGHT,
		LiteralOperand(location, 256),
		LiteralOperand(location, 2)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 64)), evaluate(expr));
}

TEST(ExpressionEvalTest, BitwiseAndTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::AND,
		LiteralOperand(location, 0x9),
		LiteralOperand(location, 0xc)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 0x8)), evaluate(expr));
}

TEST(ExpressionEvalTest, BitwiseXorTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::XOR,
		LiteralOperand(location, 0x9),
		LiteralOperand(location, 0xc)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 0x5)), evaluate(expr));
}

TEST(ExpressionEvalTest, BitwiseOrTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::OR,
		LiteralOperand(location, 0x9),
		LiteralOperand(location, 0xc)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 0xd)), evaluate(expr));
}

TEST(ExpressionEvalTest, UnaryMinus) {
	auto expr = Expression(UnaryOperation(location, UnaryOperator::MINUS,
		LiteralOperand(location, 7)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, -7)), evaluate(expr));
}

TEST(ExpressionEvalTest, UnaryPlus) {
	auto expr = Expression(UnaryOperation(location, UnaryOperator::PLUS,
		LiteralOperand(location, 7)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 7)), evaluate(expr));
}

TEST(ExpressionEvalTest, UnaryNot) {
	auto expr = Expression(UnaryOperation(location, UnaryOperator::NOT,
		LiteralOperand(location, 7)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 0)), evaluate(expr));

	auto expr2 = Expression(UnaryOperation(location, UnaryOperator::NOT,
		LiteralOperand(location, 0)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, 1)), evaluate(expr2));
}

TEST(ExpressionEvalTest, UnaryBitwiseNot) {
	auto expr = Expression(UnaryOperation(location, UnaryOperator::BITWISE_NOT,
		LiteralOperand(location, 7)));

	EXPECT_EQ(Expression(EvaluatedExpression(location, -8)), evaluate(expr));
}

TEST(ExpressionEvalTest, RegisterTest) {
	auto expr = Expression(BinaryOperation(location, BinaryOperator::PLUS,
		RegisterOperand(location, Register::A),
		LiteralOperand(location, 10)));
	EXPECT_EQ(Expression(EvaluatedExpression(location, Register::A, 10)), evaluate(expr));

	auto expr2 = Expression(BinaryOperation(location, BinaryOperator::PLUS,
		LiteralOperand(location, 10),
		RegisterOperand(location, Register::B)));
	EXPECT_EQ(Expression(EvaluatedExpression(location, Register::B, 10)), evaluate(expr2));

	auto expr3 = Expression(BinaryOperation(location, BinaryOperator::MINUS,
		RegisterOperand(location, Register::C),
		LiteralOperand(location, 10)));
	EXPECT_EQ(Expression(EvaluatedExpression(location, Register::C, -10)), evaluate(expr3));

	auto expr4 = Expression(RegisterOperand(location, Register::X));
	EXPECT_EQ(Expression(EvaluatedExpression(location, Register::X)), evaluate(expr4));

	auto expr5 = Expression(BinaryOperation(location, BinaryOperator::PLUS,
		RegisterOperand(location, Register::A),
		BinaryOperation(location, BinaryOperator::MULTIPLY,
			LiteralOperand(location, 2),
			LiteralOperand(location, 4))));
	EXPECT_EQ(Expression(EvaluatedExpression(location, Register::A, 8)), evaluate(expr5));
}