#include "Utils.hpp"

#include <gtest/gtest.h>

using namespace std;;
using namespace dcpu;
using namespace dcpu::ast;

ExpressionFunc assertIsBinaryOperation(BinaryOperator expectedOperation,
	ExpressionFunc assertLeft, ExpressionFunc assertRight) {

	return [=] (ExpressionPtr& expr) {
		BinaryOperation* binaryOp = dynamic_cast<BinaryOperation*>(expr.get());

		ASSERT_TRUE(binaryOp != nullptr);
		EXPECT_EQ(expectedOperation, binaryOp->_operator);
		{
			SCOPED_TRACE("Verify left-hand expression");
			assertLeft(binaryOp->_left);
		}
		{
			SCOPED_TRACE("Verify right-hand expression");
			assertRight(binaryOp->_right);
		}
	};
}

ExpressionFunc assertIsUnaryOperation(UnaryOperator expectedOperation, ExpressionFunc assertOperand) {

	return [=] (ExpressionPtr& expr) {
		UnaryOperation* unaryOp = dynamic_cast<UnaryOperation*>(expr.get());

		ASSERT_TRUE(unaryOp != nullptr);
		EXPECT_EQ(expectedOperation, unaryOp->_operator);
		{
			SCOPED_TRACE("Verify operand");
			assertOperand(unaryOp->_operand);
		}
	};
}

ExpressionFunc assertIsLabelRef(const string &labelName) {
	return [=] (ExpressionPtr& expr) {
		LabelOperand* labelRefOp = dynamic_cast<LabelOperand*>(expr.get());

		ASSERT_TRUE(labelRefOp != nullptr);
		EXPECT_EQ(labelName, labelRefOp->_label);
	};
}

ExpressionFunc assertIsRegister(Register expectedRegister) {
	return [=] (ExpressionPtr& expr) {
		RegisterOperand* registerOp = dynamic_cast<RegisterOperand*>(expr.get());

		ASSERT_TRUE(registerOp != nullptr);
		EXPECT_EQ(expectedRegister, registerOp->_register);
	};
}

ExpressionFunc assertIsLiteral(uint32_t expectedValue) {
	return [=] (ExpressionPtr& expr) {
		LiteralOperand* literalOp = dynamic_cast<LiteralOperand*>(expr.get());

		ASSERT_TRUE(literalOp != nullptr);
		EXPECT_EQ(expectedValue, literalOp->_value);
	};
}

ExpressionFunc assertIsEvaluatedRegister(Register expectedRegister, bool expectedHasOffset, int32_t expectedValue) {
	return [=] (ExpressionPtr& expr) {
		EvaluatedRegister* registerOp = dynamic_cast<EvaluatedRegister*>(expr.get());

		ASSERT_TRUE(registerOp != nullptr);
		EXPECT_EQ(expectedRegister, registerOp->_register);
		EXPECT_EQ(expectedHasOffset, registerOp->_hasOffset);
		EXPECT_EQ(expectedValue, registerOp->getEvaluatedValue());
	};
}

ExpressionFunc assertIsEvaluatedRegister(Register expectedRegister) {
	return assertIsEvaluatedRegister(expectedRegister, false, 0);
}

ExpressionFunc assertIsEvaluatedLiteral(int32_t expectedValue) {
	return [=] (ExpressionPtr& expr) {
		EvaluatedLiteral* literalOp = dynamic_cast<EvaluatedLiteral*>(expr.get());

		ASSERT_TRUE(literalOp != nullptr);
		EXPECT_EQ(expectedValue, literalOp->getEvaluatedValue());
	};
}
