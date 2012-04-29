#include <gtest/gtest.h>

#include <ast/Expression.hpp>

using namespace dcpu::lexer;
using namespace dcpu::ast;

static Location location("<Test>", 1, 1);

TEST(ExpressionEvalTest, AddTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::PLUS,
		Expression::literalOperand(location, 4),
		Expression::literalOperand(location, 9));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 13), expr->evaluate());
}

TEST(ExpressionEvalTest, SubtractTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::MINUS,
		Expression::literalOperand(location, 4),
		Expression::literalOperand(location, 9));

	EXPECT_EQ(Expression::evaluatedLiteral(location, -5), expr->evaluate());
}

TEST(ExpressionEvalTest, MultiplyTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::MULTIPLY,
		Expression::literalOperand(location, 2),
		Expression::literalOperand(location, 4));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 8), expr->evaluate());
}

TEST(ExpressionEvalTest, DivideTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::DIVIDE,
		Expression::literalOperand(location, 8),
		Expression::literalOperand(location, 2));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 4), expr->evaluate());

	auto expr2 = Expression::binaryOperation(location, BinaryOperator::DIVIDE,
		Expression::literalOperand(location, 15),
		Expression::literalOperand(location, 4));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 3), expr2->evaluate());
}

TEST(ExpressionEvalTest, ModuloTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::MODULO,
		Expression::literalOperand(location, 19),
		Expression::literalOperand(location, 5));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 4), expr->evaluate());
}

TEST(ExpressionEvalTest, ShiftLeftTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::SHIFT_LEFT,
		Expression::literalOperand(location, 1),
		Expression::literalOperand(location, 4));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 16), expr->evaluate());
}

TEST(ExpressionEvalTest, ShiftRightTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::SHIFT_RIGHT,
		Expression::literalOperand(location, 256),
		Expression::literalOperand(location, 2));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 64), expr->evaluate());
}

TEST(ExpressionEvalTest, BitwiseAndTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::AND,
		Expression::literalOperand(location, 0x9),
		Expression::literalOperand(location, 0xc));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 0x8), expr->evaluate());
}

TEST(ExpressionEvalTest, BitwiseXorTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::XOR,
		Expression::literalOperand(location, 0x9),
		Expression::literalOperand(location, 0xc));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 0x5), expr->evaluate());
}

TEST(ExpressionEvalTest, BitwiseOrTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::OR,
		Expression::literalOperand(location, 0x9),
		Expression::literalOperand(location, 0xc));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 0xd), expr->evaluate());
}

TEST(ExpressionEvalTest, UnaryMinus) {
	auto expr = Expression::unaryOperation(location, UnaryOperator::MINUS,
		Expression::literalOperand(location, 7));

	EXPECT_EQ(Expression::evaluatedLiteral(location, -7), expr->evaluate());
}

TEST(ExpressionEvalTest, UnaryPlus) {
	auto expr = Expression::unaryOperation(location, UnaryOperator::PLUS,
		Expression::literalOperand(location, 7));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 7), expr->evaluate());
}

TEST(ExpressionEvalTest, UnaryNot) {
	auto expr = Expression::unaryOperation(location, UnaryOperator::NOT,
		Expression::literalOperand(location, 7));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 0), expr->evaluate());

	auto expr2 = Expression::unaryOperation(location, UnaryOperator::NOT,
		Expression::literalOperand(location, 0));

	EXPECT_EQ(Expression::evaluatedLiteral(location, 1), expr2->evaluate());
}

TEST(ExpressionEvalTest, UnaryBitwiseNot) {
	auto expr = Expression::unaryOperation(location, UnaryOperator::BITWISE_NOT,
		Expression::literalOperand(location, 7));

	EXPECT_EQ(Expression::evaluatedLiteral(location, -8), expr->evaluate());
}

TEST(ExpressionEvalTest, RegisterTest) {
	auto expr = Expression::binaryOperation(location, BinaryOperator::PLUS,
		Expression::registerOperand(location, Register::A),
		Expression::literalOperand(location, 10));
	EXPECT_EQ(Expression::evaluatedRegister(location, Register::A, 10), expr->evaluate());

	auto expr2 = Expression::binaryOperation(location, BinaryOperator::PLUS,
		Expression::literalOperand(location, 10),
		Expression::registerOperand(location, Register::B));
	EXPECT_EQ(Expression::evaluatedRegister(location, Register::B, 10), expr2->evaluate());

	auto expr3 = Expression::binaryOperation(location, BinaryOperator::MINUS,
		Expression::registerOperand(location, Register::C),
		Expression::literalOperand(location, 10));
	EXPECT_EQ(Expression::evaluatedRegister(location, Register::C, -10), expr3->evaluate());

	auto expr4 = Expression::registerOperand(location, Register::X);
	EXPECT_EQ(Expression::evaluatedRegister(location, Register::X), expr4->evaluate());

	auto expr5 = Expression::binaryOperation(location, BinaryOperator::PLUS,
		Expression::registerOperand(location, Register::A),
		Expression::binaryOperation(location, BinaryOperator::MULTIPLY,
			Expression::literalOperand(location, 2),
			Expression::literalOperand(location, 4)));
	EXPECT_EQ(Expression::evaluatedRegister(location, Register::A, 8), expr5->evaluate());
}