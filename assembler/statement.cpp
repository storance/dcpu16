#include "statement.hpp"

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace dcpu::lexer;
using namespace boost;

namespace dcpu { namespace ast {
	/*************************************************************************
	 *
	 * base_argument
	 *
	 *************************************************************************/
	base_argument::base_argument(const location_ptr &location, argument_position position)
			: locatable(location), position(position) {}

	/*************************************************************************
	 *
	 * stack_argument
	 *
	 *************************************************************************/

	stack_argument::stack_argument(const location_ptr &location, argument_position position, stack_operation operation)
			: base_argument(location, position), operation(operation) {}

	bool stack_argument::operator==(const stack_argument& other) const {
		return operation == other.operation;
	}

	/*************************************************************************
	 *
	 * expression_argument
	 *
	 *************************************************************************/
	expression_argument::expression_argument(const location_ptr &location, argument_position position, const expression &expr,
			bool indirect, bool force_next_word) : base_argument(location, position), indirect(indirect),
			force_next_word(force_next_word), next_word_required(true), expr(expr) {}

	bool expression_argument::operator==(const expression_argument& other) const {
		return expr == other.expr && indirect == other.indirect && force_next_word == other.force_next_word;
	}

	/*************************************************************************
	 *
	 * instruction
	 *
	 *************************************************************************/
	instruction::instruction(const location_ptr& location, opcodes opcode, const argument &a, const optional_argument &b)
		: locatable(location), opcode(opcode), a(a), b(b) {}

	bool instruction::operator==(const instruction& other) const {
		return opcode == other.opcode && a == other.a && b == other.b;
	}

	/*************************************************************************
	 *
	 * label
	 *
	 *************************************************************************/
	label::label(const location_ptr &location, const string& name)
		: locatable(location), name(name) {
		if (starts_with(name, "..@")) {
			type = label_type::GlobalNoAttach;
		} else if (starts_with(name, ".")) {
			type = label_type::Local;
		} else {
			type = label_type::Global;
		}
	}
	
	label::label(const location_ptr &location, const string& name, label_type type)
		: locatable(location), type(type), name(name) {}

	bool label::operator==(const label &other) const {
		return name == other.name && type == other.type;
	}

	/*************************************************************************
	 *
	 * label
	 *
	 *************************************************************************/
	data::data(const location_ptr &location) : locatable(location) {}

	data::data(const location_ptr &location, const vector<uint16_t> &value)
		: locatable(location), value(value) {}

	bool data::operator==(const data& other) const {
		return value == other.value;
	}

	void data::append(uint16_t word) {
		value.push_back(word);
	}

	void data::append(const std::string literal) {
		copy (literal.begin(), literal.end(), back_inserter(value));
	}

	/*************************************************************************
	 *
	 * calculate_size_expression
	 *
	 *************************************************************************/

	calculate_size_expression::calculate_size_expression(const expression_argument &arg) : arg(arg) {}

	uint16_t calculate_size_expression::operator()(const evaluated_expression &expr) const {
		if (!arg.indirect) {
			if (expr._register || (arg.position == argument_position::A
				&& !arg.force_next_word && *expr.value >= -1 && *expr.value <= 30)) {
				return 0;
			} else {
				return 1;
			}
		} else {
			return expr._register && !expr.value ? 0 : 1;
		}
	}

	template <typename T>
	uint16_t calculate_size_expression::operator()( const T &expr) const {
		return 1;
	}

	/*************************************************************************
	 *
	 * calculate_size
	 *
	 *************************************************************************/

	uint16_t calculate_size::operator()(const stack_argument& arg) const {
		return 0;
	}

	uint16_t calculate_size::operator()(const expression_argument &arg) const {
		if (!evaluated(arg.expr)) {
			return arg.next_word_required ? 1 : 0;
		}

		return output_size(arg, arg.expr);
	}

	uint16_t calculate_size::operator()(const label &) const {
		return 0;
	}

	uint16_t calculate_size::operator()(const instruction &instruction) const {
		uint8_t size = 1;

		size += apply_visitor(*this, instruction.a);

		if (instruction.b) {
			size += apply_visitor(*this, *instruction.b);
		}

		return size;
	}

	uint16_t calculate_size::operator()(const data &data) const {
		return data.value.size();
	}

	/*************************************************************************
	 *
	 * output_size functions
	 *
	 *************************************************************************/

	uint16_t output_size(const statement &statement) {
		return apply_visitor(calculate_size(), statement);
	}

	uint16_t output_size(const argument &arg) {
		return apply_visitor(calculate_size(), arg);
	}

	uint16_t output_size(const expression_argument &arg, const expression &expr) {
		return apply_visitor(calculate_size_expression(arg), expr);
	}

	/*************************************************************************
	 *
	 * Stream operators
	 *
	 *************************************************************************/

	ostream& operator<< (ostream& stream, stack_operation operation) {
		switch (operation) {
		case stack_operation::PUSH:
			return stream << "PUSH";
		case stack_operation::POP:
			return stream << "POP";
		case stack_operation::PEEK:
			return stream << "PEEK";
		default:
			return stream << "<Unknown stack_operation " << static_cast<int>(operation) << ">";
		}
	}

	ostream& operator<< (ostream& stream, label_type labelType) {
		switch (labelType) {
		case label_type::Global:
			return stream << "Global";
		case label_type::Local:
			return stream << "Local";
		case label_type::GlobalNoAttach:
			return stream << "GlobalNoAttach";
		default:
			return stream << "<Unknown label_type " << static_cast<int>(labelType) << ">";
		}
	}

	ostream& operator<< (ostream& stream, const stack_argument &arg) {
		return stream << arg.operation;
	}

	ostream& operator<< (ostream& stream, const expression_argument &arg) {
		if (arg.indirect) {
			stream << "[";
		}

		stream << arg.expr;

		if (arg.indirect) {
			stream << "]";
		}

		return stream;
	}

	ostream& operator<< (ostream& stream, const label &label) {
		return stream << label.name << ":";
	}

	ostream& operator<< (ostream& stream, const instruction &instruction) {
		stream << instruction.opcode << " ";

		if (instruction.b) {
			stream << *instruction.b << ", ";
		}

		return stream << instruction.a;
	}

	ostream& operator<< (ostream& stream, const data &data) {
		stream << "dat ";

		bool first = true;
		for (uint16_t word : data.value) {
			if (!first) stream << ",";
			stream << boost::format("%#04x") % word;

			first = false;
		}

		return stream;
	}
}}
