#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

#include "Common.hpp"
#include "RegisterDefinition.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	class EvaluatedExpression;

	typedef std::unique_ptr<EvaluatedExpression> EvaluatedExpressionPtr;

	class Expression {
	public:
		lexer::Location _location;

		Expression(Expression&&);
		Expression(const lexer::Location&);
		virtual ~Expression();

		virtual uint8_t compile(std::vector<std::uint16_t> &output, ArgumentPosition position, bool indirect,
			bool forceNextWord);
		virtual EvaluatedExpressionPtr evaluate() const=0;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const=0;
		virtual bool isEvalsToLiteral() const=0;
		virtual bool isEvaluatable() const=0;
		virtual std::string str() const=0;
	};

	typedef std::unique_ptr<Expression> ExpressionPtr;

	class UnaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
		bool _cachedEvaluatable;
	public:
		UnaryOperator _operator;
		ExpressionPtr _operand;

		UnaryOperation(const lexer::Location&, UnaryOperator, ExpressionPtr&);

		virtual EvaluatedExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isEvalsToLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class BinaryOperation : public Expression {
		bool _cachedEvalsToLiteral;
		bool _cachedEvaluatable;
	public:
		BinaryOperator _operator;
		ExpressionPtr _left;
		ExpressionPtr _right;

		BinaryOperation(const lexer::Location&, BinaryOperator, ExpressionPtr&, ExpressionPtr&);

		virtual EvaluatedExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isEvalsToLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class RegisterOperand : public Expression {
	public:
		Register _register;

		RegisterOperand(const lexer::Location&, Register);

		virtual EvaluatedExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isEvalsToLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class LiteralOperand : public Expression {
	public:
		std::uint32_t _value;

		LiteralOperand(const lexer::Location&, std::uint32_t);

		virtual EvaluatedExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isEvalsToLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class LabelReferenceOperand : public Expression {
	public:
		std::string _label;
		std::uint16_t *_position;

		LabelReferenceOperand(const lexer::Location&, const std::string&);
		LabelReferenceOperand(lexer::TokenPtr& token);

		virtual EvaluatedExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isEvalsToLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class InvalidExpression : public Expression {
	public:
		InvalidExpression(const lexer::Location&);

		virtual EvaluatedExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual bool isEvalsToLiteral() const;
		virtual bool isEvaluatable() const;
		virtual std::string str() const;
	};

	class EvaluatedExpression : public Expression {
	protected:
		std::int32_t _value;
	public:
		std::int32_t getValue();
		virtual void setValue(std::int32_t);

		EvaluatedExpression(const lexer::Location&, std::int32_t value);

		virtual bool isRegister() const=0;
		virtual bool isEvaluatable() const;
	};

	class EvaluatedLiteral : public EvaluatedExpression {
	public:
		EvaluatedLiteral(const lexer::Location&, std::int32_t value);

		virtual bool isRegister() const;
		virtual bool isEvalsToLiteral() const;
		virtual uint8_t compile(std::vector<std::uint16_t> &output, ArgumentPosition position, bool indirect,
			bool forceNextWord);
		virtual EvaluatedExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual std::string str() const;
	};

	class EvaluatedRegister : public EvaluatedExpression {
	public:
		ast::Register _register;
		bool _hasOffset;

		EvaluatedRegister(const lexer::Location&, ast::Register _register, bool hasOffset, std::int32_t offset);

		virtual void setValue(std::int32_t);

		virtual bool isRegister() const;
		virtual uint8_t compile(std::vector<std::uint16_t> &output, ArgumentPosition position, bool indirect,
			bool forceNextWord);
		virtual bool isEvalsToLiteral() const;
		virtual EvaluatedExpressionPtr evaluate() const;
		virtual bool isNextWordRequired(ArgumentPosition position, bool forceNextWord) const;
		virtual std::string str() const;
	};

	std::string str(UnaryOperator);
	std::string str(BinaryOperator);

	std::string str(const Expression&);
	std::string str(const ExpressionPtr&);
}}