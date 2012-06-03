#include <boost/variant.hpp>
#include <boost/format.hpp>

#include <cstring>

#include "compiler.hpp"

using namespace std;
using namespace boost;

const unsigned int MAX_COMPRESS_ITERATIONS = 1000;

namespace dcpu { namespace assembler {
/*************************************************************************
	 *
	 * base_symbol_visitor
	 *
	 *************************************************************************/
	base_symbol_visitor::base_symbol_visitor(symbol_table& table, uint32_t pc) : table(table), pc(pc) {}

	/*************************************************************************
	 *
	 * build_symbol_table
	 *
	 *************************************************************************/
	build_symbol_table::build_symbol_table(symbol_table& table, log &logger, uint32_t pc)
			: base_symbol_visitor(table, pc), logger(logger) {}

	void build_symbol_table::operator()(const label &label) const {
		try {
			table.add_label(label, pc);
		} catch (std::exception &e) {
			logger.error(label.location, e.what());
		}
	}

	void build_symbol_table::operator()(const current_position_operand &expr) const {
		table.add_location(expr.location, pc);
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

	void build_symbol_table::operator()(equ_directive &equ) const {
		table.equ(equ.value);
	}

	/*************************************************************************
	 *
	 * resolve_symbols
	 *
	 *************************************************************************/
	resolve_symbols::resolve_symbols(symbol_table& table, log &logger, uint32_t pc,
			bool allow_forward_refs) : base_symbol_visitor(table, pc), logger(logger),
			allow_forward_refs(allow_forward_refs) {}

	void resolve_symbols::operator()(symbol_operand &expr) const {
		try {
			expr.resolved_symbol = table.lookup(expr.name, pc);
			if (!allow_forward_refs && expr.resolved_symbol->offset > pc) {
				logger.error(expr.location, "forward symbol references are not allowed here");
			}
		} catch (std::exception &e) {
			logger.error(expr.location, e.what());
		}
	}

	void resolve_symbols::operator()(current_position_operand &expr) const {
		try {
			expr.resolved_symbol = table.lookup(expr.location, pc);
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

	void resolve_symbols::operator()(equ_directive &equ) const {
		resolve_symbols resolver(table, logger, pc, true);
		apply_visitor(resolver, equ.value);
	}

	void resolve_symbols::operator()(fill_directive &fill) const {
		apply_visitor(resolve_symbols(table, logger, pc, false), fill.count);
		apply_visitor(resolve_symbols(table, logger, pc, true), fill.value);
	}

	/*************************************************************************
	 *
	 * compress_expressions
	 *
	 *************************************************************************/
	compress_expressions::compress_expressions(symbol_table& table, log &logger, uint32_t pc)
			: base_symbol_visitor(table, pc), logger(logger) {}

	bool compress_expressions::operator()(expression_argument &arg) const {
		if (evaluated(arg.expr) || !evaluatable(arg.expr)) {
			return false;
		}

		uint8_t expr_size = output_size(arg, evaluate(logger, arg.expr, true));
		uint8_t current_size = arg.cached_size;
		if (arg.cached_size != expr_size) {
			arg.cached_size = expr_size;
			table.update_after(pc + current_size, expr_size - current_size);

			// if updating to use short literals causes the expression to no longer fit in a short literal,
			// just force using the next word.
			uint8_t new_size = output_size(arg, evaluate(logger, arg.expr));
			if (new_size > expr_size) {
				table.update_after(pc + expr_size, new_size - expr_size);
				arg.cached_size = new_size;
				arg.force_next_word = true;
			}

			return true;
		}

		return false;
	}

	bool compress_expressions::operator()(instruction &instruction) const {
		// argument b can never be compressed, so don't both with it
		return apply_visitor(*this, instruction.a);
	}

	bool compress_expressions::operator()(fill_directive &fill) const {
		if (!evaluatable(fill.count)) {
			throw invalid_argument("fill count expression can not be evaluated");
		}

		auto evaled_count = evaluate(logger, fill.count, true);
		if (evaled_count._register) {
			throw invalid_argument("fill count expression must evaluate to a literal");
		}

		auto count = *evaled_count.value;
		if (fill.cached_size != count) {
			table.update_after(pc, count - fill.cached_size);
			fill.cached_size = count;

			return true;
		}

		return false;
	}

	bool compress_expressions::operator()(align_directive &align) const {
		uint16_t new_size = output_size(statement(align), pc);

		if (new_size != align.cached_size) {
			table.update_after(pc, new_size - align.cached_size);
			align.cached_size = new_size;

			return true;
		}

		return false;
	}

	/*************************************************************************
	 *
	 * compile_result
	 *
	 *************************************************************************/
	compile_result::compile_result()
		: value(0), next_word() {}

	compile_result::compile_result(std::uint8_t value)
			: value(value), next_word() {}

	compile_result::compile_result(std::uint8_t value, std::uint16_t next_word)
			: value(value), next_word(next_word) {}

	bool compile_result::operator==(const compile_result &result) const {
		return value == value && next_word == next_word;
	}

	std::ostream &operator<<(std::ostream &stream, const compile_result &result) {
		stream << "[value=" << result.value << ",next_word=";
		if (result.next_word) {
			stream << "none";
		} else {
			stream << *result.next_word;
		}

		return stream << "]";
	}

	/*************************************************************************
	 *
	 * expression_compiler
	 *
	 *************************************************************************/

	expression_compiler::expression_compiler(log &logger, const expression_argument &arg)
			: logger(logger), position_a(arg.position == argument_position::A),
			  inside_indirect(arg.indirect), force_next_word(arg.force_next_word) {}

	compile_result expression_compiler::operator()(const evaluated_expression& expr) const {
		if (expr._register) {
			switch (*expr._register) {
			case registers::A:
				return compile_register(expr.value, 0x0, 0x8, 0x10);
			case registers::B:
				return compile_register(expr.value, 0x1, 0x9, 0x11);
			case registers::C:
				return compile_register(expr.value, 0x2, 0xa, 0x12);
			case registers::X:
				return compile_register(expr.value, 0x3, 0xb, 0x13);
			case registers::Y:
				return compile_register(expr.value, 0x4, 0xc, 0x14);
			case registers::Z:
				return compile_register(expr.value, 0x5, 0xd, 0x15);
			case registers::I:
				return compile_register(expr.value, 0x6, 0xe, 0x16);
			case registers::J:
				return compile_register(expr.value, 0x7, 0xf, 0x17);
			case registers::SP:
				return compile_register(expr.value, 0x1b, 0x19, 0x1a);
			case registers::PC:
				return compile_result(0x1c);
			case registers::EX:
				return compile_result(0x1d);
			default:
				throw logic_error((boost::format("%s") % *expr._register ).str());
			}
		}

		if (inside_indirect) {
			return compile_result(0x1e, (uint16_t)*expr.value);
		} else {
			if (!position_a || force_next_word
					|| *expr.value < -1 || *expr.value > 30) {
				return compile_result(0x1f, (uint16_t)*expr.value);
			} else {
				return compile_result(0x21 + *expr.value);
			}
		}
	}

	compile_result expression_compiler::compile_register(boost::optional<int32_t> offset, uint8_t no_indirect,
			uint8_t indirect, uint8_t indirect_offset) const {
		if (!inside_indirect) {
			return compile_result(no_indirect);
		}

		if (offset && (force_next_word || *offset != 0)) {
			return compile_result(indirect_offset, (uint16_t)*offset);
		} else {
			return compile_result(indirect);
		}
	}

	template <typename T>
	compile_result expression_compiler::operator()(const T& expr) const {
		expression eval_expr = evaluate(logger, expression(expr));
		return apply_visitor(*this, eval_expr);
	}

	/*************************************************************************
	 *
	 * argument_compiler
	 *
	 *************************************************************************/
	argument_compiler::argument_compiler(log &logger) : logger(logger) {}

	compile_result argument_compiler::operator()(const expression_argument &arg) const {
		return apply_visitor(expression_compiler(logger, arg), arg.expr);
	}

	compile_result argument_compiler::operator()(const stack_argument &arg) const {
		switch (arg.operation) {
		case stack_operation::PUSH:
		case stack_operation::POP:
			return compile_result(0x18);
		case stack_operation::PEEK:
			return compile_result(0x19);
		default:
			throw logic_error(str(boost::format("%s") % arg.operation));
		}
	}

	/*************************************************************************
	 *
	 * statement_compiler
	 *
	 *************************************************************************/

	statement_compiler::statement_compiler(log &logger, vector<uint16_t> &output)
			: logger(logger), output(output) {}

	template <typename T> void statement_compiler::operator()(const T&) const {

	}

	void statement_compiler::operator()(const instruction &instr) const {
		if (instr.opcode == opcodes::JMP) {
			expression_argument b(instr.location, argument_position::B,
					register_operand(instr.location, registers::PC), false, false);
			statement stmt(instruction(instr.location, opcodes::SET, instr.a, argument(b)));
			apply_visitor(*this, stmt);
			return;
		}

		compile_result a_result, b_result;

		a_result = compile(logger, instr.a);
		if (instr.b) {
			b_result = compile(logger, *instr.b);
		}

		uint16_t encoded_value = static_cast<uint16_t>(instr.opcode)
						| (b_result.value & 0x1f) << 5
						| (a_result.value & 0x3f) << 10;

		output.push_back(encoded_value);
		if (a_result.next_word) {
			output.push_back(*a_result.next_word);
		}

		if (b_result.next_word) {
			output.push_back(*b_result.next_word);
		}
	}

	void statement_compiler::operator()(const data_directive &data) const {
		copy (data.value.begin(), data.value.end(), back_inserter(output));
	}

	void statement_compiler::operator()(const fill_directive &fill) const {
		auto evaled_count = evaluate(logger, fill.count);
		auto evaled_value = evaluate(logger, fill.value);

		if (evaled_count._register) {
			throw invalid_argument("fill count expression must evaluate to a literal");
		}

		if (evaled_value._register) {
			throw invalid_argument("fill value expression must evaluate to a literal");
		}

		uint16_t count = *evaled_count.value;
		uint16_t value = *evaled_value.value;

		for (int i = 0; i < count; i++) {
			output.push_back(value);
		}
	}

	void statement_compiler::operator()(const align_directive &align) const {
		for (int i = 0; i < align.cached_size; i++) {
			output.push_back(0x0001); // SET A, A
		}
	}

	/*************************************************************************
	 *
	 * compiler
	 *
	 *************************************************************************/

	compiler::compiler(log &logger, symbol_table& table, statement_list &statements)
			: logger(logger), table(table), statements(statements) {}

	void compiler::compile(std::ostream &out, compiler_mode mode, endianness format) {
		if (mode == compiler_mode::PRINT_AST) {
			print_ast(out);
			return;
		}

		build();
		resolve();

		if (mode == compiler_mode::SYNTAX_ONLY || logger.has_errors()) {
			return;
		} else if (mode == compiler_mode::PRINT_SYMBOLS) {
			table.dump(logger, out);
		}

		if (mode == compiler_mode::NORMAL) {
			vector<uint16_t> output;
			auto stmt_compiler = statement_compiler(logger, output);
			for (auto& stmt : statements) {
				apply_visitor(stmt_compiler, stmt);
			}

			if (!logger.has_errors()) {
				write(output, out, format);
			}
		}
	}

	void compiler::print_ast(std::ostream &out) {
		for (auto &stmt : statements) {
			out << stmt << endl;
		}
	}

	void compiler::build() {
		uint32_t pc = 0;
		for (auto &stmt : statements) {
			apply_visitor(build_symbol_table(table, logger, pc), stmt);

			pc += output_size(stmt);
		}
	}

	void compiler::resolve() {
		uint32_t pc = 0;
		for (auto &stmt : statements) {
			apply_visitor(resolve_symbols(table, logger, pc), stmt);
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
				updated |= apply_visitor(compress_expressions(table, logger, pc), stmt);
				pc += output_size(stmt, pc);
			}
		}

		if (updated) {
			// if updated is still true then we must have hit max iterations
			throw runtime_error("internal compiler error: failed to convert label references to short literal form "
					"after 1000 iterations");
		}

		if (pc > numeric_limits<uint16_t>::max()) {
			logger.error(get_location(statements.back()), "output exceeds 65,535 words");
		}
	}

	void compiler::write(vector<uint16_t> &output, ostream &out, endianness format) {
		for (auto word : output) {
	        uint8_t b1 = word & 0xff;
	        uint8_t b2 = (word >> 8) & 0xff;

	        if (format == endianness::BIG) {
	            out.put(b1);
	            out.put(b2);
	        } else {
	            out.put(b2);
	            out.put(b1);
	        }

	        if (out.fail()) {
				throw runtime_error(str(boost::format("Error writing compiled output: %s" )
						% strerror(errno)));
			}
	    }
	}

	compile_result compile(log &logger, const argument &arg) {
		return apply_visitor(argument_compiler(logger), arg);
	}

	void compile(log &logger, vector<uint16_t> &output, const statement &stmt) {
		return apply_visitor(statement_compiler(logger, output), stmt);
	}
}}
