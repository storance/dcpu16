#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

#include "Common.hpp"
#include "RegisterDefinition.hpp"
#include "../Types.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	class Expression {
	public:
		lexer::Location _location;

		Expression(Expression&&);
		Expression(const lexer::Location&);
		virtual ~Expression();

		virtual uint8_t compile(std::vector<std::uint16_t> &, ArgumentPosition, bool, bool);
		virtual std::unique_ptr<Expression> evaluate() const=0;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const=0;
		virtual bool isLiteral() const=0;
		virtual bool isEvaluatable() const=0;
		virtual bool isEvaluated();
		virtual std::int32_t getEvaluatedValue();
		virtual void updateEvaluatedValue(std::int32_t newValue);
		virtual std::string str() const=0;

		static ExpressionPtr unaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&);
		static ExpressionPtr binaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&, ExpressionPtr&);
		static ExpressionPtr literalOperand(const lexer::Location&, std::uint32_t);
		static ExpressionPtr labelOperand(const lexer::Location&, const std::string&);
		static ExpressionPtr registerOperand(const lexer::Location&, Register);
		static ExpressionPtr invalid(const lexer::Location&);
		static ExpressionPtr evaluatedRegister(const lexer::Location&, Register);
		static ExpressionPtr evaluatedRegister(const lexer::Location&, Register, std::int32_t);
		static ExpressionPtr evaluatedLiteral(const lexer::Location&, std::int32_t);
	};	

	class UnaryOperation : public Expression {
		bool _cachedIsLiteral;
		bool _cachedIsEvaluatable;

		void updateCache();
	public:
		UnaryOperator _operator;
		ExpressionPtr _operand;

		UnaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&);

		virtual ExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class BinaryOperation : public Expression {
		bool _cachedIsLiteral;
		bool _cachedIsEvaluatable;

		void updateCache();
	public:
		BinaryOperator _operator;
		ExpressionPtr _left;
		ExpressionPtr _right;

		BinaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&, ExpressionPtr&);

		virtual ExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class RegisterOperand : public Expression {
	public:
		Register _register;

		RegisterOperand(const lexer::Location&, Register);

		virtual ExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class LiteralOperand : public Expression {
	public:
		std::uint32_t _value;

		LiteralOperand(const lexer::Location&, std::uint32_t);

		virtual ExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class LabelOperand : public Expression {
	public:
		std::string _label;
		std::uint16_t *_position;

		LabelOperand(const lexer::Location&, const std::string&);

		virtual ExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class InvalidExpression : public Expression {
	public:
		InvalidExpression(const lexer::Location&);

		virtual ExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class EvaluatedExpression : public Expression {
	protected:
		std::int32_t _value;
	public:
		EvaluatedExpression(const lexer::Location&, std::int32_t value);

		virtual bool isEvaluatable() const;
		virtual bool isEvaluated();
		virtual std::int32_t getEvaluatedValue();
		virtual void updateEvaluatedValue(std::int32_t newValue);
	};

	class EvaluatedLiteral : public EvaluatedExpression {
	public:
		EvaluatedLiteral(const lexer::Location&, std::int32_t value);

		virtual ExpressionPtr evaluate() const;
		virtual uint8_t compile(std::vector<std::uint16_t>&, ArgumentPosition, bool, bool);
		virtual bool isLiteral() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual std::string str() const;
	};

	class EvaluatedRegister : public EvaluatedExpression {
	public:
		ast::Register _register;
		bool _hasOffset;

		EvaluatedRegister(const lexer::Location&, ast::Register _register);
		EvaluatedRegister(const lexer::Location&, ast::Register _register, bool hasOffset, std::int32_t offset);

		virtual ExpressionPtr evaluate() const;
		virtual uint8_t compile(std::vector<std::uint16_t>&, ArgumentPosition, bool, bool);
		virtual bool isLiteral() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual void updateEvaluatedValue(std::int32_t newValue);
		virtual std::string str() const;
	};

	std::string str(const Expression&);
	std::string str(const ExpressionPtr&);
}}