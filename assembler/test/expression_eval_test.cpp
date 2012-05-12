#include <gtest/gtest.h>

#include <expression.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::lexer;
using namespace dcpu::ast;

static location_ptr test_location = make_shared<lexer::location>("<Test>", 1, 1);

TEST(ExpressionEvalTest, AddTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::PLUS,
		literal_operand(test_location, 4),
		literal_operand(test_location, 9)));

	EXPECT_EQ(evaluated_expression(test_location, 13), evaluate(expr));
}

TEST(ExpressionEvalTest, SubtractTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::MINUS,
		literal_operand(test_location, 4),
		literal_operand(test_location, 9)));

	EXPECT_EQ(evaluated_expression(test_location, -5), evaluate(expr));
}

TEST(ExpressionEvalTest, MultiplyTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::MULTIPLY,
		literal_operand(test_location, 2),
		literal_operand(test_location, 4)));

	EXPECT_EQ(evaluated_expression(test_location, 8), evaluate(expr));
}

TEST(ExpressionEvalTest, DivideTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::DIVIDE,
		literal_operand(test_location, 8),
		literal_operand(test_location, 2)));

	EXPECT_EQ(evaluated_expression(test_location, 4), evaluate(expr));

	auto expr2 = expression(binary_operation(test_location, binary_operator::DIVIDE,
		literal_operand(test_location, 15),
		literal_operand(test_location, 4)));

	EXPECT_EQ(evaluated_expression(test_location, 3), evaluate(expr2));
}

TEST(ExpressionEvalTest, ModuloTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::MODULO,
		literal_operand(test_location, 19),
		literal_operand(test_location, 5)));

	EXPECT_EQ(evaluated_expression(test_location, 4), evaluate(expr));
}

TEST(ExpressionEvalTest, ShiftLeftTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::SHIFT_LEFT,
		literal_operand(test_location, 1),
		literal_operand(test_location, 4)));

	EXPECT_EQ(evaluated_expression(test_location, 16), evaluate(expr));
}

TEST(ExpressionEvalTest, ShiftRightTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::SHIFT_RIGHT,
		literal_operand(test_location, 256),
		literal_operand(test_location, 2)));

	EXPECT_EQ(evaluated_expression(test_location, 64), evaluate(expr));
}

TEST(ExpressionEvalTest, BitwiseAndTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::AND,
		literal_operand(test_location, 0x9),
		literal_operand(test_location, 0xc)));

	EXPECT_EQ(evaluated_expression(test_location, 0x8), evaluate(expr));
}

TEST(ExpressionEvalTest, BitwiseXorTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::XOR,
		literal_operand(test_location, 0x9),
		literal_operand(test_location, 0xc)));

	EXPECT_EQ(evaluated_expression(test_location, 0x5), evaluate(expr));
}

TEST(ExpressionEvalTest, BitwiseOrTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::OR,
		literal_operand(test_location, 0x9),
		literal_operand(test_location, 0xc)));

	EXPECT_EQ(evaluated_expression(test_location, 0xd), evaluate(expr));
}

TEST(ExpressionEvalTest, UnaryMinus) {
	auto expr = expression(unary_operation(test_location, unary_operator::MINUS,
		literal_operand(test_location, 7)));

	EXPECT_EQ(evaluated_expression(test_location, -7), evaluate(expr));
}

TEST(ExpressionEvalTest, UnaryPlus) {
	auto expr = expression(unary_operation(test_location, unary_operator::PLUS,
		literal_operand(test_location, 7)));

	EXPECT_EQ(evaluated_expression(test_location, 7), evaluate(expr));
}

TEST(ExpressionEvalTest, UnaryNot) {
	auto expr = expression(unary_operation(test_location, unary_operator::NOT,
		literal_operand(test_location, 7)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr));

	auto expr2 = expression(unary_operation(test_location, unary_operator::NOT,
		literal_operand(test_location, 0)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr2));
}

TEST(ExpressionEvalTest, UnaryBitwiseNot) {
	auto expr = expression(unary_operation(test_location, unary_operator::BITWISE_NOT,
		literal_operand(test_location, 7)));

	EXPECT_EQ(evaluated_expression(test_location, -8), evaluate(expr));
}

TEST(ExpressionEvalTest, RegisterTest) {
	auto expr = expression(binary_operation(test_location, binary_operator::PLUS,
		register_operand(test_location, registers::A),
		literal_operand(test_location, 10)));
	EXPECT_EQ(evaluated_expression(test_location, registers::A, 10), evaluate(expr));

	auto expr2 = expression(binary_operation(test_location, binary_operator::PLUS,
		literal_operand(test_location, 10),
		register_operand(test_location, registers::B)));
	EXPECT_EQ(evaluated_expression(test_location, registers::B, 10), evaluate(expr2));

	auto expr3 = expression(binary_operation(test_location, binary_operator::MINUS,
		register_operand(test_location, registers::C),
		literal_operand(test_location, 10)));
	EXPECT_EQ(evaluated_expression(test_location, registers::C, -10), evaluate(expr3));

	auto expr4 = expression(register_operand(test_location, registers::X));
	EXPECT_EQ(evaluated_expression(test_location, registers::X), evaluate(expr4));

	auto expr5 = expression(binary_operation(test_location, binary_operator::PLUS,
		register_operand(test_location, registers::A),
		binary_operation(test_location, binary_operator::MULTIPLY,
			literal_operand(test_location, 2),
			literal_operand(test_location, 4))));
	EXPECT_EQ(evaluated_expression(test_location, registers::A, 8), evaluate(expr5));
}
