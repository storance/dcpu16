#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <tuple>

#include <boost/optional.hpp>

#include "Common.hpp"
#include "RegisterDefinition.hpp"
#include "../Types.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	class Expression {
	protected:
		bool literal, evaluatable, evaluated;

		Expression(const lexer::Location&, bool literal, bool evaluatable, bool evaluated);
	public:
		lexer::Location location;

		bool isLiteral();
		bool isEvaluatable();
		bool isEvaluated();

		virtual std::int32_t getEvaluatedValue();
		virtual void setEvaluatedValue(std::int32_t newValue);

		virtual void resolveLabels(SymbolTablePtr&, ErrorHandlerPtr &);
		virtual bool isNextWordRequired(const ArgumentFlags&) const;
		virtual CompileResult compile(const ArgumentFlags&) const;
		virtual std::unique_ptr<Expression> evaluate() const=0;

		virtual std::string str() const=0;
		virtual bool operator==(const Expression&) const=0;

		static ExpressionPtr unaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&);
		static ExpressionPtr unaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&&);
		static ExpressionPtr binaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&, ExpressionPtr&);
		static ExpressionPtr binaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&&, ExpressionPtr&&);
		static ExpressionPtr literalOperand(const lexer::Location&, std::uint32_t);
		static ExpressionPtr labelOperand(const lexer::Location&, const std::string&);
		static ExpressionPtr registerOperand(const lexer::Location&, Register);
		static ExpressionPtr invalid(const lexer::Location&);
		static ExpressionPtr evaluatedRegister(const lexer::Location&, Register);
		static ExpressionPtr evaluatedRegister(const lexer::Location&, Register, std::int32_t);
		static ExpressionPtr evaluatedLiteral(const lexer::Location&, std::int32_t);
	};	

	class UnaryOperation : public Expression {
	protected:
		UnaryOperator _operator;
		ExpressionPtr operand;
	public:
		UnaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&);

		virtual void resolveLabels(SymbolTablePtr&, ErrorHandlerPtr &);
		virtual ExpressionPtr evaluate() const;
		virtual std::string str() const;
		
		virtual bool operator==(const Expression&) const;
	};

	class BinaryOperation : public Expression {
	protected:
		BinaryOperator _operator;
		ExpressionPtr left;
		ExpressionPtr right;
	public:
		BinaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&, ExpressionPtr&);

		virtual void resolveLabels(SymbolTablePtr&, ErrorHandlerPtr &);
		virtual ExpressionPtr evaluate() const;
		virtual std::string str() const;
		
		virtual bool operator==(const Expression&) const;
	};

	class RegisterOperand : public Expression {
	protected:
		Register _register;
	public:
		RegisterOperand(const lexer::Location&, Register);

		virtual ExpressionPtr evaluate() const;
		virtual std::string str() const;
		
		virtual bool operator==(const Expression&) const;
	};

	class LiteralOperand : public Expression {
	protected:
		std::uint32_t value;
	public:
		LiteralOperand(const lexer::Location&, std::uint32_t);

		virtual ExpressionPtr evaluate() const;;
		virtual std::string str() const;
		
		virtual bool operator==(const Expression&) const;
	};

	class LabelOperand : public Expression {
	protected:
		std::string label;
		std::uint16_t *position;
	public:
		LabelOperand(const lexer::Location&, const std::string&);

		virtual void resolveLabels(SymbolTablePtr&, ErrorHandlerPtr &);
		virtual ExpressionPtr evaluate() const;
		virtual std::string str() const;
		
		virtual bool operator==(const Expression&) const;
	};

	class InvalidExpression : public Expression {
	public:
		InvalidExpression(const lexer::Location&);

		virtual ExpressionPtr evaluate() const;
		virtual std::string str() const;
		
		virtual bool operator==(const Expression&) const;
	};

	class EvaluatedLiteral : public Expression {
	protected:
		std::int32_t value;
	public:
		EvaluatedLiteral(const lexer::Location&, std::int32_t value);

		virtual std::int32_t getEvaluatedValue();
		virtual void setEvaluatedValue(std::int32_t newValue);

		virtual bool isNextWordRequired(const ArgumentFlags&) const;
		virtual ExpressionPtr evaluate() const;
		virtual CompileResult compile(const ArgumentFlags&) const;
		virtual std::string str() const;
		
		virtual bool operator==(const Expression&) const;
	};

	class EvaluatedRegister : public Expression {
	protected:
		ast::Register _register;
		bool hasOffset;
		std::int32_t offset;
	public:
		EvaluatedRegister(const lexer::Location&, ast::Register _register, bool hasOffset, std::int32_t offset);

		virtual std::int32_t getEvaluatedValue();
		virtual void setEvaluatedValue(std::int32_t newValue);

		virtual bool isNextWordRequired(const ArgumentFlags&) const;
		virtual ExpressionPtr evaluate() const;
		virtual CompileResult compile(const ArgumentFlags&) const;
		CompileResult compileRegister(const ArgumentFlags&, uint8_t, uint8_t, uint8_t) const;
		virtual std::string str() const;
		
		virtual bool operator==(const Expression&) const;
	};

	bool operator==(const ExpressionPtr&, const ExpressionPtr &);
	std::ostream& operator<< (std::ostream& stream, const ExpressionPtr& expr);
}}