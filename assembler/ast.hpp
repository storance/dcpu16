#pragma once

#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>

#include "../common.hpp"

namespace ast {
	enum opcode_type {
		// two arg
		SET,
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

		// one arg
		JSR,
		JMP, // alias for SET PC, a
		PUSH, // alias for SET [--SP], a 
		POP, // alias for SET [SP++], a
		TOTAl_OPCODES
	};

	struct label {
		label() {}
		label(const std::string &name) : name(name) {}
		std::string name;
	};

	struct register_argument {
		register_argument() {}
		register_argument(registers::register_type type) : type(type) {}
		registers::register_type type;
	};

	struct register_ptr_argument {
		register_ptr_argument() {}
		register_ptr_argument(registers::register_type type) : type(type) {}
		registers::register_type type;
	};

	struct literal_argument {
		literal_argument() {}
		literal_argument(uint32_t value) : value(value) {}
		uint32_t value;
	};

	struct literal_ptr_argument {
		literal_ptr_argument() {}
		literal_ptr_argument(uint32_t value) : value(value) {}
		uint32_t value;
	};

	typedef boost::variant<
			register_argument,
			register_ptr_argument,
			literal_argument,
			literal_ptr_argument
		> argument;

	struct instruction {
		opcode_type opcode;
		argument a;
		boost::optional<argument> b;
	};

	typedef boost::variant<
		instruction,
		label
		> statement;
}

BOOST_FUSION_ADAPT_STRUCT(
    ast::instruction,
    (ast::opcode_type, opcode)
    (ast::argument, a)
    (boost::optional<ast::argument>, b)
)