#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include <statement.hpp>
#include <compiler.hpp>
#include <symbol_table.hpp>
#include <error_handler.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::compiler;
using namespace dcpu::ast;
using namespace dcpu::lexer;
using namespace boost;

static error_handler_ptr test_error_handler = make_shared<dcpu::error_handler>();
static location_ptr test_location = make_shared<location>("<Test>", 1, 1);

bool compress(uint16_t pc, symbol_table *table, argument &arg) {
	compress_expressions compressor(pc, table);
	return apply_visitor(compressor, arg);
}

void resolve(uint16_t pc, symbol_table *table, expression &expr) {
	resolve_symbols resolver(pc, test_error_handler, table);
	apply_visitor(resolver, expr);
}

void resolve(uint16_t pc, symbol_table *table, argument &arg) {
	resolve_symbols resolver(pc, test_error_handler, table);
	apply_visitor(resolver, arg);
}

void add_symbols(symbol_table &table, map<string, uint16_t> labels) {
	for (auto& entry : labels) {
		table.add(label(test_location, entry.first), entry.second);
	}
}

TEST(ResolveSymbolsTest, Expression_LabelOperand) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	expression expr(symbol_operand(test_location, "label2"));
	resolve(15, &table, expr);

	ASSERT_TRUE(evaluatable(expr));
	EXPECT_EQ(expression(evaluated_expression(test_location, 31)), evaluate(expr));
}

TEST(ResolveSymbolsTest, Expression_UnaryOperation) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	expression expr(unary_operation(test_location, unary_operator::MINUS,
			symbol_operand(test_location, "label2")));
	resolve(15, &table, expr);

	ASSERT_TRUE(evaluatable(expr));
	EXPECT_EQ(expression(evaluated_expression(test_location, -31)), evaluate(expr));
}

TEST(ResolveSymbolsTest, Expression_BinaryOperation) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	expression expr(binary_operation(test_location, binary_operator::MINUS,
			symbol_operand(test_location, "label2"),
			symbol_operand(test_location, "label1")));
	resolve(15, &table, expr);

	ASSERT_TRUE(evaluatable(expr));
	EXPECT_EQ(expression(evaluated_expression(test_location, 21)), evaluate(expr));
}

TEST(ResolveSymbolsTest, CompressArgATest) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	argument arg1(expression_argument(test_location, argument_position::A,
			symbol_operand(test_location, "label2"), false, false));
	resolve(8, &table, arg1);

	argument arg2(expression_argument(test_location, argument_position::A,
			symbol_operand(test_location, "label1"), false, false));
	resolve(15, &table, arg2);

	EXPECT_EQ(1, output_size(arg1));
	EXPECT_EQ(1, output_size(arg2));
	EXPECT_EQ(10, *table.lookup("label1", 0));
	EXPECT_EQ(31, *table.lookup("label2", 0));

	// compress round 1
	EXPECT_FALSE(compress(8, &table, arg1));
	EXPECT_TRUE(compress(15, &table, arg2));
	EXPECT_EQ(1, output_size(arg1));
	EXPECT_EQ(0, output_size(arg2));
	EXPECT_EQ(10, *table.lookup("label1", 0));
	EXPECT_EQ(30, *table.lookup("label2", 0));

	// compress round 2
	EXPECT_TRUE(compress(8, &table, arg1));
	EXPECT_FALSE(compress(15, &table, arg2));
	EXPECT_EQ(0, output_size(arg1));
	EXPECT_EQ(0, output_size(arg2));
	EXPECT_EQ(9, *table.lookup("label1", 0));
	EXPECT_EQ(29, *table.lookup("label2", 0));

	// compress round 3
	EXPECT_FALSE(compress(8, &table, arg1));
	EXPECT_FALSE(compress(15, &table, arg2));
	EXPECT_EQ(0, output_size(arg1));
	EXPECT_EQ(0, output_size(arg2));
	EXPECT_EQ(9, *table.lookup("label1", 0));
	EXPECT_EQ(29, *table.lookup("label2", 0));

	EXPECT_EQ(compile_result(0x3e), compile(arg1));
	EXPECT_EQ(compile_result(0x2a), compile(arg2));
}

TEST(ResolveSymbolsTest, CompressArgBTest) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	argument arg1(expression_argument(test_location, argument_position::B,
			symbol_operand(test_location, "label2"), false, false));
	resolve(8, &table, arg1);

	argument arg2(expression_argument(test_location, argument_position::B,
			symbol_operand(test_location, "label1"), false, false));
	resolve(15, &table, arg2);

	EXPECT_EQ(1, output_size(arg1));
	EXPECT_EQ(1, output_size(arg2));
	EXPECT_EQ(10, *table.lookup("label1", 0));
	EXPECT_EQ(31, *table.lookup("label2", 0));

	// compress round 1
	EXPECT_FALSE(compress(8, &table, arg1));
	EXPECT_FALSE(compress(15, &table, arg2));
	EXPECT_EQ(1, output_size(arg1));
	EXPECT_EQ(1, output_size(arg2));
	EXPECT_EQ(10, *table.lookup("label1", 0));
	EXPECT_EQ(31, *table.lookup("label2", 0));

	EXPECT_EQ(compile_result(0x1f, 31), compile(arg1));
	EXPECT_EQ(compile_result(0x1f, 10), compile(arg2));
}

TEST(ResolveSymbolsTest, CompressArgAIndirectTest) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	argument arg1(expression_argument(test_location, argument_position::A,
			symbol_operand(test_location, "label2"), true, false));
	resolve(8, &table, arg1);

	argument arg2(expression_argument(test_location, argument_position::A,
			symbol_operand(test_location, "label1"), true, false));
	resolve(15, &table, arg2);

	EXPECT_EQ(1, output_size(arg1));
	EXPECT_EQ(1, output_size(arg2));
	EXPECT_EQ(10, *table.lookup("label1", 0));
	EXPECT_EQ(31, *table.lookup("label2", 0));

	// compress round 1
	EXPECT_FALSE(compress(8, &table, arg1));
	EXPECT_FALSE(compress(15, &table, arg2));
	EXPECT_EQ(1, output_size(arg1));
	EXPECT_EQ(1, output_size(arg2));
	EXPECT_EQ(10, *table.lookup("label1", 0));
	EXPECT_EQ(31, *table.lookup("label2", 0));

	EXPECT_EQ(compile_result(0x1e, 31), compile(arg1));
	EXPECT_EQ(compile_result(0x1e, 10), compile(arg2));
}
