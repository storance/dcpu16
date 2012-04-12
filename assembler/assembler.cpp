#include <inttypes.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <exception>
#include <stdexcept>

#include <boost/variant.hpp>
#include <boost/format.hpp>

#include "statement.hpp"
#include "assembler.hpp"
#include "compiler.hpp"

using namespace std;
namespace qi = boost::spirit::qi;

void SymbolTable::add(const string &label, word_t offset) {

}

word_t *SymbolTable::lookup(const string &label) {
	return NULL;
}

list<ast::statement> parse_file(const string& filename) {
	typedef std::string::const_iterator iterator_type;

	ifstream in(filename.c_str(), ios_base::in);

    if (!in) {
        throw runtime_error(str(boost::format("Failed to open file %s") % filename));
    }

    string storage;
    in.unsetf(ios::skipws);
    copy(istream_iterator<char>(in), istream_iterator<char>(),
    	back_inserter(storage));

    iterator_type start = storage.begin();
    iterator_type end = storage.end();

    grammar::statement<iterator_type> stmt_grammar;
    grammar::skipper<iterator_type> skipper;
    list<ast::statement> result;
    if (qi::phrase_parse(start, end, stmt_grammar, skipper, result)) {
    	return result;
    } else {
    	throw runtime_error("Failed to parse file");
    }
}

int main(int argc, char **argv) {
	if (argc < 2) {
		cout << "Usage: " << argv[0] << " </path/to/dcpu/asm>" << endl;
		return 1;
	}

	list<ast::statement> result = parse_file(argv[1]);

	Compiler compiler;
	compiler.compile(result);
}