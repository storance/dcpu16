#pragma once

#include <string>
#include <list>
#include <vector>
#include <cstdint>

#include "Common.hpp"
#include "OpcodeDefinition.hpp"
#include "Argument.hpp"
#include "../ErrorHandler.hpp"

namespace dcpu { class SymbolTable; }

namespace dcpu { namespace ast {
	class Statement {
	public:
		lexer::Location _location;

		Statement(const lexer::Location&);
		virtual ~Statement();

		virtual std::string str() const=0;
		virtual void buildSymbolTable(SymbolTable& table, std::uint16_t &position) const=0;
		virtual void evaluateExpressions(SymbolTable& table, ErrorHandler &errorHandler);
		virtual bool compress(SymbolTable& table);
		virtual void compile(std::vector<std::uint16_t> &output);
	};

	typedef std::unique_ptr<ast::Statement> StatementPtr;
	typedef std::list<StatementPtr> StatementList;

	class Instruction : public Statement {
	public:
		Opcode _opcode;
		ArgumentPtr _a;
		ArgumentPtr _b;

		Instruction(const lexer::Location&, Opcode, ArgumentPtr &a, ArgumentPtr &b);
		Instruction(const lexer::Location&, Opcode, ArgumentPtr &&a, ArgumentPtr &&b);

		virtual std::string str() const;
		virtual void buildSymbolTable(SymbolTable& table, std::uint16_t &position) const;
		virtual void evaluateExpressions(SymbolTable& table, ErrorHandler &errorHandler);
		virtual bool compress(SymbolTable& table);
		virtual void compile(std::vector<std::uint16_t> &output);
	};

	class Label : public Statement {
	public:
		LabelType _type;
		std::string _name;

		Label(const lexer::Location&, const std::string&);

		virtual std::string str() const;
		virtual void buildSymbolTable(SymbolTable& table, std::uint16_t &position) const;
	};

	std::string str(const StatementPtr&);
}}