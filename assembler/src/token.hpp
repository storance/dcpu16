#pragma once

#include <cstddef>
#include <ostream>
#include <string>
#include <memory>
#include <cstdint>
#include <list>

#include <boost/variant/variant.hpp>
#include "mnemonics.hpp"

namespace dcpu { namespace lexer {
	struct location {
		std::string source;
		uint32_t line;
		uint32_t column;

		location();
		location(const std::string&, uint32_t, uint32_t);

		bool operator==(const location& other) const;
	};

	typedef std::shared_ptr<location> location_ptr;
	std::ostream& operator<< (std::ostream& stream, const location_ptr& location);
	bool operator==(const location_ptr &, const location_ptr &);

	enum class token_type {
		LABEL,
		SYMBOL,
		DIRECTIVE,
		INSTRUCTION,
		REGISTER,
		STACK_OPERATION,
		INTEGER,
		INVALID_INTEGER,
		INCREMENT,
		DECREMENT,
		SHIFT_LEFT,
		SHIFT_RIGHT,
		CHARACTER,
		NEWLINE,
		END_OF_INPUT,
		QUOTED_STRING,
	};

	enum class quote_type {
		NONE,
		SINGLE_QUOTE,
		DOUBLE_QUOTE,
		ANGLE_BRACKETS
	};

	typedef boost::variant<
		std::nullptr_t,
		uint32_t,
		instruction_definition,
		register_definition,
		directives,
		stack_operation,
		quote_type> token_data;

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
		bool is_increment() const;
		bool is_decrement() const;
		bool is_shift_left() const;
		bool is_shift_right() const;
		bool is_character(char c) const;
		bool is_newline() const;
		bool is_eoi() const;
		bool is_terminator() const;

		uint32_t get_integer() const;
		instruction_definition get_instruction() const;
		register_definition	get_register() const;
		directives get_directive() const;
		stack_operation get_stack_operation() const;
		quote_type get_quote_type() const;
	};

	typedef std::list<token> token_list;
	typedef token_list::iterator token_iterator;

	token& next(token_iterator&, token_iterator);
}}