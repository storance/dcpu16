#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include "../../common.hpp"
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
		NOT
	};

	std::string str(UnaryOperator op);
	std::string str(BinaryOperator op);

	class Expression {
	public:
		Location _location;

		//virtual void evaluate()=0;
		//virtual bool isNextWordRequired()=0;
		virtual bool isEvalsToLiteral()=0;
		virtual bool isSimple()=0;

		Expression(const Location&);
		virtual ~Expression();
	};

	class UnaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
	public:
		UnaryOperator _operator;
		std::shared_ptr<Expression> _operand;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		UnaryOperation(const Location&, UnaryOperator, std::shared_ptr<Expression>);
	};

	class BinaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
	public:
		BinaryOperator _operator;
		std::shared_ptr<Expression> _left;
		std::shared_ptr<Expression> _right;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		BinaryOperation(const Location&, BinaryOperator, std::shared_ptr<Expression>, std::shared_ptr<Expression>);
	};

	class RegisterOperand : public Expression {
	public:
		common::Register _register;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		RegisterOperand(const Location&, common::Register);
	};

	class LiteralOperand : public Expression {
	public:
		std::uint32_t _value;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		LiteralOperand(const Location&, std::uint32_t);
	};

	class LabelReferenceOperand : public Expression {
	public:
		std::string _label;

		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		LabelReferenceOperand(const Location&, const std::string&);
		LabelReferenceOperand(std::shared_ptr<Token> token);
	};

	class InvalidExpression : public Expression {
	public:
		virtual bool isEvalsToLiteral();
		virtual bool isSimple();

		InvalidExpression(const Location&);
	};
}}