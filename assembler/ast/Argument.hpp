#pragma once

#include <string>
#include <cstdint>

#include "Common.hpp"
#include "Expression.hpp"
#include "../Types.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	class Argument {
	protected:
		ArgumentFlags flags;
	public:
		lexer::Location location;

		Argument(const lexer::Location&, const ArgumentFlags&);

		virtual void resolveLabels(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler, std::uint16_t pc);
		virtual bool compress(SymbolTablePtr& table, std::uint16_t pc);
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
		StackOperation operation;
	public:
		StackArgument(const lexer::Location&, ArgumentPosition, StackOperation);

		virtual bool isNextWordRequired() const;
		virtual CompileResult compile() const;
		virtual std::string str() const;

		virtual bool operator==(const Argument&) const;
	};

	class ExpressionArgument : public Argument {
		ExpressionPtr expr;
		bool nextWordRequired;
	public:
		ExpressionArgument(ArgumentPosition, ExpressionPtr&, bool, bool);

		virtual void resolveLabels(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler, std::uint16_t pc);
		virtual bool compress(SymbolTablePtr& table, std::uint16_t pc);
		virtual bool isNextWordRequired() const;
		virtual CompileResult compile() const;
		virtual std::string str() const;

		virtual bool operator==(const Argument&) const;
	};

	bool operator== (const ArgumentPtr& left, const ArgumentPtr& right);
	std::ostream& operator<< (std::ostream& stream, const ArgumentPtr& arg);
} }