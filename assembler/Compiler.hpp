#pragma once

#include <vector>
#include <cstdint>

#include "ast/Statement.hpp"
#include "ast/Common.hpp"
#include "ErrorHandler.hpp"
#include "SymbolTable.hpp"
#include "Types.hpp"

namespace dcpu { namespace compiler {
	enum class OutputFormat {
		LittleEndian,
		BigEndian
	};

	class Compiler {
	private:
		ErrorHandlerPtr errorHandler;
		SymbolTablePtr symbolTable;

		std::vector<std::uint16_t> output;
	public:
		Compiler(ErrorHandlerPtr &errorHandler, SymbolTablePtr &symbolTable);

		void compile(StatementList &statement);
		void write(std::ostream &out, OutputFormat format=OutputFormat::BigEndian);
	};
}}