#include "Compiler.hpp"

namespace dcpu { namespace compiler {
	Compiler::Compiler(StatementList &statements, ErrorHandler &errorHandler, SymbolTable &table)
		: _statements(statements), _errorHandler(errorHandler), _table(table), _format(format) {}

	void Compiler::compile() {
		for (auto& stmt : _statements) {
        	stmt->evaluateExpressions(_table, _errorHandler);
    	}

    	bool anyCompressed = false;
    	do {
    		anyCompressed = false;
    		for (auto& stmt : _statements) {
	        	anyCompressed |= stmt->compress(_table);
	    	}
    	} while (anyCompressed)

    	for (auto& stmt : _statements) {
        	stmt->compile(_output);
    	}
	}

	void Compiler::write(ostream &out, OutputFormat format) {
		for (auto word : _output) {
	        uint8_t b1 = word & 0xff;
	        uint8_t b2 = (word >> 8) & 0xff;

	        if (format == OutputFormat::BigEndian) {
	            out.put(b1);
	            out.put(b2);
	        } else {
	            out.put(b2);
	            out.put(b1);
	        }
	    }
	}
}}