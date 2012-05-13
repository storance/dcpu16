#include <boost/variant.hpp>

#include "symbol_table.hpp"

#include <iomanip>
#include <exception>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace dcpu::ast;
using namespace dcpu::lexer;
using namespace boost;
using namespace boost::algorithm;

namespace dcpu {
	/*************************************************************************
	 *
	 * duplicate_symbol_error
	 *
	 *************************************************************************/
	duplicate_symbol_error::duplicate_symbol_error(const string &name, location_ptr &first_location) {
		message = str(format("redefinition of symbol '%'; previously defined at %s") % name % first_location);
	}

	duplicate_symbol_error::~duplicate_symbol_error() throw() {}

	const char *duplicate_symbol_error::what() const throw() {
		return message.c_str();
	}

	/*************************************************************************
	 *
	 * no_global_label_error
	 *
	 *************************************************************************/
	no_global_label_error::no_global_label_error(const string &name) {
		message = str(format("local label '%s' defined before any non-local labels") % name);
	}

	no_global_label_error::~no_global_label_error() throw() {

	}

	const char *no_global_label_error::what() const throw() {
		return message.c_str();
	}

	/*************************************************************************
	 *
	 * undefined_symbol_error
	 *
	 *************************************************************************/
	undefined_symbol_error::undefined_symbol_error(const string &name) {
		message = str(format("undefined symbol '%s'") % name);
	}

	undefined_symbol_error::~undefined_symbol_error() throw() {}

	const char *undefined_symbol_error::what() const throw() {
		return message.c_str();
	}

	/*************************************************************************
	 *
	 * symbol
	 *
	 *************************************************************************/

	symbol::symbol(const location_ptr &location, symbol_type type, const string name, std::uint16_t offset)
		: locatable(location), type(type), name(name), offset(offset), equ_expr(nullptr) {}

	/*************************************************************************
	 *
	 * symbol_table
	 *
	 *************************************************************************/
	symbol *symbol_table::last_global_before(uint16_t offset) {
		for (auto it = symbols.rbegin(); it != symbols.rend(); it++) {
			if (it->type == symbol_type::GLOBAL_LABEL && it->offset < offset) {
				return &*it;
			}
		}

		return nullptr;
	}

	void symbol_table::add_label(const label &label, uint16_t offset) {
		string name = full_name(label.name, offset);

		symbol_type type = symbol_type::GLOBAL_LABEL;
		if (label.type == label_type::LOCAL) {
			type = symbol_type::LOCAL_LABEL;
		}

		auto existing_symbol = lookup_table.find(name);
		if (existing_symbol != lookup_table.end()) {
			throw duplicate_symbol_error(name, existing_symbol->second.location);
		}

		add_symbol(symbol(label.location, type, name, offset));
	}

	void symbol_table::add_location(const location_ptr &location, uint16_t offset) {
		add_symbol(symbol(location, symbol_type::CURRENT_LOCATION, name_for_location(location), offset));
	}

	string symbol_table::name_for_location(const lexer::location_ptr &location) {
		return str(boost::format("#%s") % location);
	}

	void symbol_table::add_symbol(const symbol &_symbol) {
		symbols.push_back(_symbol);
		lookup_table.insert(pair<string, symbol&>(_symbol.name, symbols.back()));
	}

	void symbol_table::equ(expression &expr) {
		symbols.back().equ_expr = &expr;
	}

	symbol *symbol_table::lookup(const string &name, uint16_t offset) {
		auto _symbol = lookup_table.find(full_name(name, offset));
		if (_symbol == lookup_table.end()) {
			throw undefined_symbol_error(name);
		}
		return &_symbol->second;
	}

	symbol *symbol_table::lookup(const lexer::location_ptr &location, std::uint16_t offset) {
		return lookup(name_for_location(location), offset);
	}

	string symbol_table::full_name(const string &name, uint16_t offset) {
		if (starts_with(name, ".")) {
			symbol *global = last_global_before(offset);

			if (global != nullptr) {
				return global->name + name;
			}
		}

		return name;
	}

	void symbol_table::update_after(uint16_t offset, int amount) {
		for (auto it = symbols.rbegin(); it != symbols.rend() && it->offset > offset; it++) {
			it->offset += amount;
		}
	}

	void symbol_table::dump(std::ostream &out) {
		out << " Value  | Symbol Name" << endl
			 << "---------------------" << endl;
		for (auto &symbol : symbols) {
			// these are not really symbols, so don't display them
			if (symbol.type == symbol_type::CURRENT_LOCATION) {
				continue;
			}

			out << " ";
			if (symbol.type == symbol_type::EQU) {
				out << evaluate(*symbol.equ_expr);
			} else {
				out << boost::format("%#06x") % symbol.offset;
			}

			out << " | " << symbol.name << endl;
		}
	}
}
