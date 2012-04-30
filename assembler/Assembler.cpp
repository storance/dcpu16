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

    Lexer lexer(storage, inputFile);
    lexer.parse();

    Parser parser(lexer);
    parser.parse();

    Compiler compiler(parser.errorHandler, parser.symbolTable);
    compiler.compile(parser.statements);

    if (parser.errorHandler->hasErrors()) {
        parser.errorHandler->summary();
        return 1;
    }

    ofstream out(outputFile, ios_base::binary | ios_base::out);
    if (!out) {
        cerr << "Failed to open file " <<  outputFile << " for write" << endl;
        return 1;
    }
    compiler.write(out);
}

