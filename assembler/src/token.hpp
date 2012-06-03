#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <memory>
#include <cstdint>
#include <list>

#include <boost/variant/variant.hpp>
#include "location.hpp"
#include "mnemonics.hpp"

namespace dcpu { namespace assembler {
	enum class token_type : uint8_t {
		LABEL,
		SYMBOL,
		DIRECTIVE,
		INSTRUCTION,
		REGISTER,
		STACK_OPERATION,
		INTEGER,
		INVALID_INTEGER,
		CHARACTER,
		NEWLINE,
		END_OF_INPUT,
		QUOTED_STRING,
		OPERATOR
	};

	enum class operator_type : uint8_t {
		SHIFT_LEFT,
		SHIFT_RIGHT,
		EQ,
		NEQ,
		GTE,
		LTE,
		AND,
		OR
	};

	enum class quote_type : uint8_t {
		DOUBLE_QUOTE,
		ANGLE_BRACKETS
	};

	enum class symbol_type : uint8_t {
		NORMAL,
		EXPLICIT
	};

	typedef boost::variant<
		std::nullptr_t,
		uint32_t,
		instruction_definition,
		registers,
		directives,
		stack_operation,
		quote_type,
		symbol_type,
		operator_type> token_data;

	class token {
	public:
		location_ptr location;
		token_type type;
		std::string content;
		token_data data;

		token(location_ptr&, token_type, const std::string&);
		token(location_ptr&, token_type, const std::string&, token_data data);
		token(location_ptr&, token_type, char c);

		bool is_integer() const;
		bool is_invalid_integer() const;
		bool is_quoted_string() const;
		bool is_register() const;
		bool is_register(registers) const;
		bool is_instruction(opcodes) const;
		bool is_instruction() const;
		bool is_directive() const;
		bool is_directive(directives) const;
		bool is_stack_operation() const;
		bool is_stack_operation(stack_operation) const;
		bool is_label() const;
		bool is_symbol() const;
		bool is_operator(operator_type) const;
		bool is_character(char c) const;
		bool is_newline() const;
		bool is_eoi() const;
		bool is_terminator() const;

		uint32_t get_integer() const;
		instruction_definition get_instruction() const;
		registers get_register() const;
		directives get_directive() const;
		stack_operation get_stack_operation() const;
		quote_type get_quote_type() const;
		symbol_type get_symbol_type() const;
		operator_type get_operator_type() const;
	};

	typedef std::list<token> token_list;
	typedef token_list::iterator token_iterator;

	token& next(token_iterator&, token_iterator);
}}
