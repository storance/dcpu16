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
	undefined_symbol_error::undefined_symbol_error(const std::string &name) {
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
	symbol::symbol(const location_ptr &location, const string name, bool global, uint16_t offset)
		: locatable(location), name(name), offset(offset), global(global) {}


	/*************************************************************************
	 *
	 * build_symbol_table
	 *
	 *************************************************************************/
	build_symbol_table::build_symbol_table(error_handler_ptr &error_handler, symbol_table *table)
		: pc(0), error_handler(error_handler), table(table) {}

	void build_symbol_table::operator()(const label &label) {
		try {
			table->add(label, pc);
		} catch (std::exception &e) {
			error_handler->error(label.location, e.what());
		}
	}

	void build_symbol_table::operator()(const instruction &instruction) {
		pc += output_size(statement(instruction));
	}

	/*************************************************************************
	 *
	 * resolve_symbols
	 *
	 *************************************************************************/
	resolve_symbols::resolve_symbols(error_handler_ptr &error_handler, symbol_table *table)
				: pc(0), error_handler(error_handler), table(table) {}

	resolve_symbols::resolve_symbols(uint16_t pc, error_handler_ptr &error_handler, symbol_table *table)
		: pc(pc), error_handler(error_handler), table(table) {}

	void resolve_symbols::operator()(symbol_operand &expr) {
		try {
			expr.pc = table->lookup(expr.label, pc);
		} catch (std::exception &e) {
			error_handler->error(expr.location, e.what());
		}
	}

	void resolve_symbols::operator()(current_position_operand &expr) {
		expr.pc = pc;
	}

	void resolve_symbols::operator()(binary_operation &expr) {
		apply_visitor(*this, expr.left);
		apply_visitor(*this, expr.right);
	}

	void resolve_symbols::operator()(unary_operation &expr) {
		apply_visitor(*this, expr.operand);
	}

	void resolve_symbols::operator()(expression_argument &arg) {
		if (evaluated(arg.expr)) {
			return;
		}

		apply_visitor(*this, arg.expr);
	}

	void resolve_symbols::operator()(instruction &instruction) {
		apply_visitor(*this, instruction.a);

		if (instruction.b) {
			apply_visitor(*this, *instruction.b);
		}

		pc += output_size(statement(instruction));
	}

	template <typename T>
	void resolve_symbols::operator()( const T &) {

	}

	/*************************************************************************
	 *
	 * update_current_position
	 *
	 *************************************************************************/

	update_current_position::update_current_position(uint16_t pc) : pc(pc) {}

	void update_current_position::operator()(current_position_operand &expr) const {
		expr.pc = pc;
	}

	void update_current_position::operator()(binary_operation &expr) const {
		apply_visitor(*this, expr.left);
		apply_visitor(*this, expr.right);
	}

	void update_current_position::operator()(unary_operation &expr) const {
		apply_visitor(*this, expr.operand);
	}

	template <typename T>
	void update_current_position::operator()( const T &) const {

	}

	/*************************************************************************
	 *
	 * compress_expressions
	 *
	 *************************************************************************/
	compress_expressions::compress_expressions(symbol_table* table) : pc(0), table(table) {}

	compress_expressions::compress_expressions(std::uint16_t pc, symbol_table* table)
			: pc(pc), table(table) {}

	bool compress_expressions::operator()(expression_argument &arg) {
		if (evaluated(arg.expr) || !evaluatable(arg.expr)) {
			return false;
		}

		apply_visitor(update_current_position(pc), arg.expr);

		uint8_t expr_size = output_size(arg, evaluate(arg.expr));
		if (arg.next_word_required && !expr_size) {
			arg.next_word_required = false;
			table->compress_after(pc);

			return true;
		}

		return false;
	}

	bool compress_expressions::operator()(instruction &instruction) {
		bool compressed = apply_visitor(*this, instruction.a);

		if (instruction.b) {
			compressed |= apply_visitor(*this, *instruction.b);
		}

		pc += output_size(statement(instruction));

		return compressed;
	}

	template <typename T>
	bool compress_expressions::operator()( const T &) {
		return false;
	}

	/*************************************************************************
	 *
	 * symbol_table
	 *
	 *************************************************************************/

	symbol *symbol_table::last_global() {
		for (auto it = symbols.rbegin(); it != symbols.rend(); it++) {
			if (it->global) {
				return &*it;
			}
		}

		return nullptr;
	}

	symbol *symbol_table::last_global_before(uint16_t offset) {
		for (auto it = symbols.rbegin(); it != symbols.rend(); it++) {
			if (it->global && it->offset < offset) {
				return &*it;
			}
		}

		return nullptr;
	}

	void symbol_table::add(const label &label, uint16_t offset) {
		string name = label.name;

		if (label.type == label_type::Local) {
			symbol *_symbol = last_global();
			if (!_symbol) {
				throw no_global_label_error(label.name);
			}

			name = _symbol->name + name;
		}

		auto existing_symbol = lookup_table.find(name);
		if (existing_symbol != lookup_table.end()) {
			throw duplicate_symbol_error(name, existing_symbol->second.location);
		}

		symbol entry(label.location, name, label.type == label_type::Global, offset);
		symbols.push_back(entry);
		lookup_table.insert(pair<string, symbol&>(name, symbols.back()));
	}

	uint16_t *symbol_table::lookup(const string &name, std::uint16_t offset) {
		auto _symbol = lookup_table.find(resolve_full_name(name, offset));
		if (_symbol == lookup_table.end()) {
			throw undefined_symbol_error(name);
		}
		return &_symbol->second.offset;
	}

	string symbol_table::resolve_full_name(const std::string &name, std::uint16_t offset) {
		if (!starts_with(name, "..@") && starts_with(name, ".")) {
			symbol *global = last_global_before(offset);

			if (global != nullptr) {
				return global->name + name;
			}
		}

		return name;
	}

	void symbol_table::compress_after(uint16_t offset) {
		for (auto it = symbols.rbegin(); it != symbols.rend() && it->offset > offset; it++) {
			--it->offset;
		}
	}

	void symbol_table::build(const ast::statement_list &statements, error_handler_ptr &error_handler) {
		auto builder = build_symbol_table(error_handler, this);
		for (auto &stmt : statements) {
			apply_visitor(builder, stmt);
		}
	}

	void symbol_table::resolve(const ast::statement_list &statements, error_handler_ptr &error_handler) {
		auto resolver = resolve_symbols(error_handler, this);
		for (auto &stmt : statements) {
			apply_visitor(resolver, stmt);
		}

		bool compressed = false;
		do {
			compressed = false;
			auto compressor = compress_expressions(this);
			for (auto& stmt : statements) {
				compressed |= apply_visitor(compressor, stmt);
			}
		} while (compressed);
	}

	void symbol_table::dump() {
		cout << " Value  | Symbol Name" << endl
			 << "---------------------" << endl;
		for (auto &symbol : symbols) {
			cout << " " << boost::format("%#06x") % symbol.offset << " | " << symbol.name << endl;
		}
	}
}
