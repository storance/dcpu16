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

		Expression(Expression&&);
		Expression(const lexer::Location&);
		virtual ~Expression();

		//virtual void evaluate()=0;
		virtual bool isNextWordRequired() const=0;
		virtual bool isEvalsToLiteral() const=0;
		virtual std::string str() const=0;
	};

	typedef std::unique_ptr<Expression> ExpressionPtr;

	class UnaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
	public:
		UnaryOperator _operator;
		ExpressionPtr _operand;

		UnaryOperation(UnaryOperation&&);
		UnaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&);

		virtual bool isNextWordRequired() const;
		virtual bool isEvalsToLiteral() const;
		virtual std::string str() const;
	};

	class BinaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
	public:
		BinaryOperator _operator;
		ExpressionPtr _left;
		ExpressionPtr _right;

		BinaryOperation(BinaryOperation&&);
		BinaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&, ExpressionPtr&);

		virtual bool isNextWordRequired() const;
		virtual bool isEvalsToLiteral() const;
		virtual std::string str() const;
	};

	class RegisterOperand : public Expression {
	public:
		Register _register;

		RegisterOperand(const lexer::Location&, Register);

		virtual bool isNextWordRequired() const;
		virtual bool isEvalsToLiteral() const;
		virtual std::string str() const;
	};

	class LiteralOperand : public Expression {
	public:
		std::uint32_t _value;

		LiteralOperand(const lexer::Location&, std::uint32_t);

		virtual bool isNextWordRequired() const;
		virtual bool isEvalsToLiteral() const;
		virtual std::string str() const;
	};

	class LabelReferenceOperand : public Expression {
	public:
		std::string _label;

		LabelReferenceOperand(const lexer::Location&, const std::string&);
		LabelReferenceOperand(lexer::TokenPtr& token);

		virtual bool isNextWordRequired() const;
		virtual bool isEvalsToLiteral() const;
		virtual std::string str() const;
	};

	class InvalidExpression : public Expression {
	public:
		InvalidExpression(const lexer::Location&);

		virtual bool isNextWordRequired() const;
		virtual bool isEvalsToLiteral() const;
		virtual std::string str() const;
	};

	std::string str(UnaryOperator);
	std::string str(BinaryOperator);

	std::string str(const Expression&);
	std::string str(const ExpressionPtr&);
}}