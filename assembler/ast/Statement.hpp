#pragma once

#include <string>
#include <list>
#include <cstdint>

#include "Argument.hpp"

namespace dcpu { namespace ast {
	enum class Opcode {
		SET=0x1,
		ADD,
		SUB,
		MUL,
		DIV,
		MOD,
		SHL,
		SHR,
		AND,
		BOR,
		XOR,
		IFE,
		IFN,
		IFG,
		IFB,
		JSR=0x100,
		JMP=0x400,
		PUSH,
		POP
	};

	enum class LabelType {
		Global,
		Local,
		GlobalNoAttach
	};

	class Statement {
	public:
		Location _location;

		//virtual void buildSymbolTable()=0;
		//virtual void compile()=0;

		Statement(const Location&);
		virtual ~Statement();
	};

	typedef std::unique_ptr<ast::Statement> StatementPtr;
	typedef std::list<StatementPtr> StatementList;

	class Instruction : public Statement {
	public:
		Opcode _opcode;
		ArgumentPtr _a;
		ArgumentPtr _b;

		Instruction(const Location&, Opcode, ArgumentPtr &a, ArgumentPtr &b);
		Instruction(const Location&, Opcode, ArgumentPtr &&a, ArgumentPtr &&b);
	};

	class Label : public Statement {
	public:
		LabelType _type;
		std::string _name;

		Label(const Location&, const std::string&);
	};

} }