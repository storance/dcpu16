#include "Compiler.hpp"

using namespace std;
using namespace dcpu::ast;

namespace dcpu { namespace compiler {
	Compiler::Compiler(ErrorHandlerPtr &errorHandler, SymbolTablePtr &symbolTable)
		: errorHandler(errorHandler), symbolTable(symbolTable) {}

	void Compiler::compile(StatementList &statements) {
		uint16_t pc = 0;
		for (auto& stmt : statements) {
        	stmt->resolveLabels(symbolTable, errorHandler, pc);
    	}

    	if (errorHandler->hasErrors()) {
    		return;
    	}

    	bool anyCompressed = false;
    	do {
    		anyCompressed = false;
    		pc = 0;
    		for (auto& stmt : statements) {
	        	anyCompressed |= stmt->compress(symbolTable, pc);
	    	}
    	} while (anyCompressed);

    	for (auto& stmt : statements) {
        	stmt->compile(output);
    	}
	}

	void Compiler::write(ostream &out, OutputFormat format) {
		for (auto word : output) {
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