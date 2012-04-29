#pragma once

#include <string>
#include <cstdint>

#include "Common.hpp"
#include "Expression.hpp"
#include "../Types.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	class Argument {
	public:
		lexer::Location location;
		ArgumentPosition position;

		Argument(const lexer::Location&, ArgumentPosition);

		virtual bool isNextWordRequired() const=0;
		virtual CompileResult compile() const=0;
		virtual std::string str() const=0;

		virtual bool operator==(const Argument&) const=0;

		static ArgumentPtr stack(const lexer::Location&, ArgumentPosition, StackOperation);
		static ArgumentPtr stackPop(const lexer::Location&, ArgumentPosition);
		static ArgumentPtr stackPush(const lexer::Location&, ArgumentPosition);
		static ArgumentPtr stackPeek(const lexer::Location&, ArgumentPosition);
		static ArgumentPtr indirect(ArgumentPosition, ExpressionPtr&&);
		static ArgumentPtr expression(ArgumentPosition, ExpressionPtr&&);
		static ArgumentPtr null();
	};	

	class StackArgument : public Argument {
	public:
		StackOperation operation;

		StackArgument(const lexer::Location&, ArgumentPosition, StackOperation);

		virtual bool isNextWordRequired() const;
		virtual CompileResult compile() const;
		virtual std::string str() const;

		virtual bool operator==(const Argument&) const;
	};

	class IndirectArgument : public Argument {
	public:
		ExpressionPtr expr;

		IndirectArgument(ArgumentPosition, ExpressionPtr&&);
		IndirectArgument(ArgumentPosition, ExpressionPtr&);

		virtual bool isNextWordRequired() const;
		virtual CompileResult compile() const;
		virtual std::string str() const;

		virtual bool operator==(const Argument&) const;
	};

	class ExpressionArgument : public Argument {
	public:
		ExpressionPtr expr;

		ExpressionArgument(ArgumentPosition, ExpressionPtr&&);
		ExpressionArgument(ArgumentPosition, ExpressionPtr&);

		virtual bool isNextWordRequired() const;
		virtual CompileResult compile() const;
		virtual std::string str() const;

		virtual bool operator==(const Argument&) const;
	};

	bool operator== (const ArgumentPtr& left, const ArgumentPtr& right);
	std::ostream& operator<< (std::ostream& stream, const ArgumentPtr& arg);
} }