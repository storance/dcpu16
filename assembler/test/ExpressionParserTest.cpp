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

void runParser(const string &content, ExpressionPtr &expr, bool insideIndirect,
	bool allowRegisters) {

	Lexer lexer(content.begin(), content.end(), "<ExpressionParserTest>");
	lexer.parse();

	ErrorHandler errorHandler;

	auto begin = lexer.tokens.begin();
	ExpressionParser parser(begin, lexer.tokens.end(), errorHandler, insideIndirect, allowRegisters);
	expr = move(parser.parse());
}

TEST(ParserTest, OperatorPrecedenceTest) {
	ExpressionPtr expr;

	ASSERT_NO_FATAL_FAILURE(runParser("1 + 2 * 3", expr, false, true));
	{
		SCOPED_TRACE("Expression 1 + 2 * 3");
		assertIsBinaryOperation(BinaryOperator::PLUS,
			assertIsLiteral(1),
			assertIsBinaryOperation(BinaryOperator::MULTIPLY, 
				assertIsLiteral(2),
				assertIsLiteral(3)
			)
		)(expr);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("1 & 2 | 3 ^ 4", expr, false, true));
	{
		SCOPED_TRACE("Expression 1 & 2 | 3 ^ 4");
		assertIsBinaryOperation(BinaryOperator::OR,
			assertIsBinaryOperation(BinaryOperator::AND,
				assertIsLiteral(1),
				assertIsLiteral(2)
			), assertIsBinaryOperation(BinaryOperator::XOR, 
				assertIsLiteral(3),
				assertIsLiteral(4)
			)
		)(expr);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("1 << 4 & 2 >> 3", expr, false, true));
	{
		SCOPED_TRACE("Expression 1 << 4 & 2 >> 3");
		assertIsBinaryOperation(BinaryOperator::AND,
			assertIsBinaryOperation(BinaryOperator::SHIFT_LEFT,
				assertIsLiteral(1),
				assertIsLiteral(4)
			), assertIsBinaryOperation(BinaryOperator::SHIFT_RIGHT, 
				assertIsLiteral(2),
				assertIsLiteral(3)
			)
		)(expr);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("1 + 2 << 3 >> 4 - 5", expr, false, true));
	{
		SCOPED_TRACE("Expression 1 + 2 << 3 >> 4 - 5");
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
		)(expr);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("1 * 2 + 3 / 4 % 5", expr, false, true));
	{
		SCOPED_TRACE("Expression 1 * 2 + 3 / 4 % 5");
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
		)(expr);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("-1 * !-2 / +3 % ~4", expr, false, true));
	{
		SCOPED_TRACE("Expression -1 * !-2 / +3 % ~4");
		assertIsBinaryOperation(BinaryOperator::MODULO,
			assertIsBinaryOperation(BinaryOperator::DIVIDE,
				assertIsBinaryOperation(BinaryOperator::MULTIPLY,
					assertIsUnaryOperation(UnaryOperator::MINUS, assertIsLiteral(1)),
					assertIsUnaryOperation(UnaryOperator::NOT,
						assertIsUnaryOperation(UnaryOperator::MINUS, assertIsLiteral(2)))
				), assertIsUnaryOperation(UnaryOperator::PLUS, assertIsLiteral(3))
			), assertIsUnaryOperation(UnaryOperator::BITWISE_NOT, assertIsLiteral(4))
		)(expr);
	}

	ASSERT_NO_FATAL_FAILURE(runParser("-(1 + 2) * (3 + 4)", expr, false, true));
	{
		SCOPED_TRACE("Expression -(1 + 2) * (3 + 4)");
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
		)(expr);
	}
}