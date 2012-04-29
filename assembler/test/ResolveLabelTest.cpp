#include <iostream>
#include <list>
#include <gtest/gtest.h>

#include <boost/format.hpp>
#include <ast/Statement.hpp>
#include <SymbolTable.hpp>
#include <ErrorHandler.hpp>

using namespace std;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::lexer;

static ErrorHandlerPtr errorHandler = make_shared<ErrorHandler>();
static Location location("<Test>", 1, 1);

SymbolTablePtr createSymbolTable(map<string, uint16_t> table) {
	SymbolTablePtr symbolTable = make_shared<SymbolTable>();
	for (auto& entry : table) {
		symbolTable->add(Label(location, entry.first), entry.second);
	}

	return symbolTable;
}

TEST(ResolveLabelTest, Expression_LabelOperand) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 31} });

	auto expr = Expression::labelOperand(location, "label2");
	expr->resolveLabels(symbolTable, errorHandler);

	ASSERT_TRUE(expr->isEvaluatable());
	EXPECT_EQ(Expression::evaluatedLiteral(location, 31), expr->evaluate());
}

TEST(ResolveLabelTest, Expression_UnaryOperation) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 31} });

	auto expr = Expression::unaryOperation(location, UnaryOperator::MINUS,
		Expression::labelOperand(location, "label2"));
	expr->resolveLabels(symbolTable, errorHandler);

	ASSERT_TRUE(expr->isEvaluatable());
	EXPECT_EQ(Expression::evaluatedLiteral(location, -31), expr->evaluate());
}

TEST(ResolveLabelTest, Expression_BinaryOperation) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 31} });

	auto expr = Expression::binaryOperation(location, BinaryOperator::MINUS,
		Expression::labelOperand(location, "label2"),
		Expression::labelOperand(location, "label1"));
	expr->resolveLabels(symbolTable, errorHandler);

	ASSERT_TRUE(expr->isEvaluatable());
	EXPECT_EQ(Expression::evaluatedLiteral(location, 21), expr->evaluate());
}