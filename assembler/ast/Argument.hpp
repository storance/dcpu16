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
		lexer::Location _location;
		ArgumentPosition _position;

		Argument(const lexer::Location&, ArgumentPosition);
		virtual ~Argument();

		virtual bool isNextWordRequired() const=0;
		virtual uint8_t compile(std::vector<std::uint16_t> &output)=0;
		virtual std::string str() const=0;

		static std::unique_ptr<Argument> stack(const lexer::Location&, ArgumentPosition, StackOperation);
		static std::unique_ptr<Argument> stackPop(const lexer::Location&, ArgumentPosition);
		static std::unique_ptr<Argument> stackPush(const lexer::Location&, ArgumentPosition);
		static std::unique_ptr<Argument> stackPeek(const lexer::Location&, ArgumentPosition);
		static std::unique_ptr<Argument> indirect(ArgumentPosition, ExpressionPtr&&);
		static std::unique_ptr<Argument> expression(ArgumentPosition, ExpressionPtr&&);
		static std::unique_ptr<Argument> null();
	};	

	class StackArgument : public Argument {
	public:
		StackOperation _operation;

		StackArgument(const lexer::Location&, ArgumentPosition, StackOperation);

		virtual bool isNextWordRequired() const;
		virtual uint8_t compile(std::vector<std::uint16_t> &output);
		virtual std::string str() const;
	};

	class IndirectArgument : public Argument {
	public:
		ExpressionPtr _expr;

		IndirectArgument(ArgumentPosition, ExpressionPtr&&);
		IndirectArgument(ArgumentPosition, ExpressionPtr&);

		virtual bool isNextWordRequired() const;
		virtual uint8_t compile(std::vector<std::uint16_t> &output);
		virtual std::string str() const;
	};

	class ExpressionArgument : public Argument {
	public:
		ExpressionPtr _expr;

		ExpressionArgument(ArgumentPosition, ExpressionPtr&&);
		ExpressionArgument(ArgumentPosition, ExpressionPtr&);

		virtual bool isNextWordRequired() const;
		virtual uint8_t compile(std::vector<std::uint16_t> &output);
		virtual std::string str() const;
	};

	std::string str(const ArgumentPtr &);
} }