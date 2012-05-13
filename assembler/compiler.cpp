#include <boost/variant.hpp>

#include "compiler.hpp"

#include <boost/format.hpp>

using namespace std;
using namespace dcpu::ast;
using namespace boost;

namespace dcpu { namespace compiler {
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

	expression_compiler::expression_compiler(const expression_argument &arg) : arg(arg) {}

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

		if (arg.indirect) {
			return compile_result(0x1e, (uint16_t)*expr.value);
		} else {
			if (arg.position == argument_position::B || arg.force_next_word
					|| *expr.value < -1 || *expr.value > 30) {
				return compile_result(0x1f, (uint16_t)*expr.value);
			} else {
				return compile_result(0x21 + *expr.value);
			}
		}
	}

	compile_result expression_compiler::compile_register(boost::optional<int32_t> offset, uint8_t no_indirect,
			uint8_t indirect, uint8_t indirect_offset) const {
		if (!arg.indirect) {
			return compile_result(no_indirect);
		}

		if (offset && (arg.force_next_word || *offset != 0)) {
			return compile_result(indirect_offset, (uint16_t)*offset);
		} else {
			return compile_result(indirect);
		}
	}

	template <typename T>
	compile_result expression_compiler::operator()(const T& expr) const {
		expression eval_expr = evaluate(expression(expr));
		return apply_visitor(*this, eval_expr);
	}

	/*************************************************************************
	 *
	 * argument_compiler
	 *
	 *************************************************************************/

	compile_result argument_compiler::operator()(const expression_argument &arg) const {
		return apply_visitor(expression_compiler(arg), arg.expr);
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

	statement_compiler::statement_compiler(vector<uint16_t> &output) : output(output) {}

	template <typename T> void statement_compiler::operator()(const T&) const {

	}

	void statement_compiler::operator()(const instruction &instruction) const {
		if (instruction.opcode == opcodes::JMP) {
			expression_argument b(instruction.location, argument_position::B,
					register_operand(instruction.location, registers::PC), false, false);
			statement stmt(ast::instruction(instruction.location, opcodes::SET, instruction.a, argument(b)));
			apply_visitor(*this, stmt);
			return;
		}

		compile_result a_result, b_result;

		a_result = compile(instruction.a);
		if (instruction.b) {
			b_result = compile(*instruction.b);
		}

		uint16_t encoded_value = static_cast<uint16_t>(instruction.opcode)
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

	void statement_compiler::operator()(const ast::fill_directive &fill) const {
		auto evaled_count = evaluate(fill.count);
		auto evaled_value = evaluate(fill.value);

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

	/*************************************************************************
	 *
	 * compiler
	 *
	 *************************************************************************/

	compiler::compiler(error_handler_ptr &error_handler, symbol_table &table)
			: error_handler(error_handler), table(table) {}

	void compiler::compile(statement_list &statements) {
		auto stmt_compiler = statement_compiler(output);
		for (auto& stmt : statements) {
			apply_visitor(stmt_compiler, stmt);
		}
	}

	void compiler::write(ostream &out, Endianness format) {
		for (auto word : output) {
	        uint8_t b1 = word & 0xff;
	        uint8_t b2 = (word >> 8) & 0xff;

	        if (format == Endianness::BIG) {
	            out.put(b1);
	            out.put(b2);
	        } else {
	            out.put(b2);
	            out.put(b1);
	        }
	    }
	}

	compile_result compile(const argument &arg) {
		return apply_visitor(argument_compiler(), arg);
	}

	void compile(vector<uint16_t> &output, const statement &stmt) {
		return apply_visitor(statement_compiler(output), stmt);
	}
}}
