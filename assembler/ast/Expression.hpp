#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include "RegisterDefinition.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	enum class BinaryOperator : std::uint8_t {
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
		PLUS,
		MINUS,
		NOT,
		BITWISE_NOT
	};

	class Expression {
	public:
		lexer::Location _location;

		//virtual void evaluate()=0;
		//virtual bool isNextWordRequired()=0;
		virtual bool isEvalsToLiteral() const=0;
		virtual bool isSimple() const=0;
		virtual std::string str() const=0;

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

		virtual bool isEvalsToLiteral() const;
		virtual bool isSimple() const;
		virtual std::string str() const;

		UnaryOperation(UnaryOperation&&);
		UnaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&);
	};

	class BinaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
	public:
		BinaryOperator _operator;
		ExpressionPtr _left;
		ExpressionPtr _right;

		virtual bool isEvalsToLiteral() const;
		virtual bool isSimple() const;
		virtual std::string str() const;

		BinaryOperation(BinaryOperation&&);
		BinaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&, ExpressionPtr&);
	};

	class RegisterOperand : public Expression {
	public:
		Register _register;

		virtual bool isEvalsToLiteral() const;
		virtual bool isSimple() const;
		virtual std::string str() const;

		RegisterOperand(const lexer::Location&, Register);
	};

	class LiteralOperand : public Expression {
	public:
		std::uint32_t _value;

		virtual bool isEvalsToLiteral() const;
		virtual bool isSimple() const;
		virtual std::string str() const;

		LiteralOperand(const lexer::Location&, std::uint32_t);
	};

	class LabelReferenceOperand : public Expression {
	public:
		std::string _label;

		virtual bool isEvalsToLiteral() const;
		virtual bool isSimple() const;
		virtual std::string str() const;

		LabelReferenceOperand(const lexer::Location&, const std::string&);
		LabelReferenceOperand(lexer::TokenPtr& token);
	};

	class InvalidExpression : public Expression {
	public:
		virtual bool isEvalsToLiteral() const;
		virtual bool isSimple() const;
		virtual std::string str() const;

		InvalidExpression(const lexer::Location&);
	};

	std::string str(UnaryOperator);
	std::string str(BinaryOperator);

	std::string str(const Expression&);
	std::string str(ExpressionPtr&);
}}