#pragma once

#include <string>
#include <cstdint>

#include "Expression.hpp"

namespace dcpu { namespace ast {
	enum class StackOperation {
		PUSH,
		POP,
		PEEK
	};

	enum class ArgumentPosition {
		A,
		B
	};

	class Argument {
	public:
		lexer::Location _location;
		ArgumentPosition _position;

		//virtual bool isNextWordUsed()=0;
		//virtual void compile()=0;

		Argument(const lexer::Location&, ArgumentPosition);
		virtual ~Argument();
	};

	typedef std::unique_ptr<Argument> ArgumentPtr;

	class StackArgument : public Argument {
	public:
		StackOperation _operation;

		StackArgument(const lexer::Location&, ArgumentPosition, StackOperation);
	};

	class IndirectArgument : public Argument {
	public:
		ExpressionPtr _expr;

		IndirectArgument(ArgumentPosition, ExpressionPtr&&);
		IndirectArgument(ArgumentPosition, ExpressionPtr&);
	};

	class ExpressionArgument : public Argument {
	public:
		ExpressionPtr _expr;

		ExpressionArgument(ArgumentPosition, ExpressionPtr&&);
		ExpressionArgument(ArgumentPosition, ExpressionPtr&);
	};

	std::string str(ArgumentPosition);
	std::string str(StackOperation);
} }