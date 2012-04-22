#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include "../../common.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	enum class BinaryOperator {
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

	enum class UnaryOperator {
		PLUS,
		MINUS,
		NOT
	};

	class Expression {
	public:
		Location _location;

		//virtual void evaluate()=0;
		//virtual bool isNextWordRequired()=0;

		Expression(const Location&);
		virtual ~Expression();
	};

	class UnaryOperation : public Expression {
	public:
		UnaryOperator _operator;
		std::shared_ptr<Expression> _operand;

		UnaryOperation(const Location&, UnaryOperator, std::shared_ptr<Expression>);
	};

	class BinaryOperation : public Expression {
	public:
		BinaryOperator _operator;
		std::shared_ptr<Expression> _left;
		std::shared_ptr<Expression> _right;

		BinaryOperation(const Location&, BinaryOperator, std::shared_ptr<Expression>, std::shared_ptr<Expression>);
	};

	class RegisterOperand : public Expression {
	public:
		common::Register _register;

		RegisterOperand(const Location&, common::Register);
	};

	class LiteralOperand : public Expression {
	public:
		std::uint32_t _value;

		LiteralOperand(const Location&, std::uint32_t);
	};

	class LabelReferenceOperand : public Expression {
	public:
		std::string _label;

		LabelReferenceOperand(const Location&, const std::string&);
	};

	class InvalidExpression : public Expression {
	public:
		InvalidExpression(const Location&);
	};
}}