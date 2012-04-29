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

TEST(ResolveLabelTest, ArgumentA_NoIndirect_Short_LabelOperand) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 30} });

	auto arg = Argument::expression(ArgumentPosition::A,
			Expression::labelOperand(location, "label2"));
	arg->resolveLabels(symbolTable, errorHandler, 10);

	EXPECT_FALSE(arg->isNextWordRequired());
	EXPECT_EQ(10, *symbolTable->lookup("label1"));
	EXPECT_EQ(29, *symbolTable->lookup("label2"));
}

TEST(ResolveLabelTest, ArgumentA_NoIndirect_BinaryOperation_PCInBetween) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 30} });

	auto arg1 = Argument::expression(ArgumentPosition::A,
		Expression::binaryOperation(location, BinaryOperator::MINUS,
			Expression::labelOperand(location, "label2"),
			Expression::labelOperand(location, "label1")));
	arg1->resolveLabels(symbolTable, errorHandler, 15);

	EXPECT_FALSE(arg1->isNextWordRequired());
	EXPECT_EQ(10, *symbolTable->lookup("label1"));
	EXPECT_EQ(29, *symbolTable->lookup("label2"));
}

TEST(ResolveLabelTest, ArgumentA_NoIndirect_BinaryOperation_PCBefore) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 30} });

	auto arg1 = Argument::expression(ArgumentPosition::A,
		Expression::binaryOperation(location, BinaryOperator::MINUS,
			Expression::labelOperand(location, "label2"),
			Expression::labelOperand(location, "label1")));
	arg1->resolveLabels(symbolTable, errorHandler, 8);

	EXPECT_FALSE(arg1->isNextWordRequired());
	EXPECT_EQ(9, *symbolTable->lookup("label1"));
	EXPECT_EQ(29, *symbolTable->lookup("label2"));
}

TEST(ResolveLabelTest, ArgumentA_Indirect_Short_LabelOperand) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 30} });

	auto arg = Argument::indirect(ArgumentPosition::A,
			Expression::labelOperand(location, "label2"));
	arg->resolveLabels(symbolTable, errorHandler, 10);

	EXPECT_TRUE(arg->isNextWordRequired());
	EXPECT_EQ(10, *symbolTable->lookup("label1"));
	EXPECT_EQ(30, *symbolTable->lookup("label2"));
}

TEST(ResolveLabelTest, ArgumentB_NoIndirect_Short_LabelOperand) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 30} });

	auto arg = Argument::indirect(ArgumentPosition::B,
			Expression::labelOperand(location, "label2"));
	arg->resolveLabels(symbolTable, errorHandler, 10);

	EXPECT_TRUE(arg->isNextWordRequired());
	EXPECT_EQ(10, *symbolTable->lookup("label1"));
	EXPECT_EQ(30, *symbolTable->lookup("label2"));
}

TEST(ResolveLabelTest, ArgumentA_NoIndirect_Long_LabelOperand) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 31} });

	auto arg = Argument::expression(ArgumentPosition::A,
			Expression::labelOperand(location, "label2"));
	arg->resolveLabels(symbolTable, errorHandler, 10);

	EXPECT_TRUE(arg->isNextWordRequired());
	EXPECT_EQ(10, *symbolTable->lookup("label1"));
	EXPECT_EQ(31, *symbolTable->lookup("label2"));
}

TEST(ResolveLabelTest, CompressTest) {
	SymbolTablePtr symbolTable = createSymbolTable({ {"label1", 10}, {"label2", 31} });

	auto arg1 = Argument::expression(ArgumentPosition::A,
			Expression::labelOperand(location, "label2"));
	arg1->resolveLabels(symbolTable, errorHandler, 8);

	auto arg2 = Argument::expression(ArgumentPosition::A,
			Expression::labelOperand(location, "label1"));
	arg2->resolveLabels(symbolTable, errorHandler, 15);

	EXPECT_TRUE(arg1->isNextWordRequired());
	EXPECT_FALSE(arg2->isNextWordRequired());
	EXPECT_EQ(10, *symbolTable->lookup("label1"));
	EXPECT_EQ(30, *symbolTable->lookup("label2"));

	// compress round 1
	EXPECT_TRUE(arg1->compress(symbolTable, 8));
	EXPECT_FALSE(arg2->compress(symbolTable, 15));
	EXPECT_FALSE(arg1->isNextWordRequired());
	EXPECT_FALSE(arg2->isNextWordRequired());
	EXPECT_EQ(9, *symbolTable->lookup("label1"));
	EXPECT_EQ(29, *symbolTable->lookup("label2"));

	// compress round 2
	EXPECT_FALSE(arg1->compress(symbolTable, 8));
	EXPECT_FALSE(arg2->compress(symbolTable, 14));
	EXPECT_FALSE(arg1->isNextWordRequired());
	EXPECT_FALSE(arg2->isNextWordRequired());
	EXPECT_EQ(9, *symbolTable->lookup("label1"));
	EXPECT_EQ(29, *symbolTable->lookup("label2"));

	EXPECT_EQ(CompileResult(0x3e, boost::none), arg1->compile());
	EXPECT_EQ(CompileResult(0x2a, boost::none), arg2->compile());
}