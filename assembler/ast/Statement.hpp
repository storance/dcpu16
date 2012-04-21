#pragma once

#include <string>
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

	class Instruction : public Statement {
	public:
		Opcode _opcode;
		std::shared_ptr<Argument> _a;
		std::shared_ptr<Argument> _b;

		Instruction(const Location&, Opcode, std::shared_ptr<Argument> a, std::shared_ptr<Argument> b);
	};

	class Label : public Statement {
	public:
		LabelType _type;
		std::string _name;

		Label(const Location&, const std::string&);
	};

} }