#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include "Parser.hpp"
#include "SymbolTable.hpp"
#include "Compiler.hpp"

using namespace std;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::lexer;
using namespace dcpu::parser;

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " </path/to/dcpu/asm>" << endl;
		return 1;
	}

	ifstream in(argv[1], ios_base::in);

    if (!in) {
        cerr << "Failed to open file " <<  argv[1] << endl;
        return 1;
    }

    string storage;
    in.unsetf(ios::skipws);
    copy(istream_iterator<char>(in), istream_iterator<char>(), back_inserter(storage));

    Lexer lexer(storage.begin(), storage.end(), argv[1]);
    lexer.parse();

    ErrorHandler errorHandler;
    SymbolTable table;
    Parser parser(lexer.tokens.begin(), lexer.tokens.end(), errorHandler, table);
    parser.parse();

    if (errorHandler.hasErrors()) {
        errorHandler.summary();
        return 1;
    }

    for (auto& stmt : parser.statements) {
        cout << str(stmt) << endl;
    }
}

