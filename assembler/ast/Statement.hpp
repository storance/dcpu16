#pragma once

#include <string>
#include <list>
#include <vector>
#include <cstdint>

#include <boost/variant.hpp>

#include "Common.hpp"
#include "OpcodeDefinition.hpp"
#include "Argument.hpp"
#include "../Types.hpp"
#include "../Token.hpp"

namespace dcpu { namespace ast {
	struct Instruction : public Locatable {
		Opcode opcode;
		Argument a;
		Argument b;

		Instruction(lexer::location_t& location, Opcode opcode, Argument &a, Argument &b);

		bool operator==(const Instruction& other) const;
	};

	struct Label : public Locatable {
		enum class Type {
			Global,
			Local,
			GlobalNoAttach
		};

		Type type;
		std::string name;

		Label(lexer::location_t &location, const std::string& name);
		Label(lexer::location_t &location, const std::string& name, Type type);

		bool operator==(const Label&) const;
	};

	std::ostream& operator<< (std::ostream& stream, const Label &label);
	std::ostream& operator<< (std::ostream& stream, const Instruction &instruction);

	typedef boost::variant<Instruction, Label> Statement;

	uint8_t size(const Statement &statement);
	void resolveLabels(const Statement &statement);
	void buildSymbolTable(const Statement &statement, SymbolTablePtr &symbolTable);
}}