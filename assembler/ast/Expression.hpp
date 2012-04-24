#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include "../../common.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	enum class BinaryOperator : std::uint8_t {
		NONE,
		PLUS,
		MINUS,
		MULTIPLY,
		DIVIDE,
		MODULO,
		SHIFT_LEFT,
		SHIFT_RIGHT,
		AND,
		OR,
		XOR
	};

	enum class UnaryOperator : std::uint8_t {
		NONE,
		PLUS,
		MINUS,
		NOT
	};

	class Expression {
	public:
		lexer::Location _location;

		//virtual void evaluate()=0;
		//virtual bool isNextWordRequired()=0;
		virtual bool isEvalsToLiteral()=0;
		virtual bool isSimple()=0;

		Expression(Expression&&);
		Expression(const lexer::Location&);
		virtual ~Expression();
	};

	typedef std::unique_ptr<Expression> ExpressionPtr;

	class UnaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
	public:
		UnaryOperator _operator;
		ExpressionPtr _operand;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		UnaryOperation(UnaryOperation&&);
		UnaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&);
	};

	class BinaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
	public:
		BinaryOperator _operator;
		ExpressionPtr _left;
		ExpressionPtr _right;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		BinaryOperation(BinaryOperation&&);
		BinaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&, ExpressionPtr&);
	};

	class RegisterOperand : public Expression {
	public:
		common::Register _register;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		RegisterOperand(const lexer::Location&, common::Register);
	};

	class LiteralOperand : public Expression {
	public:
		std::uint32_t _value;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		LiteralOperand(const lexer::Location&, std::uint32_t);
	};

	class LabelReferenceOperand : public Expression {
	public:
		std::string _label;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		LabelReferenceOperand(const lexer::Location&, const std::string&);
		LabelReferenceOperand(lexer::TokenPtr& token);
	};

	class InvalidExpression : public Expression {
	public:
		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		InvalidExpression(const lexer::Location&);
	};

	std::string str(UnaryOperator op);
	std::string str(BinaryOperator op);
}}