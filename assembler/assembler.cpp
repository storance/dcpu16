#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include "parser.hpp"
#include "symbol_table.hpp"
#include "compiler.hpp"

using namespace std;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::compiler;
using namespace dcpu::lexer;
using namespace dcpu::parser;

int main(int argc, char **argv) {
	if (argc < 3) {
		cout << "Usage: " << argv[0] << " </path/to/dcpu/asm> </path/to/output/file>" << endl;
		return 1;
	}

    string inputFile = argv[1];
    string outputFile = argv[2];

	ifstream in(inputFile, ios_base::in);

    if (!in) {
        cerr << "Failed to open file " <<  inputFile << endl;
        return 1;
    }

    string storage;
    in.unsetf(ios::skipws);
    copy(istream_iterator<char>(in), istream_iterator<char>(), back_inserter(storage));

    lexer::lexer _lexer(storage, inputFile);
    _lexer.parse();

    parser::parser _parser(_lexer);
    _parser.parse();

    compiler::compiler _compiler(_lexer.error_handler);
    _compiler.compile(_parser.statements);

    if (_lexer.error_handler->has_errors()) {
    	_lexer.error_handler->summary();
        return 1;
    }

    ofstream out(outputFile, ios_base::binary | ios_base::out);
    if (!out) {
        cerr << "Failed to open file " <<  outputFile << " for write" << endl;
        return 1;
    }
    _compiler.write(out);
}

