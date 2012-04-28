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
		lexer::Location _location;

		Statement(const lexer::Location&);
		virtual ~Statement();

		virtual std::string str() const=0;
		virtual void buildSymbolTable(SymbolTablePtr& table, uint16Ptr &position) const=0;
		virtual void evaluateExpressions(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler);
		virtual bool compress(SymbolTablePtr& table);
		virtual void compile(std::vector<std::uint16_t> &output);

		static StatementPtr label(const lexer::Location&, const std::string &);
		static StatementPtr instruction(const lexer::Location&, Opcode, ArgumentPtr&, ArgumentPtr&);
		static StatementPtr null();
	};

	class Instruction : public Statement {
	public:
		Opcode _opcode;
		ArgumentPtr _a;
		ArgumentPtr _b;

		Instruction(const lexer::Location&, Opcode, ArgumentPtr &a, ArgumentPtr &b);

		virtual std::string str() const;
		virtual void buildSymbolTable(SymbolTablePtr& table, uint16Ptr &position) const;
		virtual void evaluateExpressions(SymbolTablePtr& table, ErrorHandlerPtr &errorHandler);
		virtual bool compress(SymbolTablePtr& table);
		virtual void compile(std::vector<std::uint16_t> &output);
	};

	class Label : public Statement {
	public:
		LabelType _type;
		std::string _name;

		Label(const lexer::Location&, const std::string&);

		virtual std::string str() const;
		virtual void buildSymbolTable(SymbolTablePtr& table, uint16Ptr &position) const;
	};

	std::string str(const StatementPtr&);
}}