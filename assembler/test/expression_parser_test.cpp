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
	logging::log logger;
	lexer::lexer lex(content, "<Test>", logger);
	lex.parse();

	auto begin = lex.tokens.begin();
	expression_parser parser(begin, lex.tokens.end(), lex.logger, allow_registers, true, indirect);
	return parser.parse(next(begin, lex.tokens.end()));
}

TEST(ExpressionParser, Operators) {
	location_ptr _location = make_shared<location>("<Test>", 1, 1);

	expression expr = run_expression_parser("2^5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::BITWISE_XOR,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2||5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::OR,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2&&5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::AND,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2==5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::EQ,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2!=5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::NEQ,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2<>5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::EQ,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2<=5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::LTE,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2<5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::LT,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2>=5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::GTE,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2>5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::GT,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2|5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::BITWISE_OR,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2&5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::BITWISE_AND,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2+5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::PLUS,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2-5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::MINUS,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2*5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::MULTIPLY,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2/5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::DIVIDE,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2%5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::MODULO,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2<<5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::SHIFT_LEFT,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("2>>5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::SHIFT_RIGHT,
		literal_operand(_location, 2),
		literal_operand(_location, 5))
	));

	expr = run_expression_parser("-2", false, true);
	EXPECT_EQ(expr, expression(unary_operation(_location, unary_operator::MINUS,
		literal_operand(_location, 2))
	));

	expr = run_expression_parser("+2", false, true);
	EXPECT_EQ(expr, expression(unary_operation(_location, unary_operator::PLUS,
		literal_operand(_location, 2))
	));

	expr = run_expression_parser("!2", false, true);
	EXPECT_EQ(expr, expression(unary_operation(_location, unary_operator::NOT,
		literal_operand(_location, 2))
	));

	expr = run_expression_parser("~2", false, true);
	EXPECT_EQ(expr, expression(unary_operation(_location, unary_operator::BITWISE_NOT,
		literal_operand(_location, 2))
	));

	expr = run_expression_parser("--2", false, true);
	EXPECT_EQ(expr, expression(unary_operation(_location, unary_operator::MINUS,
		unary_operation(_location, unary_operator::MINUS, literal_operand(_location, 2)))
	));
}

TEST(ExpressionParser, OperatorPrecedence) {
	location_ptr _location = make_shared<location>("<Test>", 1, 1);

	expression expr = run_expression_parser("1 && 2 || 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::OR,
		binary_operation(_location, binary_operator::AND,
			literal_operand(_location, 1),
			literal_operand(_location, 2)
		), literal_operand(_location, 3)
	)));

	expr = run_expression_parser("1 | 2 && 3 | 4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::BITWISE_AND,
		binary_operation(_location, binary_operator::BITWISE_OR,
			literal_operand(_location, 1),
			literal_operand(_location, 2)
		), binary_operation(_location, binary_operator::BITWISE_OR,
			literal_operand(_location, 3),
			literal_operand(_location, 4)
		)
	)));

	expr = run_expression_parser("1 ^ 2 | 3 ^ 4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::BITWISE_OR,
		binary_operation(_location, binary_operator::BITWISE_XOR,
			literal_operand(_location, 1),
			literal_operand(_location, 2)
		), binary_operation(_location, binary_operator::BITWISE_XOR,
			literal_operand(_location, 3),
			literal_operand(_location, 4)
		)
	)));

	expr = run_expression_parser("1 & 2 ^ 3 & 4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::BITWISE_XOR,
		binary_operation(_location, binary_operator::BITWISE_AND,
			literal_operand(_location, 1),
			literal_operand(_location, 2)
		), binary_operation(_location, binary_operator::BITWISE_AND,
			literal_operand(_location, 3),
			literal_operand(_location, 4)
		)
	)));

	expr = run_expression_parser("1 == 2 & 3 != 4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::BITWISE_AND,
		binary_operation(_location, binary_operator::EQ,
			literal_operand(_location, 1),
			literal_operand(_location, 2)
		), binary_operation(_location, binary_operator::NEQ,
			literal_operand(_location, 3),
			literal_operand(_location, 4)
		)
	)));

	expr = run_expression_parser("1 <= 2 == 3 >= 4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::EQ,
		binary_operation(_location, binary_operator::LTE,
			literal_operand(_location, 1),
			literal_operand(_location, 2)
		), binary_operation(_location, binary_operator::GTE,
			literal_operand(_location, 3),
			literal_operand(_location, 4)
		)
	)));

	expr = run_expression_parser("1 < 2 == 3 > 4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::EQ,
		binary_operation(_location, binary_operator::LT,
			literal_operand(_location, 1),
			literal_operand(_location, 2)
		), binary_operation(_location, binary_operator::GT,
			literal_operand(_location, 3),
			literal_operand(_location, 4)
		)
	)));


	expr = run_expression_parser("1 << 4 > 2 >> 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::GT,
		binary_operation(_location, binary_operator::SHIFT_LEFT,
			literal_operand(_location, 1),
			literal_operand(_location, 4)
		), binary_operation(_location, binary_operator::SHIFT_RIGHT,
			literal_operand(_location, 2),
			literal_operand(_location, 3)
		)
	)));

	expr = run_expression_parser("1 + 2 << 3 >> 4 - 5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::SHIFT_RIGHT,
		binary_operation(_location, binary_operator::SHIFT_LEFT,
			binary_operation(_location, binary_operator::PLUS,
				literal_operand(_location, 1),
				literal_operand(_location, 2)
			), literal_operand(_location, 3)
		), binary_operation(_location, binary_operator::MINUS,
			literal_operand(_location, 4),
			literal_operand(_location, 5)
		)
	)));

	expr = run_expression_parser("1 * 2 + 3 / 4 % 5", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::PLUS,
		binary_operation(_location, binary_operator::MULTIPLY,
			literal_operand(_location, 1),
			literal_operand(_location, 2)
		), binary_operation(_location, binary_operator::MODULO,
			binary_operation(_location, binary_operator::DIVIDE,
				literal_operand(_location, 3),
				literal_operand(_location, 4)
			), literal_operand(_location, 5)
		)
	)));

	expr = run_expression_parser("-1 * !-2 / +3 % ~4", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::MODULO,
		binary_operation(_location, binary_operator::DIVIDE,
			binary_operation(_location, binary_operator::MULTIPLY,
				unary_operation(_location, unary_operator::MINUS, literal_operand(_location, 1)),
				unary_operation(_location, unary_operator::NOT,
					unary_operation(_location, unary_operator::MINUS, literal_operand(_location, 2))
				)
			), unary_operation(_location, unary_operator::PLUS, literal_operand(_location, 3))
		), unary_operation(_location, unary_operator::BITWISE_NOT, literal_operand(_location, 4))
	)));

	expr = run_expression_parser("5--10", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::MINUS,
			literal_operand(_location, 5),
			unary_operation(_location, unary_operator::MINUS,
					literal_operand(_location, 10)
			)
	)));

	expr = run_expression_parser("-(1 + 2) * (3 + 4)", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::MULTIPLY,
		unary_operation(_location, unary_operator::MINUS,
			binary_operation(_location, binary_operator::PLUS,
				literal_operand(_location, 1),
				literal_operand(_location, 2)
			)
		), binary_operation(_location, binary_operator::PLUS,
			literal_operand(_location, 3),
			literal_operand(_location, 4)
		)
	)));
}

TEST(ExpressionParser, Symbols) {
	location_ptr _location = make_shared<location>("<Test>", 1, 1);

	expression expr = run_expression_parser("$a + 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::PLUS,
		symbol_operand(_location, "a"),
		literal_operand(_location, 3))
	));

	expr = run_expression_parser("label + 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::PLUS,
		symbol_operand(_location, "label"),
		literal_operand(_location, 3))
	));
}

TEST(ExpressionParser, Registers) {
	location_ptr _location = make_shared<location>("<Test>", 1, 1);

	expression expr = run_expression_parser("a + 3", true, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::PLUS,
		register_operand(_location, registers::A),
		literal_operand(_location, 3))
	));
}

TEST(ExpressionParser, CurrentPosition) {
	location_ptr _location = make_shared<location>("<Test>", 1, 1);

	expression expr = run_expression_parser("$ + 3", false, true);
	EXPECT_EQ(expr, expression(binary_operation(_location, binary_operator::PLUS,
		current_position_operand(_location),
		literal_operand(_location, 3))
	));
}
