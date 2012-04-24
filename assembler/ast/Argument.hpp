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

	class Argument {
	public:
		lexer::Location _location;

		//virtual bool isNextWordUsed()=0;
		//virtual void compile()=0;

		Argument(const lexer::Location&);
		virtual ~Argument();
	};

	typedef std::unique_ptr<Argument> ArgumentPtr;

	class StackArgument : public Argument {
	public:
		StackOperation _operation;

		StackArgument(const lexer::Location&, StackOperation);
	};

	class IndirectArgument : public Argument {
	public:
		ExpressionPtr _expr;

		IndirectArgument(ExpressionPtr&&);
		IndirectArgument(ExpressionPtr&);
	};

	class ExpressionArgument : public Argument {
	public:
		ExpressionPtr _expr;

		ExpressionArgument(ExpressionPtr&&);
		ExpressionArgument(ExpressionPtr&);
	};
} }