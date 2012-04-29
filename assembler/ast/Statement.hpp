#pragma once

#include <string>
#include <list>
#include <vector>
#include <cstdint>

#include "Common.hpp"
#include "OpcodeDefinition.hpp"
#include "Argument.hpp"
#include "../Types.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	class Statement {
	public:
		lexer::Location location;

		Statement(const lexer::Location&);
		virtual ~Statement();

		virtual std::string str() const=0;
		virtual void buildSymbolTable(SymbolTablePtr& table, uint16Ptr &position) const=0;
		virtual void evaluateExpressions(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler);
		virtual bool compress(SymbolTablePtr& table);
		virtual void compile(std::vector<std::uint16_t> &output);

		virtual bool operator==(const Statement&) const=0;

		static StatementPtr label(const lexer::Location&, const std::string &);
		static StatementPtr label(const lexer::Location&, const std::string &, LabelType type);
		static StatementPtr instruction(const lexer::Location&, Opcode, ArgumentPtr&, ArgumentPtr&);
		static StatementPtr instruction(const lexer::Location&, Opcode, ArgumentPtr&&, ArgumentPtr&&);
		static StatementPtr null();
	};

	class Instruction : public Statement {
		static void compile(std::vector<std::uint16_t> &output, Opcode opcode, ArgumentPtr &a, ArgumentPtr &b);
	public:
		Opcode opcode;
		ArgumentPtr a;
		ArgumentPtr b;

		Instruction(const lexer::Location&, Opcode, ArgumentPtr &a, ArgumentPtr &b);

		virtual std::string str() const;
		virtual void buildSymbolTable(SymbolTablePtr& table, uint16Ptr &position) const;
		virtual void evaluateExpressions(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler);
		virtual bool compress(SymbolTablePtr& table);
		virtual void compile(std::vector<std::uint16_t> &output);

		virtual bool operator==(const Statement&) const;
	};

	class Label : public Statement {
	public:
		LabelType type;
		std::string name;

		Label(const lexer::Location&, const std::string&);
		Label(const lexer::Location&, const std::string&, LabelType type);

		virtual std::string str() const;
		virtual void buildSymbolTable(SymbolTablePtr& table, uint16Ptr &position) const;

		virtual bool operator==(const Statement&) const;
	};

	bool operator== (const StatementPtr& left, const StatementPtr& right);
	std::ostream& operator<< (std::ostream& stream, const StatementPtr&);
}}