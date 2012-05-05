#pragma once

#include <string>
#include <cstdint>
#include <boost/variant.hpp>

#include "mnemonics.hpp"
#include "expression.hpp"
#include "token.hpp"

namespace dcpu { namespace ast {
	enum class argument_position : std::uint8_t {
		A, B
	};

	enum class stack_operation : std::uint8_t {
		PUSH, POP, PEEK
	};

	enum class label_type : std::uint8_t {
		Global, Local, GlobalNoAttach
	};

	struct base_argument : public locatable {
		argument_position position;

		base_argument(const lexer::location_ptr &location, argument_position position);
	};

	struct stack_argument : public base_argument {
		stack_operation operation;
		optional_expression pick_expr;

		stack_argument(const lexer::location_ptr &location, argument_position position, stack_operation operation);

		bool operator==(const stack_argument& other) const;
	};

	struct expression_argument : public base_argument {
		bool indirect;
		bool force_next_word;
		bool next_word_required;
		expression expr;

		expression_argument(const lexer::location_ptr &location, argument_position position, const expression &expr,
			bool indirect, bool force_next_word);

		bool operator==(const expression_argument& other) const;
	};

	typedef boost::variant<stack_argument, expression_argument> argument;
	typedef boost::optional<argument> optional_argument;

	struct instruction : public locatable {
		opcodes opcode;
		argument a;
		optional_argument b;

		instruction(const lexer::location_ptr& location, opcodes opcode, const argument &a, const optional_argument &b);

		bool operator==(const instruction& other) const;
	};

	struct label : public locatable {
		label_type type;
		std::string name;

		label(const lexer::location_ptr &location, const std::string& name);
		label(const lexer::location_ptr &location, const std::string& name, label_type type);

		bool operator==(const label&) const;
	};

	struct data : public locatable {
		std::vector<std::uint16_t> value;

		data(const lexer::location_ptr &location);
		data(const lexer::location_ptr &location, const std::vector<std::uint16_t> &value);

		bool operator==(const data&) const;
		void append(uint16_t word);
		void append(const std::string literal);
	};

	typedef boost::variant<instruction, label, data> statement;
	typedef std::list<statement> statement_list;

	class calculate_size_expression : public boost::static_visitor<std::uint16_t> {
		const expression_argument &arg;
	public:
		calculate_size_expression(const expression_argument &arg);

		std::uint16_t operator()(const evaluated_expression &expr) const;
		template <typename T> std::uint16_t operator()( const T &expr) const;
	};

	class calculate_size : public boost::static_visitor<std::uint16_t> {
	public:
		std::uint16_t operator()(const stack_argument& arg) const;
		std::uint16_t operator()(const expression_argument &arg) const;
		std::uint16_t operator()(const data &) const;
		std::uint16_t operator()(const label &) const;
		std::uint16_t operator()(const instruction &instruction) const;
	};

	std::uint16_t output_size(const statement &statement);
	std::uint16_t output_size(const argument &arg);
	std::uint16_t output_size(const expression_argument &arg, const expression &expr);

	std::ostream& operator<< (std::ostream& stream, stack_operation operation);
	std::ostream& operator<< (std::ostream& stream, label_type labelType);
	std::ostream& operator<< (std::ostream& stream, const stack_argument &arg);
	std::ostream& operator<< (std::ostream& stream, const expression_argument &arg);
	std::ostream& operator<< (std::ostream& stream, const label &label);
	std::ostream& operator<< (std::ostream& stream, const instruction &instruction);
	std::ostream& operator<< (std::ostream& stream, const data &label);
}}
