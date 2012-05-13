#include <iostream>
#include <list>
#include <gtest/gtest.h>
#include <boost/variant.hpp>

#include <expression_parser.hpp>
#include <lexer.hpp>

using namespace std;
using namespace std::placeholders;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::parser;
using namespace dcpu::lexer;

expression run_expression_parser(const string &content, bool indirect, bool allow_registers) {
	lexer::lexer lex(content, "<Test>");
	lex.parse();

	auto begin = lex.tokens.begin();
	expression_parser parser(begin, lex.tokens.end(), lex.error_handler, allow_registers, true, indirect);
	return parser.parse(next(begin, lex.tokens.end()));
}

TEST(ExpressionParserTest, OperatorPrecedenceTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);

	expression expr = run_expression_parser("1 + 2 * 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::PLUS,
		literal_operand(location, 1),
		binary_operation(location, binary_operator::MULTIPLY,
			literal_operand(location, 2),
			literal_operand(location, 3)
		)
	)));

	expr = run_expression_parser("1 & 2 | 3 ^ 4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::OR,
		binary_operation(location, binary_operator::AND,
			literal_operand(location, 1),
			literal_operand(location, 2)
		), binary_operation(location, binary_operator::XOR,
			literal_operand(location, 3),
			literal_operand(location, 4)
		)
	)));

	expr = run_expression_parser("1 << 4 & 2 >> 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::AND,
		binary_operation(location, binary_operator::SHIFT_LEFT,
			literal_operand(location, 1),
			literal_operand(location, 4)
		), binary_operation(location, binary_operator::SHIFT_RIGHT,
			literal_operand(location, 2),
			literal_operand(location, 3)
		)
	)));

	expr = run_expression_parser("1 + 2 << 3 >> 4 - 5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::SHIFT_RIGHT,
		binary_operation(location, binary_operator::SHIFT_LEFT,
			binary_operation(location, binary_operator::PLUS,
				literal_operand(location, 1),
				literal_operand(location, 2)
			), literal_operand(location, 3)
		), binary_operation(location, binary_operator::MINUS,
			literal_operand(location, 4),
			literal_operand(location, 5)
		)
	)));

	expr = run_expression_parser("1 * 2 + 3 / 4 % 5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::PLUS,
		binary_operation(location, binary_operator::MULTIPLY,
			literal_operand(location, 1),
			literal_operand(location, 2)
		), binary_operation(location, binary_operator::MODULO,
			binary_operation(location, binary_operator::DIVIDE,
				literal_operand(location, 3),
				literal_operand(location, 4)
			), literal_operand(location, 5)
		)
	)));

	expr = run_expression_parser("-1 * !-2 / +3 % ~4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::MODULO,
		binary_operation(location, binary_operator::DIVIDE,
			binary_operation(location, binary_operator::MULTIPLY,
				unary_operation(location, unary_operator::MINUS, literal_operand(location, 1)),
				unary_operation(location, unary_operator::NOT,
					unary_operation(location, unary_operator::MINUS, literal_operand(location, 2))
				)
			), unary_operation(location, unary_operator::PLUS, literal_operand(location, 3))
		), unary_operation(location, unary_operator::BITWISE_NOT, literal_operand(location, 4))
	)));

	expr = run_expression_parser("-(1 + 2) * (3 + 4)", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::MULTIPLY,
		unary_operation(location, unary_operator::MINUS,
			binary_operation(location, binary_operator::PLUS,
				literal_operand(location, 1),
				literal_operand(location, 2)
			)
		), binary_operation(location, binary_operator::PLUS,
			literal_operand(location, 3),
			literal_operand(location, 4)
		)
	)));
}

TEST(ExpressionParserTest, SymbolTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);

	expression expr = run_expression_parser("$a + 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::PLUS,
		symbol_operand(location, "a"),
		literal_operand(location, 3))
	));

	expr = run_expression_parser("label + 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::PLUS,
		symbol_operand(location, "label"),
		literal_operand(location, 3))
	));
}

TEST(ExpressionParserTest, RegisterTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);

	expression expr = run_expression_parser("a + 3", true, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::PLUS,
		register_operand(location, registers::A),
		literal_operand(location, 3))
	));
}

TEST(ExpressionParserTest, CurrnetPosTest) {
	location_ptr location = make_shared<lexer::location>("<Test>", 1, 1);

	expression expr = run_expression_parser("$ + 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(location, binary_operator::PLUS,
		current_position_operand(location),
		literal_operand(location, 3))
	));
}
