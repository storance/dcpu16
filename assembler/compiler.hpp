#pragma once

#include <vector>
#include <cstdint>

#include "statement.hpp"
#include "error_handler.hpp"
#include "symbol_table.hpp"

namespace dcpu { namespace compiler {
	enum class Endianness {
		LITTLE,
		BIG
	};

	struct compile_result {
		std::uint8_t value;
		boost::optional<std::uint16_t> next_word;

		compile_result();
		compile_result(std::uint8_t value);
		compile_result(std::uint8_t value, std::uint16_t next_word);

		bool operator==(const compile_result &result) const;
	};

	std::ostream &operator<<(std::ostream &stream, const compile_result &result);

	class expression_compiler : public boost::static_visitor<compile_result> {
		const ast::expression_argument &arg;

		compile_result compile_register(boost::optional<std::int32_t>, std::uint8_t, std::uint8_t, std::uint8_t) const;
	public:
		expression_compiler(const ast::expression_argument &arg);

		compile_result operator()(const ast::evaluated_expression& expr) const;
		template <typename T> compile_result operator()(const T& expr) const;
	};

	class argument_compiler : public boost::static_visitor<compile_result> {
	public:
		compile_result operator()(const ast::expression_argument &arg) const;
		compile_result operator()(const ast::stack_argument &arg) const;
	};

	class statement_compiler : public boost::static_visitor<> {
		std::vector<uint16_t> &output;
	public:
		statement_compiler(std::vector<uint16_t> &output);

		void operator()(const ast::label &label) const;
		void operator()(const ast::instruction &instruction) const;
	};


	class compiler {
	private:
		error_handler_ptr error_handler;
		symbol_table table;
		std::vector<std::uint16_t> output;
	public:
		compiler(error_handler_ptr &error_handler);

		void compile(ast::statement_list &statement);
		void write(std::ostream &out, Endianness format=Endianness::BIG);
	};

	compile_result compile(const ast::argument &arg);
	void compile(std::vector<uint16_t> &output, const ast::statement &stmt);
}}
