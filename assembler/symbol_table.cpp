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
		: locatable(location), type(type), name(name), data(offset) {}

	void symbol::make_equ(expression &expr) {
		type = symbol_type::EQU;
		data = &expr;
	}

	bool symbol::has_offset() {
		return data.which() == 0;
	}

	uint16_t& symbol::offset() {
		return boost::get<uint16_t>(data);
	}

	bool symbol::is_evaluatable() {
		if (data.which() == 0) {
			return true;
		} else {
			return evaluatable(*boost::get<expression*>(data));
		}
	}

	evaluated_expression symbol::evaluate(const location_ptr &location) {
		if (data.which() == 0) {
			return evaluated_expression(location, offset());
		} else {
			auto eval_expr = apply_visitor(expression_evaluator(), *boost::get<expression*>(data));
			eval_expr.location = location;

			return eval_expr;
		}
	}

	/*************************************************************************
	 *
	 * base_symbol_visitor
	 *
	 *************************************************************************/

	base_symbol_visitor::base_symbol_visitor(symbol_table *table) : pc(0), table(table) {}

	base_symbol_visitor::base_symbol_visitor(uint16_t pc, symbol_table *table) : pc(pc), table(table) {}

	/*************************************************************************
	 *
	 * build_symbol_table
	 *
	 *************************************************************************/
	build_symbol_table::build_symbol_table(error_handler_ptr &error_handler, symbol_table *table)
			: base_symbol_visitor(table), error_handler(error_handler) {}

	void build_symbol_table::operator()(const label &label) {
		try {
			table->add_label(label, pc);
		} catch (std::exception &e) {
			error_handler->error(label.location, e.what());
		}
	}

	void build_symbol_table::operator()(const current_position_operand &expr) {
		table->add_location(expr.location, pc);
	}

	void build_symbol_table::operator()(const binary_operation &expr) {
		apply_visitor(*this, expr.left);
		apply_visitor(*this, expr.right);
	}

	void build_symbol_table::operator()(const unary_operation &expr) {
		apply_visitor(*this, expr.operand);
	}

	void build_symbol_table::operator()(const expression_argument &arg) {
		if (evaluated(arg.expr)) {
			return;
		}

		apply_visitor(*this, arg.expr);
	}

	void build_symbol_table::operator()(const instruction &instruction) {
		apply_visitor(*this, instruction.a);

		if (instruction.b) {
			apply_visitor(*this, *instruction.b);
		}
	}

	template <typename T> void build_symbol_table::operator()(const T &) {}

	/*************************************************************************
	 *
	 * resolve_symbols
	 *
	 *************************************************************************/
	resolve_symbols::resolve_symbols(error_handler_ptr &error_handler, symbol_table *table)
			: base_symbol_visitor(table), error_handler(error_handler) {}

	resolve_symbols::resolve_symbols(uint16_t pc, error_handler_ptr &error_handler, symbol_table *table)
		: base_symbol_visitor(pc, table), error_handler(error_handler) {}

	void resolve_symbols::operator()(symbol_operand &expr) {
		try {
			expr.resolved_symbol = table->lookup(expr.name, pc);
		} catch (std::exception &e) {
			error_handler->error(expr.location, e.what());
		}
	}

	void resolve_symbols::operator()(current_position_operand &expr) {
		try {
			expr.resolved_symbol = table->lookup(expr.location, pc);
		} catch (std::exception &e) {
			// we should never get here
			throw runtime_error(str(boost::format("internal compiler error: failed to resolve $ at %s") %
					expr.location));
		}
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
	}

	template <typename T>
	void resolve_symbols::operator()( T &) {}

	/*************************************************************************
	 *
	 * compress_expressions
	 *
	 *************************************************************************/
	compress_expressions::compress_expressions(symbol_table* table) : base_symbol_visitor(table) {}

	compress_expressions::compress_expressions(uint16_t pc, symbol_table* table)
			: base_symbol_visitor(pc, table) {}

	bool compress_expressions::operator()(expression_argument &arg) {
		if (evaluated(arg.expr) || !evaluatable(arg.expr)) {
			return false;
		}

		uint8_t expr_size = output_size(arg, evaluate(arg.expr));
		if (arg.next_word_required && !expr_size) {
			arg.next_word_required = false;
			table->compress_after(pc+1);

			return true;
		}

		return false;
	}

	bool compress_expressions::operator()(instruction &instruction) {
		// argument b can never be compressed, so don't both with it
		return apply_visitor(*this, instruction.a);
	}

	template <typename T>
	bool compress_expressions::operator()( T &) {
		return false;
	}

	/*************************************************************************
	 *
	 * symbol_table
	 *
	 *************************************************************************/
	symbol *symbol_table::last_global_before(uint16_t offset) {
		for (auto it = symbols.rbegin(); it != symbols.rend(); it++) {
			if (it->type == symbol_type::GLOBAL_LABEL && it->offset() < offset) {
				return &*it;
			}
		}

		return nullptr;
	}

	void symbol_table::add_label(const label &label, uint16_t offset) {
		string name = resolve_full_name(label.name, offset);

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
		symbols.back().make_equ(expr);
	}

	symbol *symbol_table::lookup(const string &name, uint16_t offset) {
		auto _symbol = lookup_table.find(resolve_full_name(name, offset));
		if (_symbol == lookup_table.end()) {
			throw undefined_symbol_error(name);
		}
		return &_symbol->second;
	}

	symbol *symbol_table::lookup(const lexer::location_ptr &location, std::uint16_t offset) {
		return lookup(name_for_location(location), offset);
	}

	string symbol_table::resolve_full_name(const string &name, uint16_t offset) {
		if (starts_with(name, ".")) {
			symbol *global = last_global_before(offset);

			if (global != nullptr) {
				return global->name + name;
			}
		}

		return name;
	}

	void symbol_table::compress_after(uint16_t offset) {
		for (auto it = symbols.rbegin(); it != symbols.rend(); it++) {
			if (it->has_offset()) {
				if (it->offset() < offset) {
					break;
				}

				--it->offset();
			}
		}
	}

	void symbol_table::build(const ast::statement_list &statements, error_handler_ptr &error_handler) {
		uint16_t last_pc = 0;
		auto builder = build_symbol_table(error_handler, this);
		for (auto &stmt : statements) {
			apply_visitor(builder, stmt);

			last_pc = builder.pc;
			builder.pc += output_size(stmt);

			if (builder.pc < last_pc) {
				error_handler->error(get_location(stmt), "generated output exceeds DCPU-16 memory size");
			}
		}
	}

	void symbol_table::resolve(ast::statement_list &statements, error_handler_ptr &error_handler) {
		auto resolver = resolve_symbols(error_handler, this);
		for (auto &stmt : statements) {
			apply_visitor(resolver, stmt);
			resolver.pc += output_size(stmt);
		}

		bool compressed = false;
		do {
			compressed = false;
			auto compressor = compress_expressions(this);
			for (auto& stmt : statements) {
				compressed |= apply_visitor(compressor, stmt);
				compressor.pc += output_size(stmt);
			}
		} while (compressed);
	}

	void symbol_table::dump() {
		cout << " Value  | Symbol Name" << endl
			 << "---------------------" << endl;
		for (auto &symbol : symbols) {
			// these are not really symbols, so don't display them
			if (symbol.type == symbol_type::CURRENT_LOCATION) {
				continue;
			}

			cout << " ";
			if (symbol.type == symbol_type::EQU) {
				cout << symbol.evaluate(symbol.location);
			} else {
				cout << boost::format("%#06x") % symbol.offset();
			}

			cout << " | " << symbol.name << endl;
		}
	}
}
