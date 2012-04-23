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
		Location _location;

		//virtual bool isNextWordUsed()=0;
		//virtual void compile()=0;

		Argument(const Location&);
		virtual ~Argument();
	};

	class StackArgument : public Argument {
	public:
		StackOperation _operation;

		StackArgument(const Location&, StackOperation);
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