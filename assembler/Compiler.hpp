#pragma once

#include <vector>
#include <cstdint>

#include "ast/Statement.hpp"
#include "ErrorHandler.hpp"
#include "SymbolTable.hpp"

namespace dcpu { namespace compiler {
	enum class OutputFormat {
		LittleEndian,
		BigEndian
	};

	class Compiler {
		StatementList &_statements;
		ErrorHandler &_errorHandler;
		SymbolTable &_table;

		vector<std::uint16_t> _output;
	public:
		Compiler(StatementList &statements, ErrorHandler &_errorHandler, SymbolTable &_table);

		void compile();
		void write(ostream &out, OutputFormat format=OutputFormat::BigEndian);
	};
}}