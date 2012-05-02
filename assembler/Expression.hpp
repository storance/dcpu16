#pragma once

#include <string>
#include <cstdint>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include "Token.hpp"
#include "Mnemonics.hpp"

namespace dcpu { namespace ast {
	enum class BinaryOperator : std::uint8_t {
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

	enum class UnaryOperator : std::uint8_t {
		PLUS,
		MINUS,
		NOT,
		BITWISE_NOT
	};

	struct Locatable {
		lexer::location_t location;

		Locatable(lexer::location_t &location);
	};

	struct LiteralOperand : public Locatable {
		std::uint32_t value;

		LiteralOperand(lexer::location_t &location, std::uint32_t value);

		bool operator==(const LiteralOperand& other) const;
	};

	struct LabelOperand : public Locatable {
		std::string label;
		boost::optional<std::uint16_t> pc;

		LabelOperand(lexer::location_t &location, const std::string &label);
		LabelOperand(lexer::location_t &location, const std::string &label, std::uint16_t pc);

		bool operator==(const LabelOperand& other) const;
	};

	struct RegisterOperand : public Locatable {
		Register _register;

		RegisterOperand(lexer::location_t &location, Register _register);

		bool operator==(const RegisterOperand& other) const;
	};

	struct EvaluatedExpression : public Locatable {
		boost::optional<Register> _register;
		boost::optional<std::int32_t> value;

		EvaluatedExpression(lexer::location_t &location, Register _register);
		EvaluatedExpression(lexer::location_t &location, Register _register, std::int32_t value);
		EvaluatedExpression(lexer::location_t &location, std::int32_t value);

		bool operator==(const EvaluatedExpression& other) const;
	};

	struct UnaryOperation;
	struct BinaryOperation;

	typedef boost::variant<
		LiteralOperand,
		LiteralOperand,
		RegisterOperand,
		EvaluatedExpression,
		boost::recursive_wrapper<UnaryOperation>,
		boost::recursive_wrapper<BinaryOperation>> Expression;

	struct UnaryOperation : public Locatable {
		UnaryOperator _operator;
		Expression operand;

		UnaryOperation(lexer::location_t &location, UnaryOperator _operator, const Expression &operand);

		bool operator==(const UnaryOperation& other) const;
	};

	struct BinaryOperation : public Locatable {
		BinaryOperator _operator;
		Expression left;
		Expression right;

		BinaryOperation(lexer::location_t &location, BinaryOperator _operator, const Expression &left,
			const Expression &right);

		bool operator==(const BinaryOperation& other) const;
	};

	bool evaluated(const Expression &expr);
	bool evaluatable(const Expression &expr);
	Expression evaluate(Expression &expr);

	std::ostream& operator<< (std::ostream& stream, UnaryOperator);
	std::ostream& operator<< (std::ostream& stream, BinaryOperator);
	std::ostream& operator<< (std::ostream& stream, const UnaryOperation &expr);
	std::ostream& operator<< (std::ostream& stream, const BinaryOperation &expr);
	std::ostream& operator<< (std::ostream& stream, const EvaluatedExpression &expr);
	std::ostream& operator<< (std::ostream& stream, const RegisterOperand &expr);
	std::ostream& operator<< (std::ostream& stream, const LabelOperand &expr);
	std::ostream& operator<< (std::ostream& stream, const LiteralOperand &expr);
}}