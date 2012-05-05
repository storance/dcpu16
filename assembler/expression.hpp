#pragma once

#include <string>
#include <cstdint>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include "token.hpp"
#include "mnemonics.hpp"

namespace dcpu { namespace ast {
	enum class binary_operator : std::uint8_t {
		PLUS,
		MINUS,
		MULTIPLY,
		DIVIDE,
		MODULO,
		SHIFT_LEFT,
		SHIFT_RIGHT,
		AND,
		OR,
		XOR
	};

	enum class unary_operator : std::uint8_t {
		PLUS,
		MINUS,
		NOT,
		BITWISE_NOT
	};

	struct locatable {
		lexer::location_ptr location;

		locatable(const lexer::location_ptr &location);
	};

	struct literal_operand : public locatable {
		std::uint32_t value;

		literal_operand(const lexer::location_ptr &location, std::uint32_t value);

		bool operator==(const literal_operand& other) const;
	};

	struct symbol_operand : public locatable {
		std::string label;
		std::uint16_t *pc;

		symbol_operand(const lexer::location_ptr &location, const std::string &label);
		symbol_operand(const lexer::location_ptr &location, const std::string &label, std::uint16_t &pc);

		bool operator==(const symbol_operand& other) const;
	};

	struct register_operand : public locatable {
		registers _register;

		register_operand(const lexer::location_ptr &location, registers _register);

		bool operator==(const register_operand& other) const;
	};

	struct current_position_operand : public locatable {
		boost::optional<std::uint16_t> pc;

		current_position_operand(const lexer::location_ptr &location);
		current_position_operand(const lexer::location_ptr &location, std::uint16_t pc);

		bool operator==(const current_position_operand& other) const;
	};

	struct evaluated_expression : public locatable {
		boost::optional<registers> _register;
		boost::optional<std::int32_t> value;

		evaluated_expression(const lexer::location_ptr &location, registers _register);
		evaluated_expression(const lexer::location_ptr &location, registers _register, std::int32_t value);
		evaluated_expression(const lexer::location_ptr &location, std::int32_t value);

		bool operator==(const evaluated_expression& other) const;
	};

	struct invalid_expression : public locatable {
		invalid_expression(const lexer::location_ptr &location);

		bool operator==(const invalid_expression& other) const;
	};

	struct unary_operation;
	struct binary_operation;

	typedef boost::variant<
		current_position_operand,
		invalid_expression,
		literal_operand,
		symbol_operand,
		register_operand,
		evaluated_expression,
		boost::recursive_wrapper<unary_operation>,
		boost::recursive_wrapper<binary_operation> > expression;

	typedef boost::optional<expression> optional_expression;

	struct unary_operation : public locatable {
		unary_operator _operator;
		expression operand;

		unary_operation(const lexer::location_ptr &location, unary_operator _operator, const expression &operand);

		bool operator==(const unary_operation& other) const;
	};

	struct binary_operation : public locatable {
		binary_operator _operator;
		expression left;
		expression right;

		binary_operation(const lexer::location_ptr &location, binary_operator _operator, const expression &left,
			const expression &right);

		bool operator==(const binary_operation& other) const;
	};

	class get_location : public boost::static_visitor<lexer::location_ptr> {
	public:
		lexer::location_ptr operator()(const locatable &locatable) const;

		template <typename T> lexer::location_ptr operator()(const T &) const;
	};

	bool evaluated(const expression &expr);
	bool evaluatable(const expression &expr);
	bool evaluates_to_literal(const expression &expr);
	expression evaluate(const expression &expr);
	lexer::location_ptr locate(const expression &expr);
	
	std::ostream& operator<< (std::ostream& stream, unary_operator);
	std::ostream& operator<< (std::ostream& stream, binary_operator);
	std::ostream& operator<< (std::ostream& stream, const unary_operation &expr);
	std::ostream& operator<< (std::ostream& stream, const binary_operation &expr);
	std::ostream& operator<< (std::ostream& stream, const evaluated_expression &expr);
	std::ostream& operator<< (std::ostream& stream, const register_operand &expr);
	std::ostream& operator<< (std::ostream& stream, const symbol_operand &expr);
	std::ostream& operator<< (std::ostream& stream, const literal_operand &expr);
	std::ostream& operator<< (std::ostream& stream, const current_position_operand &expr);
	std::ostream& operator<< (std::ostream& stream, const invalid_expression &expr);
}}
