#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include "Utils.hpp"
#include "../ExpressionParser.hpp"

using namespace std;
using namespace std::placeholders;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::parser;
using namespace dcpu::lexer;

TEST(ExpressionParserTest, OperatorPrecedenceTest) {
	Location location("<Test>", 1, 1);
	ExpressionPtr expr;

	ASSERT_NO_FATAL_FAILURE(runExpressionParser("1 + 2 * 3", expr, false, true));
	EXPECT_EQ(expr, Expression::binaryOperation(location, BinaryOperator::PLUS,
		Expression::literalOperand(location, 1),
		Expression::binaryOperation(location, BinaryOperator::MULTIPLY,
			Expression::literalOperand(location, 2),
			Expression::literalOperand(location, 3)
		)
	));

	ASSERT_NO_FATAL_FAILURE(runExpressionParser("1 & 2 | 3 ^ 4", expr, false, true));
	EXPECT_EQ(expr, Expression::binaryOperation(location, BinaryOperator::OR,
		Expression::binaryOperation(location, BinaryOperator::AND,
			Expression::literalOperand(location, 1),
			Expression::literalOperand(location, 2)
		), Expression::binaryOperation(location, BinaryOperator::XOR,
			Expression::literalOperand(location, 3),
			Expression::literalOperand(location, 4)
		)
	));

	ASSERT_NO_FATAL_FAILURE(runExpressionParser("1 << 4 & 2 >> 3", expr, false, true));
	EXPECT_EQ(expr, Expression::binaryOperation(location, BinaryOperator::AND,
		Expression::binaryOperation(location, BinaryOperator::SHIFT_LEFT,
			Expression::literalOperand(location, 1),
			Expression::literalOperand(location, 4)
		), Expression::binaryOperation(location, BinaryOperator::SHIFT_RIGHT,
			Expression::literalOperand(location, 2),
			Expression::literalOperand(location, 3)
		)
	));

	ASSERT_NO_FATAL_FAILURE(runExpressionParser("1 + 2 << 3 >> 4 - 5", expr, false, true));
	EXPECT_EQ(expr, Expression::binaryOperation(location, BinaryOperator::SHIFT_RIGHT,
		Expression::binaryOperation(location, BinaryOperator::SHIFT_LEFT,
			Expression::binaryOperation(location, BinaryOperator::PLUS,
				Expression::literalOperand(location, 1),
				Expression::literalOperand(location, 2)
			), Expression::literalOperand(location, 3)
		), Expression::binaryOperation(location, BinaryOperator::MINUS,
			Expression::literalOperand(location, 4),
			Expression::literalOperand(location, 5)
		)
	));

	ASSERT_NO_FATAL_FAILURE(runExpressionParser("1 * 2 + 3 / 4 % 5", expr, false, true));
	EXPECT_EQ(expr, Expression::binaryOperation(location, BinaryOperator::PLUS,
		Expression::binaryOperation(location, BinaryOperator::MULTIPLY,
			Expression::literalOperand(location, 1),
			Expression::literalOperand(location, 2)
		), Expression::binaryOperation(location, BinaryOperator::MODULO,
			Expression::binaryOperation(location, BinaryOperator::DIVIDE,
				Expression::literalOperand(location, 3),
				Expression::literalOperand(location, 4)
			), Expression::literalOperand(location, 5)
		)
	));

	ASSERT_NO_FATAL_FAILURE(runExpressionParser("-1 * !-2 / +3 % ~4", expr, false, true));
	EXPECT_EQ(expr, Expression::binaryOperation(location, BinaryOperator::MODULO,
		Expression::binaryOperation(location, BinaryOperator::DIVIDE,
			Expression::binaryOperation(location, BinaryOperator::MULTIPLY,
				Expression::unaryOperation(location, UnaryOperator::MINUS, Expression::literalOperand(location, 1)),
				Expression::unaryOperation(location, UnaryOperator::NOT, 
					Expression::unaryOperation(location, UnaryOperator::MINUS, Expression::literalOperand(location, 2))
				)
			), Expression::unaryOperation(location, UnaryOperator::PLUS, Expression::literalOperand(location, 3))
		), Expression::unaryOperation(location, UnaryOperator::BITWISE_NOT, Expression::literalOperand(location, 4))
	));

	ASSERT_NO_FATAL_FAILURE(runExpressionParser("-(1 + 2) * (3 + 4)", expr, false, true));
	EXPECT_EQ(expr, Expression::binaryOperation(location, BinaryOperator::MULTIPLY,
		Expression::unaryOperation(location, UnaryOperator::MINUS,
			Expression::binaryOperation(location, BinaryOperator::PLUS,
				Expression::literalOperand(location, 1),
				Expression::literalOperand(location, 2)
			)
		), Expression::binaryOperation(location, BinaryOperator::PLUS,
			Expression::literalOperand(location, 3),
			Expression::literalOperand(location, 4)
		)
	));
}