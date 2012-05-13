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

const unsigned int MAX_COMPRESS_ITERATIONS = 1000;

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
	 * base_symbol_visitor
	 *
	 *************************************************************************/
	base_symbol_visitor::base_symbol_visitor(symbol_table *table, uint32_t pc) : table(table), pc(pc) {}

	/*************************************************************************
	 *
	 * build_symbol_table
	 *
	 *************************************************************************/
	build_symbol_table::build_symbol_table(symbol_table *table, logging::log &logger, uint32_t pc)
			: base_symbol_visitor(table, pc), logger(logger) {}

	void build_symbol_table::operator()(const label &label) const {
		try {
			table->add_label(label, pc);
		} catch (std::exception &e) {
			logger.error(label.location, e.what());
		}
	}

	void build_symbol_table::operator()(const current_position_operand &expr) const {
		table->add_location(expr.location, pc);
	}

	void build_symbol_table::operator()(const binary_operation &expr) const {
		apply_visitor(*this, expr.left);
		apply_visitor(*this, expr.right);
	}

	void build_symbol_table::operator()(const unary_operation &expr) const {
		apply_visitor(*this, expr.operand);
	}

	void build_symbol_table::operator()(const expression_argument &arg) const {
		if (evaluated(arg.expr)) {
			return;
		}

		apply_visitor(*this, arg.expr);
	}

	void build_symbol_table::operator()(const instruction &instruction) const {
		apply_visitor(*this, instruction.a);

		if (instruction.b) {
			apply_visitor(*this, *instruction.b);
		}
	}

	void build_symbol_table::operator()(ast::equ_directive &equ) const {
		table->equ(equ.value);
	}

	/*************************************************************************
	 *
	 * resolve_symbols
	 *
	 *************************************************************************/
	resolve_symbols::resolve_symbols(symbol_table *table, logging::log &logger, uint32_t pc,
			bool allow_forward_refs) : base_symbol_visitor(table, pc), logger(logger),
			allow_forward_refs(allow_forward_refs) {}

	void resolve_symbols::operator()(symbol_operand &expr) const {
		try {
			expr.resolved_symbol = table->lookup(expr.name, pc);
			if (!allow_forward_refs && expr.resolved_symbol->offset > pc) {
				logger.error(expr.location, "forward symbol references are not allowed here");
			}
		} catch (std::exception &e) {
			logger.error(expr.location, e.what());
		}
	}

	void resolve_symbols::operator()(current_position_operand &expr) const {
		try {
			expr.resolved_symbol = table->lookup(expr.location, pc);
		} catch (std::exception &e) {
			// we should never get here
			throw runtime_error(str(boost::format("internal compiler error: failed to resolve $ at %s") %
					expr.location));
		}
	}

	void resolve_symbols::operator()(binary_operation &expr) const {
		apply_visitor(*this, expr.left);
		apply_visitor(*this, expr.right);
	}

	void resolve_symbols::operator()(unary_operation &expr) const {
		apply_visitor(*this, expr.operand);
	}

	void resolve_symbols::operator()(expression_argument &arg) const {
		if (evaluated(arg.expr)) {
			return;
		}

		apply_visitor(resolve_symbols(table, logger, pc, true), arg.expr);
	}

	void resolve_symbols::operator()(instruction &instruction) const {
		apply_visitor(*this, instruction.a);

		if (instruction.b) {
			apply_visitor(*this, *instruction.b);
		}
	}

	void resolve_symbols::operator()(ast::equ_directive &equ) const {
		resolve_symbols resolver(table, logger, pc, true);
		apply_visitor(resolver, equ.value);
	}

	void resolve_symbols::operator()(ast::fill_directive &fill) const {
		apply_visitor(resolve_symbols(table, logger, pc, false), fill.count);
		apply_visitor(resolve_symbols(table, logger, pc, true), fill.value);
	}

	/*************************************************************************
	 *
	 * compress_expressions
	 *
	 *************************************************************************/
	compress_expressions::compress_expressions(symbol_table* table, uint32_t pc)
			: base_symbol_visitor(table, pc) {}

	bool compress_expressions::operator()(expression_argument &arg) const {
		if (evaluated(arg.expr) || !evaluatable(arg.expr)) {
			return false;
		}

		uint8_t expr_size = output_size(arg, evaluate(arg.expr));
		if (arg.cached_size != expr_size) {
			table->update_after(pc + arg.cached_size, expr_size - arg.cached_size);
			arg.cached_size = expr_size;

			return true;
		}

		return false;
	}

	bool compress_expressions::operator()(instruction &instruction) const {
		// argument b can never be compressed, so don't both with it
		return apply_visitor(*this, instruction.a);
	}

	bool compress_expressions::operator()(ast::fill_directive &fill) const {
		if (!evaluatable(fill.count)) {
			throw invalid_argument("fill count expression can not be evaluated");
		}

		auto evaled_count = evaluate(fill.count);
		if (evaled_count._register) {
			throw invalid_argument("fill count expression must evaluate to a literal");
		}

		auto count = *evaled_count.value;
		if (fill.cached_size != count) {
			table->update_after(pc, count - fill.cached_size);
			fill.cached_size = count;

			return true;
		}

		return false;
	}

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

	void symbol_table::build(ast::statement_list &statements, logging::log &logger) {
		uint32_t pc = 0;
		for (auto &stmt : statements) {
			apply_visitor(build_symbol_table(this, logger, pc), stmt);

			pc += output_size(stmt);
		}
	}

	void symbol_table::resolve(ast::statement_list &statements, logging::log &logger) {
		uint32_t pc = 0;
		for (auto &stmt : statements) {
			apply_visitor(resolve_symbols(this, logger, pc), stmt);
			pc += output_size(stmt);
		}

		// if we have encountered errors, don't bother attempting to compress
		if (logger.has_errors()) {
			return;
		}

		bool updated = true;
		for (unsigned int i = 0; i < MAX_COMPRESS_ITERATIONS && updated; i++) {
			updated = false;
			pc = 0;
			for (auto& stmt : statements) {
				updated |= apply_visitor(compress_expressions(this, pc), stmt);
				pc += output_size(stmt);
			}
		}

		if (updated) {
			// if updated is still true then we must have hit max iterations
			throw runtime_error("internal compiler error: failed to convert label references to short literal form "
					"after 1000 iterations");
		}

		if (pc > UINT16_MAX) {
			logger.error(get_location(statements.back()), "output exceeds 65,535 words");
		}
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
				cout << evaluate(*symbol.equ_expr);
			} else {
				cout << boost::format("%#06x") % symbol.offset;
			}

			cout << " | " << symbol.name << endl;
		}
	}
}
