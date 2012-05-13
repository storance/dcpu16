#include <system_error>
#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <boost/variant.hpp>

#include "parser.hpp"
#include "compiler.hpp"

using namespace std;
using namespace dcpu;
using namespace dcpu::ast;
using namespace dcpu::compiler;
using namespace dcpu::lexer;
using namespace dcpu::parser;

namespace po = boost::program_options;

const string VERSION="0.1";

void read_file(const string &filename, string &content) {
	ifstream in(filename, ios_base::in);
	in.unsetf(ios_base::skipws);
	if (!in) {
		throw runtime_error(str(boost::format("Failed to open file %s: %s" ) % filename % strerror(errno)));
	}

	copy(istream_iterator<char>(in), istream_iterator<char>(), back_inserter(content));
	in.close();

	if (in.fail() && !in.eof()) {
		throw runtime_error(str(boost::format("Failed to read from file %s: %s" ) % filename % strerror(errno)));
	}
}

void parse_file(const string &filename, logging::log &logger, statement_list &statements) {
	string content;
	read_file(filename, content);

	lexer::lexer _lexer(content, filename, logger);
	_lexer.parse();

	parser::parser _parser(_lexer, statements);
	_parser.parse();
}

void handle_errors(logging::log &logger) {
	if (logger.has_errors()) {
		logger.summary();
		exit(1);
	}
}

void print_ast(statement_list &statements) {
	for (auto &statement : statements) {
		cout << statement << endl;
	}
}

void usage(const char *program_name, const po::options_description &visible_options) {
	cout << "Usage: " << program_name << " [OPTIONS] <input-file>" << endl;
	cout << visible_options << endl;
}

int main(int argc, char **argv) {
	bool ast_print;
	bool symbols_print;
	bool syntax_only;
	string input_file;
	string output_file;

	po::options_description visible_options("OPTIONS");
	visible_options.add_options()
	    ("help,h", "Displays this information")
	    ("version,v", "Displays the assembler version")
	    ("ast-print", po::bool_switch(&ast_print), "prints the abstract syntax tree")
	    ("symbols-print", po::bool_switch(&symbols_print), "prints all symbols and their memory location")
	    ("syntax-only", po::bool_switch(&syntax_only), "performs a syntax check only and does not produce any output")
	    ("include-path,I", po::value<vector<string> >(), "Add the directory to the list of directories to be search for includes.")
	    ("output-file,o", po::value<string>(&output_file), "Write output to the specified file.  Use - for stdout.");

	po::options_description hidden_options("Hidden options");
	hidden_options.add_options()
		("input-file", po::value<string>(&input_file), "the input file");

	po::options_description cmdline_options;
	cmdline_options.add(visible_options).add(hidden_options);

	po::positional_options_description positional_args;
	positional_args.add("input-file", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).
	          options(cmdline_options).positional(positional_args).run(), vm);
	po::notify(vm);

	if (vm.count("help")) {
		usage(argv[0], visible_options);
		return 0;
	}

	if (vm.count("version")) {
		cout << "DCPU-16 assembler v." << VERSION << endl;
		cout << "Copyright (c) 2012 Steven Torance" << endl;
		return 0;
	}

	if (input_file.length() == 0) {
		cerr << "Missing required input-file argument" << endl << endl;
		usage(argv[0], visible_options);
		return 1;
	}

	compiler_mode mode = compiler_mode::NORMAL;
	if (ast_print) {
		mode = compiler_mode::PRINT_AST;
	} else if (symbols_print) {
		mode = compiler_mode::PRINT_SYMBOLS;
	} else if (syntax_only) {
		mode = compiler_mode::SYNTAX_ONLY;
	}

	if (output_file.length() == 0 && mode == compiler_mode::NORMAL) {
		string::size_type ext_index = input_file.rfind('.');

		if (ext_index != string::npos) {
		    output_file = input_file.substr(0, ext_index) + ".bin";
		} else {
			output_file = input_file + ".bin";
		}
	} else {
		output_file = "-";
	}

	ofstream fout;
	if (output_file != "-") {
		fout.open(output_file, ios_base::binary | ios_base::out);
		if (!fout) {
			throw runtime_error(str(boost::format("Failed to open file %s for write: %s" )
					% output_file % strerror(errno)));
		}
	}

	ostream &out = (output_file == "-" ? cout : fout);

	try {
		logging::log logger;
		statement_list statements;
		parse_file(input_file, logger, statements);

		symbol_table table;
		compiler::compiler _compiler(logger, table, statements);
		_compiler.compile(out, mode);
	} catch (std::exception &e) {
		cerr << e.what() << endl;
		return 1;
	}
}

