#pragma once

#include <string>
#include <list>
#include <cstdint>

#include "Argument.hpp"

namespace dcpu { namespace ast {
	enum class Opcode : std::uint16_t {
		// Basic
		SET=0x1,
		ADD,
		SUB,
		MUL,
		MLI,
		DIV,
		DVI,
		MOD,
		AND,
		BOR,
		XOR,
		SHR,
		ASR,
		SHL,
		IFB,
		IFC,
		IFE,
		IFN,
		IFG,
		IFA,
		IFL,
		IFU,
		// Non-basic 
		JSR=0x20,
		INT=0x100,
		ING=0x120,
		INS=0x140,
		HWN=0x200,
		HWQ=0x220,
		HWI=0x240,
		// Extensions
		JMP=0x400,
	};

	enum class LabelType {
		Global,
		Local,
		GlobalNoAttach
	};

	class Statement {
	public:
		lexer::Location _location;

		//virtual void buildSymbolTable()=0;
		//virtual void compile()=0;

		Statement(const lexer::Location&);
		virtual ~Statement();
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
	};

	class Label : public Statement {
	public:
		LabelType _type;
		std::string _name;

		Label(const lexer::Location&, const std::string&);
	};

	std::string str(Opcode);
} }