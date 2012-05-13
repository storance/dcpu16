#include <gtest/gtest.h>

#include <boost/variant.hpp>
#include <expression.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::lexer;
using namespace dcpu::ast;

static location_ptr test_location = make_shared<lexer::location>("<Test>", 1, 1);

TEST(ExpressionEval, Add) {
	auto expr = expression(binary_operation(test_location, binary_operator::PLUS,
		literal_operand(test_location, 4),
		literal_operand(test_location, 9)));

	EXPECT_EQ(evaluated_expression(test_location, 13), evaluate(expr));
}

TEST(ExpressionEval, Subtract) {
	auto expr = expression(binary_operation(test_location, binary_operator::MINUS,
		literal_operand(test_location, 4),
		literal_operand(test_location, 9)));

	EXPECT_EQ(evaluated_expression(test_location, -5), evaluate(expr));
}

TEST(ExpressionEval, Multiply) {
	auto expr = expression(binary_operation(test_location, binary_operator::MULTIPLY,
		literal_operand(test_location, 2),
		literal_operand(test_location, 4)));

	EXPECT_EQ(evaluated_expression(test_location, 8), evaluate(expr));
}

TEST(ExpressionEval, Divide) {
	auto expr = expression(binary_operation(test_location, binary_operator::DIVIDE,
		literal_operand(test_location, 8),
		literal_operand(test_location, 2)));

	EXPECT_EQ(evaluated_expression(test_location, 4), evaluate(expr));

	auto expr2 = expression(binary_operation(test_location, binary_operator::DIVIDE,
		literal_operand(test_location, 15),
		literal_operand(test_location, 4)));

	EXPECT_EQ(evaluated_expression(test_location, 3), evaluate(expr2));
}

TEST(ExpressionEval, Modulo) {
	auto expr = expression(binary_operation(test_location, binary_operator::MODULO,
		literal_operand(test_location, 19),
		literal_operand(test_location, 5)));

	EXPECT_EQ(evaluated_expression(test_location, 4), evaluate(expr));
}

TEST(ExpressionEval, ShiftLeft) {
	auto expr = expression(binary_operation(test_location, binary_operator::SHIFT_LEFT,
		literal_operand(test_location, 1),
		literal_operand(test_location, 4)));

	EXPECT_EQ(evaluated_expression(test_location, 16), evaluate(expr));
}

TEST(ExpressionEval, ShiftRight) {
	auto expr = expression(binary_operation(test_location, binary_operator::SHIFT_RIGHT,
		literal_operand(test_location, 256),
		literal_operand(test_location, 2)));

	EXPECT_EQ(evaluated_expression(test_location, 64), evaluate(expr));
}

TEST(ExpressionEval, BitwiseAnd) {
	auto expr = expression(binary_operation(test_location, binary_operator::BITWISE_AND,
		literal_operand(test_location, 0x9),
		literal_operand(test_location, 0xc)));

	EXPECT_EQ(evaluated_expression(test_location, 0x8), evaluate(expr));
}

TEST(ExpressionEval, BitwiseXor) {
	auto expr = expression(binary_operation(test_location, binary_operator::BITWISE_XOR,
		literal_operand(test_location, 0x9),
		literal_operand(test_location, 0xc)));

	EXPECT_EQ(evaluated_expression(test_location, 0x5), evaluate(expr));
}

TEST(ExpressionEval, BitwiseOr) {
	auto expr = expression(binary_operation(test_location, binary_operator::BITWISE_OR,
		literal_operand(test_location, 0x9),
		literal_operand(test_location, 0xc)));

	EXPECT_EQ(evaluated_expression(test_location, 0xd), evaluate(expr));
}

TEST(ExpressionEval, Equals) {
	expression expr1(binary_operation(test_location, binary_operator::EQ,
		literal_operand(test_location, 19),
		literal_operand(test_location, 19)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr1));

	expression expr2(binary_operation(test_location, binary_operator::EQ,
		literal_operand(test_location, 43),
		literal_operand(test_location, 17)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr2));
}

TEST(ExpressionEval, NotEquals) {
	expression expr1(binary_operation(test_location, binary_operator::NEQ,
		literal_operand(test_location, 19),
		literal_operand(test_location, 10)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr1));

	expression expr2(binary_operation(test_location, binary_operator::NEQ,
		literal_operand(test_location, 43),
		literal_operand(test_location, 43)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr2));
}

TEST(ExpressionEval, LessThan) {
	expression expr1(binary_operation(test_location, binary_operator::LT,
		literal_operand(test_location, 8),
		literal_operand(test_location, 19)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr1));

	expression expr2(binary_operation(test_location, binary_operator::LT,
		literal_operand(test_location, 43),
		literal_operand(test_location, 17)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr2));

	expression expr3(binary_operation(test_location, binary_operator::LT,
		literal_operand(test_location, 17),
		literal_operand(test_location, 17)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr3));
}

TEST(ExpressionEval, GreaterThan) {
	expression expr1(binary_operation(test_location, binary_operator::GT,
		literal_operand(test_location, 19),
		literal_operand(test_location, 8)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr1));

	expression expr2(binary_operation(test_location, binary_operator::GT,
		literal_operand(test_location, 17),
		literal_operand(test_location, 43)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr2));

	expression expr3(binary_operation(test_location, binary_operator::GT,
		literal_operand(test_location, 17),
		literal_operand(test_location, 17)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr3));
}

TEST(ExpressionEval, LessThanOrEqual) {
	expression expr1(binary_operation(test_location, binary_operator::LTE,
		literal_operand(test_location, 8),
		literal_operand(test_location, 19)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr1));

	expression expr2(binary_operation(test_location, binary_operator::LTE,
		literal_operand(test_location, 43),
		literal_operand(test_location, 17)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr2));

	expression expr3(binary_operation(test_location, binary_operator::LTE,
		literal_operand(test_location, 17),
		literal_operand(test_location, 17)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr3));
}

TEST(ExpressionEval, GreaterThanOrEqual) {
	expression expr1(binary_operation(test_location, binary_operator::GTE,
		literal_operand(test_location, 19),
		literal_operand(test_location, 8)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr1));

	expression expr2(binary_operation(test_location, binary_operator::GTE,
		literal_operand(test_location, 17),
		literal_operand(test_location, 43)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr2));

	expression expr3(binary_operation(test_location, binary_operator::GTE,
		literal_operand(test_location, 17),
		literal_operand(test_location, 17)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr3));
}

TEST(ExpressionEval, BooleanAnd) {
	expression expr1(binary_operation(test_location, binary_operator::AND,
		literal_operand(test_location, 1),
		literal_operand(test_location, 1)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr1));

	expression expr2(binary_operation(test_location, binary_operator::AND,
		literal_operand(test_location, 0),
		literal_operand(test_location, 1)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr2));

	expression expr3(binary_operation(test_location, binary_operator::AND,
		literal_operand(test_location, 1),
		literal_operand(test_location, 0)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr3));

	expression expr4(binary_operation(test_location, binary_operator::AND,
		literal_operand(test_location, 0),
		literal_operand(test_location, 0)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr4));
}

TEST(ExpressionEval, BooleanOr) {
	expression expr1(binary_operation(test_location, binary_operator::OR,
		literal_operand(test_location, 1),
		literal_operand(test_location, 1)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr1));

	expression expr2(binary_operation(test_location, binary_operator::OR,
		literal_operand(test_location, 0),
		literal_operand(test_location, 1)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr2));

	expression expr3(binary_operation(test_location, binary_operator::OR,
		literal_operand(test_location, 1),
		literal_operand(test_location, 0)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr3));

	expression expr4(binary_operation(test_location, binary_operator::OR,
		literal_operand(test_location, 0),
		literal_operand(test_location, 0)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr4));
}

TEST(ExpressionEval, UnaryMinus) {
	auto expr = expression(unary_operation(test_location, unary_operator::MINUS,
		literal_operand(test_location, 7)));

	EXPECT_EQ(evaluated_expression(test_location, -7), evaluate(expr));
}

TEST(ExpressionEval, UnaryPlus) {
	auto expr = expression(unary_operation(test_location, unary_operator::PLUS,
		literal_operand(test_location, 7)));

	EXPECT_EQ(evaluated_expression(test_location, 7), evaluate(expr));
}

TEST(ExpressionEval, UnaryNot) {
	auto expr = expression(unary_operation(test_location, unary_operator::NOT,
		literal_operand(test_location, 7)));

	EXPECT_EQ(evaluated_expression(test_location, 0), evaluate(expr));

	auto expr2 = expression(unary_operation(test_location, unary_operator::NOT,
		literal_operand(test_location, 0)));

	EXPECT_EQ(evaluated_expression(test_location, 1), evaluate(expr2));
}

TEST(ExpressionEval, UnaryBitwiseNot) {
	auto expr = expression(unary_operation(test_location, unary_operator::BITWISE_NOT,
		literal_operand(test_location, 7)));

	EXPECT_EQ(evaluated_expression(test_location, -8), evaluate(expr));
}

TEST(ExpressionEval, Register) {
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
