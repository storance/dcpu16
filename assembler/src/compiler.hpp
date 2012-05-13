#pragma once

#include <vector>
#include <cstdint>

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

		void operator()(const ast::instruction &instruction) const;
		void operator()(const ast::data_directive &data) const;
		void operator()(const ast::fill_directive &fill) const;
		template <typename T> void operator()(const T&) const;
	};


	class compiler {
	private:
		logging::log &logger;
		symbol_table &table;
		std::vector<std::uint16_t> output;
	public:
		compiler(logging::log &logger, symbol_table &table);

		void compile(ast::statement_list &statement);
		void write(std::ostream &out, Endianness format=Endianness::BIG);
	};

	compile_result compile(const ast::argument &arg);
	void compile(std::vector<uint16_t> &output, const ast::statement &stmt);
}}