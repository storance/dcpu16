#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "expression.hpp"

namespace dcpu { namespace ast {
	enum class argument_position : std::uint8_t {
		A, B
	};

	enum class label_type : std::uint8_t {
		GLOBAL, LOCAL
	};

	struct base_argument : public locatable {
		argument_position position;

		base_argument(const lexer::location_ptr &location, argument_position position);
	};

	struct stack_argument : public base_argument {
		stack_operation operation;

		stack_argument(const lexer::location_ptr &location, argument_position position, stack_operation operation);

		bool operator==(const stack_argument& other) const;
	};

	struct expression_argument : public base_argument {
		bool indirect;
		bool force_next_word;
		uint8_t cached_size;
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

	struct data_directive : public locatable {
		std::vector<std::uint16_t> value;

		data_directive(const lexer::location_ptr &location);
		data_directive(const lexer::location_ptr &location, const std::vector<std::uint16_t> &value);

		bool operator==(const data_directive&) const;
	};

	struct org_directive : public locatable {
		std::uint16_t offset;

		org_directive(const lexer::location_ptr &location, std::uint16_t offset);
		bool operator==(const org_directive&) const;
	};

	struct fill_directive : public locatable {
		expression count;
		expression value;
		uint16_t cached_size;

		fill_directive(const lexer::location_ptr &location, const expression &count, const expression &value);
		bool operator==(const fill_directive&) const;
	};

	struct equ_directive : public locatable {
		expression value;

		equ_directive(const lexer::location_ptr &location, const expression &value);
		bool operator==(const equ_directive&) const;
	};

	struct align_directive : public locatable {
		uint16_t alignment;
		uint16_t cached_size;

		align_directive(const lexer::location_ptr &location, uint16_t alignment);
		bool operator==(const align_directive&) const;
	};

	typedef boost::variant<
			instruction,
			label,
			data_directive,
			org_directive,
			fill_directive,
			equ_directive,
			align_directive> statement;
	typedef std::list<statement> statement_list;

	class calculate_size_expression : public boost::static_visitor<std::uint16_t> {
		const expression_argument &arg;
	public:
		calculate_size_expression(const expression_argument &arg);

		std::uint16_t operator()(const evaluated_expression &expr) const;
		template <typename T> std::uint16_t operator()( const T &expr) const;
	};

	class calculate_size : public boost::static_visitor<std::uint16_t> {
		boost::optional<uint16_t> pc;
	public:
		calculate_size(boost::optional<uint16_t> pc);

		std::uint16_t operator()(const stack_argument& arg) const;
		std::uint16_t operator()(const expression_argument &arg) const;
		std::uint16_t operator()(const data_directive &) const;
		std::uint16_t operator()(const org_directive &) const;
		std::uint16_t operator()(const fill_directive &) const;
		std::uint16_t operator()(const align_directive &) const;
		std::uint16_t operator()(const instruction &instruction) const;
		template <typename T> std::uint16_t operator()( const T &expr) const;
	};

	std::uint16_t output_size(const statement &statement, boost::optional<uint16_t> pc=boost::none);
	std::uint16_t output_size(const argument &arg, boost::optional<uint16_t> pc=boost::none);
	std::uint16_t output_size(const expression_argument &arg, const expression &expr);

	std::ostream& operator<< (std::ostream& stream, label_type labelType);
	std::ostream& operator<< (std::ostream& stream, const stack_argument &arg);
	std::ostream& operator<< (std::ostream& stream, const expression_argument &arg);
	std::ostream& operator<< (std::ostream& stream, const label &label);
	std::ostream& operator<< (std::ostream& stream, const instruction &instruction);
	std::ostream& operator<< (std::ostream& stream, const data_directive &data);
	std::ostream& operator<< (std::ostream& stream, const org_directive &org);
	std::ostream& operator<< (std::ostream& stream, const fill_directive &fill);
	std::ostream& operator<< (std::ostream& stream, const equ_directive &reserve);
	std::ostream& operator<< (std::ostream& stream, const align_directive &reserve);
}}
