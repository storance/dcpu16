#include <gtest/gtest.h>

#include <boost/variant.hpp>
#include <expression.hpp>

using namespace std;
using namespace dcpu::assembler;

static dcpu::assembler::log logger;
static location_ptr _location = make_shared<location>("<Test>", 1, 1);

TEST(ExpressionEval, Add) {
	auto expr = expression(binary_operation(_location, binary_operator::PLUS,
		literal_operand(_location, 4),
		literal_operand(_location, 9)));

	EXPECT_EQ(evaluated_expression(_location, 13), evaluate(logger, expr));
}

TEST(ExpressionEval, Subtract) {
	auto expr = expression(binary_operation(_location, binary_operator::MINUS,
		literal_operand(_location, 4),
		literal_operand(_location, 9)));

	EXPECT_EQ(evaluated_expression(_location, -5), evaluate(logger, expr));
}

TEST(ExpressionEval, Multiply) {
	auto expr = expression(binary_operation(_location, binary_operator::MULTIPLY,
		literal_operand(_location, 2),
		literal_operand(_location, 4)));

	EXPECT_EQ(evaluated_expression(_location, 8), evaluate(logger, expr));
}

TEST(ExpressionEval, Divide) {
	auto expr = expression(binary_operation(_location, binary_operator::DIVIDE,
		literal_operand(_location, 8),
		literal_operand(_location, 2)));

	EXPECT_EQ(evaluated_expression(_location, 4), evaluate(logger, expr));

	auto expr2 = expression(binary_operation(_location, binary_operator::DIVIDE,
		literal_operand(_location, 15),
		literal_operand(_location, 4)));

	EXPECT_EQ(evaluated_expression(_location, 3), evaluate(logger, expr2));
}

TEST(ExpressionEval, DivideByZero) {
	stringstream out;
	dcpu::assembler::log capture_log(out);

	auto expr = expression(binary_operation(_location, binary_operator::DIVIDE,
		literal_operand(_location, 8),
		literal_operand(_location, 0)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(capture_log, expr, true));
	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(capture_log, expr));
	EXPECT_EQ("<Test>:1:1: warning: divide by zero\n", out.str());
}

TEST(ExpressionEval, ModuloByZero) {
	stringstream out;
	dcpu::assembler::log capture_log(out);

	auto expr = expression(binary_operation(_location, binary_operator::MODULO,
		literal_operand(_location, 8),
		literal_operand(_location, 0)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(capture_log, expr, true));
	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(capture_log, expr));
	EXPECT_EQ("<Test>:1:1: warning: modulo by zero\n", out.str());
}

TEST(ExpressionEval, Modulo) {
	auto expr = expression(binary_operation(_location, binary_operator::MODULO,
		literal_operand(_location, 19),
		literal_operand(_location, 5)));

	EXPECT_EQ(evaluated_expression(_location, 4), evaluate(logger, expr));
}

TEST(ExpressionEval, ShiftLeft) {
	auto expr = expression(binary_operation(_location, binary_operator::SHIFT_LEFT,
		literal_operand(_location, 1),
		literal_operand(_location, 4)));

	EXPECT_EQ(evaluated_expression(_location, 16), evaluate(logger, expr));
}

TEST(ExpressionEval, ShiftRight) {
	auto expr = expression(binary_operation(_location, binary_operator::SHIFT_RIGHT,
		literal_operand(_location, 256),
		literal_operand(_location, 2)));

	EXPECT_EQ(evaluated_expression(_location, 64), evaluate(logger, expr));
}

TEST(ExpressionEval, BitwiseAnd) {
	auto expr = expression(binary_operation(_location, binary_operator::BITWISE_AND,
		literal_operand(_location, 0x9),
		literal_operand(_location, 0xc)));

	EXPECT_EQ(evaluated_expression(_location, 0x8), evaluate(logger, expr));
}

TEST(ExpressionEval, BitwiseXor) {
	auto expr = expression(binary_operation(_location, binary_operator::BITWISE_XOR,
		literal_operand(_location, 0x9),
		literal_operand(_location, 0xc)));

	EXPECT_EQ(evaluated_expression(_location, 0x5), evaluate(logger, expr));
}

TEST(ExpressionEval, BitwiseOr) {
	auto expr = expression(binary_operation(_location, binary_operator::BITWISE_OR,
		literal_operand(_location, 0x9),
		literal_operand(_location, 0xc)));

	EXPECT_EQ(evaluated_expression(_location, 0xd), evaluate(logger, expr));
}

TEST(ExpressionEval, Equals) {
	expression expr1(binary_operation(_location, binary_operator::EQ,
		literal_operand(_location, 19),
		literal_operand(_location, 19)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr1));

	expression expr2(binary_operation(_location, binary_operator::EQ,
		literal_operand(_location, 43),
		literal_operand(_location, 17)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr2));
}

TEST(ExpressionEval, NotEquals) {
	expression expr1(binary_operation(_location, binary_operator::NEQ,
		literal_operand(_location, 19),
		literal_operand(_location, 10)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr1));

	expression expr2(binary_operation(_location, binary_operator::NEQ,
		literal_operand(_location, 43),
		literal_operand(_location, 43)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr2));
}

TEST(ExpressionEval, LessThan) {
	expression expr1(binary_operation(_location, binary_operator::LT,
		literal_operand(_location, 8),
		literal_operand(_location, 19)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr1));

	expression expr2(binary_operation(_location, binary_operator::LT,
		literal_operand(_location, 43),
		literal_operand(_location, 17)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr2));

	expression expr3(binary_operation(_location, binary_operator::LT,
		literal_operand(_location, 17),
		literal_operand(_location, 17)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr3));
}

TEST(ExpressionEval, GreaterThan) {
	expression expr1(binary_operation(_location, binary_operator::GT,
		literal_operand(_location, 19),
		literal_operand(_location, 8)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr1));

	expression expr2(binary_operation(_location, binary_operator::GT,
		literal_operand(_location, 17),
		literal_operand(_location, 43)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr2));

	expression expr3(binary_operation(_location, binary_operator::GT,
		literal_operand(_location, 17),
		literal_operand(_location, 17)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr3));
}

TEST(ExpressionEval, LessThanOrEqual) {
	expression expr1(binary_operation(_location, binary_operator::LTE,
		literal_operand(_location, 8),
		literal_operand(_location, 19)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr1));

	expression expr2(binary_operation(_location, binary_operator::LTE,
		literal_operand(_location, 43),
		literal_operand(_location, 17)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr2));

	expression expr3(binary_operation(_location, binary_operator::LTE,
		literal_operand(_location, 17),
		literal_operand(_location, 17)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr3));
}

TEST(ExpressionEval, GreaterThanOrEqual) {
	expression expr1(binary_operation(_location, binary_operator::GTE,
		literal_operand(_location, 19),
		literal_operand(_location, 8)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr1));

	expression expr2(binary_operation(_location, binary_operator::GTE,
		literal_operand(_location, 17),
		literal_operand(_location, 43)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr2));

	expression expr3(binary_operation(_location, binary_operator::GTE,
		literal_operand(_location, 17),
		literal_operand(_location, 17)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr3));
}

TEST(ExpressionEval, BooleanAnd) {
	expression expr1(binary_operation(_location, binary_operator::AND,
		literal_operand(_location, 1),
		literal_operand(_location, 1)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr1));

	expression expr2(binary_operation(_location, binary_operator::AND,
		literal_operand(_location, 0),
		literal_operand(_location, 1)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr2));

	expression expr3(binary_operation(_location, binary_operator::AND,
		literal_operand(_location, 1),
		literal_operand(_location, 0)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr3));

	expression expr4(binary_operation(_location, binary_operator::AND,
		literal_operand(_location, 0),
		literal_operand(_location, 0)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr4));
}

TEST(ExpressionEval, BooleanOr) {
	expression expr1(binary_operation(_location, binary_operator::OR,
		literal_operand(_location, 1),
		literal_operand(_location, 1)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr1));

	expression expr2(binary_operation(_location, binary_operator::OR,
		literal_operand(_location, 0),
		literal_operand(_location, 1)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr2));

	expression expr3(binary_operation(_location, binary_operator::OR,
		literal_operand(_location, 1),
		literal_operand(_location, 0)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr3));

	expression expr4(binary_operation(_location, binary_operator::OR,
		literal_operand(_location, 0),
		literal_operand(_location, 0)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr4));
}

TEST(ExpressionEval, UnaryMinus) {
	auto expr = expression(unary_operation(_location, unary_operator::MINUS,
		literal_operand(_location, 7)));

	EXPECT_EQ(evaluated_expression(_location, -7), evaluate(logger, expr));
}

TEST(ExpressionEval, UnaryPlus) {
	auto expr = expression(unary_operation(_location, unary_operator::PLUS,
		literal_operand(_location, 7)));

	EXPECT_EQ(evaluated_expression(_location, 7), evaluate(logger, expr));
}

TEST(ExpressionEval, UnaryNot) {
	auto expr = expression(unary_operation(_location, unary_operator::NOT,
		literal_operand(_location, 7)));

	EXPECT_EQ(evaluated_expression(_location, 0), evaluate(logger, expr));

	auto expr2 = expression(unary_operation(_location, unary_operator::NOT,
		literal_operand(_location, 0)));

	EXPECT_EQ(evaluated_expression(_location, 1), evaluate(logger, expr2));
}

TEST(ExpressionEval, UnaryBitwiseNot) {
	auto expr = expression(unary_operation(_location, unary_operator::BITWISE_NOT,
		literal_operand(_location, 7)));

	EXPECT_EQ(evaluated_expression(_location, -8), evaluate(logger, expr));
}

TEST(ExpressionEval, Register) {
	auto expr = expression(binary_operation(_location, binary_operator::PLUS,
		register_operand(_location, registers::A),
		literal_operand(_location, 10)));
	EXPECT_EQ(evaluated_expression(_location, registers::A, 10), evaluate(logger, expr));

	auto expr2 = expression(binary_operation(_location, binary_operator::PLUS,
		literal_operand(_location, 10),
		register_operand(_location, registers::B)));
	EXPECT_EQ(evaluated_expression(_location, registers::B, 10), evaluate(logger, expr2));

	auto expr3 = expression(binary_operation(_location, binary_operator::MINUS,
		register_operand(_location, registers::C),
		literal_operand(_location, 10)));
	EXPECT_EQ(evaluated_expression(_location, registers::C, -10), evaluate(logger, expr3));

	auto expr4 = expression(register_operand(_location, registers::X));
	EXPECT_EQ(evaluated_expression(_location, registers::X), evaluate(logger, expr4));

	auto expr5 = expression(binary_operation(_location, binary_operator::PLUS,
		register_operand(_location, registers::A),
		binary_operation(_location, binary_operator::MULTIPLY,
			literal_operand(_location, 2),
			literal_operand(_location, 4))));
	EXPECT_EQ(evaluated_expression(_location, registers::A, 8), evaluate(logger, expr5));
}
