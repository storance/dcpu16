#pragma once

#include <string>
#include <cstdint>

#include <boost/variant/variant.hpp>
#include <boost/optional.hpp>

#include "../common.hpp"

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
		GLOBAL,
		LOCAL,
		GLOBAL_NO_LOCAL
	};

	enum class StackArgument {
		PUSH,
		POP,
		PEEK
	};

	class Label {
	public:
		LabelType type;
		std::string name;

		Label(std::string name);
	};

	class LiteralArgument {
	public:
		bool forceLongForm;
		std::uint32_t value;

		LiteralArgument(std::uint32_t, bool);
	};

	typedef boost::variant<Register, LiteralArgument> IndirectArgument;

	typedef boost::variant<IndirectArgument, Register, LiteralArgument, StackArgument> Argument;

	class Instruction {
	public:
		Opcode opcode;
		Argument a;
		boost::optional<Argument> b;

		Instruction(Opcode, Argument a);
		Instruction(Opcode, Argument a, Argument b);
	};

	typedef boost::variant<Instruction, Label> Statement;
} }
