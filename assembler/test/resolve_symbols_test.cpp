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
	compress_expressions compressor(table, pc);
	return apply_visitor(compressor, arg);
}

void resolve(uint16_t pc, symbol_table *table, expression &expr) {
	resolve_symbols resolver(table, test_error_handler, pc);
	apply_visitor(resolver, expr);
}

void resolve(uint16_t pc, symbol_table *table, argument &arg) {
	resolve_symbols resolver(table, test_error_handler, pc);
	apply_visitor(resolver, arg);
}

void add_symbols(symbol_table &table, map<string, uint16_t> labels) {
	for (auto& entry : labels) {
		table.add_label(label(test_location, entry.first), entry.second);
	}
}

TEST(ResolveSymbols, Expression_LabelOperand) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	expression expr(symbol_operand(test_location, "label2"));
	resolve(15, &table, expr);

	ASSERT_TRUE(evaluatable(expr));
	EXPECT_EQ(evaluated_expression(test_location, 31), evaluate(expr));
}

TEST(ResolveSymbols, Expression_CurrentPos) {
	symbol_table table;
	table.add_location(test_location, 15);

	expression expr = expression(current_position_operand(test_location));
	resolve(15, &table, expr);

	ASSERT_TRUE(evaluatable(expr));
	EXPECT_EQ(evaluated_expression(test_location, 15), evaluate(expr));
}

TEST(ResolveSymbols, Expression_UnaryOperation) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	expression expr(unary_operation(test_location, unary_operator::MINUS,
			symbol_operand(test_location, "label2")));
	resolve(15, &table, expr);

	ASSERT_TRUE(evaluatable(expr));
	EXPECT_EQ(evaluated_expression(test_location, -31), evaluate(expr));
}

TEST(ResolveSymbols, Expression_BinaryOperation) {
	symbol_table table;
	add_symbols(table, { {"label1", 10}, {"label2", 31} });

	expression expr(binary_operation(test_location, binary_operator::MINUS,
			symbol_operand(test_location, "label2"),
			symbol_operand(test_location, "label1")));
	resolve(15, &table, expr);

	ASSERT_TRUE(evaluatable(expr));
	EXPECT_EQ(evaluated_expression(test_location, 21), evaluate(expr));
}

static expression create_register_expression(int line, registers _register=registers::A) {
	return evaluated_expression(make_shared<location>("<Test>", line, 7), _register);
}

static expression create_symbol_expression(int line, const string &symbol) {
	return symbol_operand(make_shared<location>("<Test>", line, 8), symbol);
}

static expression create_current_pos_expression(int line) {
	return current_position_operand(make_shared<location>("<Test>", line, 9));
}

static argument create_expression_argument(int line, argument_position position, const expression &expr) {
	int column = position == argument_position::A ? 8 : 5;

	return expression_argument(make_shared<location>("<Test>", line, column), position, expr, false, false);
}

static argument create_indirect_argument(int line, argument_position position, const expression &expr) {
	int column = position == argument_position::A ? 8 : 5;

	return expression_argument(make_shared<location>("<Test>", line, column), position, expr, true, false);
}

static statement create_instruction(int line, const argument &a, const argument &b) {
	return instruction(make_shared<location>("<Test>", line, 1), opcodes::SET, a, b);
}

static statement create_label(int line, const string &name) {
	return label(make_shared<location>("<Test>", line, 1), name);
}

static statement create_no_op(int line) {
	return create_instruction(line,
			create_expression_argument(line, argument_position::A, create_register_expression(line)),
			create_expression_argument(line, argument_position::B, create_register_expression(line)));
}

TEST(SymbolTable, BuildResolve) {
	statement_list statements;
	for (int i = 1; i <= 8; i++) {
		statements.push_back(create_no_op(i));
	}
	statements.push_back(create_instruction(10,
			create_expression_argument(10, argument_position::A, create_symbol_expression(10, "label2")),
			create_expression_argument(10, argument_position::B, create_register_expression(10))));
	// pc=10
	statements.push_back(create_label(11, "label1"));
	statements.push_back(create_instruction(12,
			create_indirect_argument(12, argument_position::A, create_symbol_expression(12, "label2")),
			create_expression_argument(12, argument_position::B, create_register_expression(12))));
	// pc=12
	statements.push_back(create_instruction(13,
				create_expression_argument(13, argument_position::A, create_current_pos_expression(13)),
				create_expression_argument(13, argument_position::B, create_register_expression(13))));
	// pc=14
	statements.push_back(create_instruction(14,
				create_expression_argument(14, argument_position::A, create_symbol_expression(15, "label1")),
				create_expression_argument(14, argument_position::B, create_register_expression(15))));
	// pc=16
	statements.push_back(create_label(11, "label3"));
	statements.push_back(create_instruction(15,
				create_expression_argument(15, argument_position::A, create_register_expression(15)),
				create_expression_argument(15, argument_position::B, create_symbol_expression(15, "label1"))));
	//pc = 18
	for (int i = 0; i <= 12; i++) {
		statements.push_back(create_no_op(16 + i));
	}
	// pc=31
	statements.push_back(create_label(32, "label2"));
	statements.push_back(create_no_op(33));

	symbol_table table;
	table.build(statements, test_error_handler);

	EXPECT_EQ(10, table.lookup("label1", 0)->offset);
	EXPECT_EQ(31, table.lookup("label2", 0)->offset);
	EXPECT_EQ(16, table.lookup("label3", 0)->offset);
	EXPECT_EQ(12, table.lookup(make_shared<location>("<Test>", 13, 9), 0)->offset);

	table.resolve(statements, test_error_handler);

	EXPECT_EQ(9, table.lookup("label1", 0)->offset);
	EXPECT_EQ(28, table.lookup("label2", 0)->offset);
	EXPECT_EQ(13, table.lookup("label3", 0)->offset);
	EXPECT_EQ(11, table.lookup(make_shared<location>("<Test>", 13, 9), 0)->offset);
}
