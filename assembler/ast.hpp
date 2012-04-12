#pragma once

#include <boost/variant/recursive_variant.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/optional.hpp>

#include "../common.hpp"

namespace ast {
	enum opcode_type {
		SET=1,
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
		JSR,
		JMP,
		PUSH,
		POP,
		TOTAl_OPCODES
	};

	struct label {
		label() {}
		label(const std::string &name) : name(name) {}
		std::string name;
	};

	struct register_argument {
		register_argument() {}
		register_argument(bool is_ptr, registers::register_type type) 
			: is_ptr(is_ptr), type(type) {}

		bool is_ptr;
		registers::register_type type;
	};

	struct literal_argument {
		literal_argument() {}
		literal_argument(bool is_ptr, uint32_t value) 
			: is_ptr(is_ptr), value(value) {}

		bool is_ptr;
		uint32_t value;
	};

	enum stack_type {
		STACK_PUSH,
		STACK_POP,
		STACK_PEEK
	};

	struct stack_argument {
		stack_argument() {}
		stack_argument(stack_type type) : type(type) {}

		stack_type type;
	};

	typedef boost::variant<
			register_argument,
			literal_argument,
			stack_argument
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
    ast::register_argument,
    (bool, is_ptr)
    (registers::register_type, type)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::literal_argument,
    (bool, is_ptr)
    (uint32_t, value)
)

BOOST_FUSION_ADAPT_STRUCT(
    ast::instruction,
    (ast::opcode_type, opcode)
    (ast::argument, a)
    (boost::optional<ast::argument>, b)
)