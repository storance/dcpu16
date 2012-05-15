#pragma once

#include <vector>
#include <cstdint>

#include "symbol_table.hpp"

namespace dcpu { namespace compiler {
	/*************************************************************************
	 *
	 * endianness
	 *
	 *************************************************************************/
	enum class endianness {
		LITTLE,
		BIG
	};

	/*************************************************************************
	 *
	 * compiler_mode
	 *
	 *************************************************************************/
	enum class compiler_mode {
		NORMAL,
		SYNTAX_ONLY,
		PRINT_AST,
		PRINT_SYMBOLS
	};

	/*************************************************************************
	 *
	 * compile_result
	 *
	 *************************************************************************/
	struct compile_result {
		std::uint8_t value;
		boost::optional<std::uint16_t> next_word;

		compile_result();
		compile_result(std::uint8_t value);
		compile_result(std::uint8_t value, std::uint16_t next_word);

		bool operator==(const compile_result &result) const;
	};

	std::ostream &operator<<(std::ostream &stream, const compile_result &result);

	/*************************************************************************
	 *
	 * expression_compiler
	 *
	 *************************************************************************/
	class expression_compiler : public boost::static_visitor<compile_result> {
		logging::log &logger;
		bool position_a;
		bool inside_indirect;
		bool force_next_word;

		compile_result compile_register(boost::optional<std::int32_t>, std::uint8_t, std::uint8_t, std::uint8_t) const;
	public:
		expression_compiler(logging::log &logger, const ast::expression_argument &arg);

		compile_result operator()(const ast::evaluated_expression& expr) const;
		template <typename T> compile_result operator()(const T& expr) const;
	};

	/*************************************************************************
	 *
	 * argument_compiler
	 *
	 *************************************************************************/
	class argument_compiler : public boost::static_visitor<compile_result> {
		logging::log &logger;
	public:
		argument_compiler(logging::log &logger);

		compile_result operator()(const ast::expression_argument &arg) const;
		compile_result operator()(const ast::stack_argument &arg) const;
	};

	/*************************************************************************
	 *
	 * base_symbol_visitor
	 *
	 *************************************************************************/
	class statement_compiler : public boost::static_visitor<> {
		logging::log &logger;
		std::vector<uint16_t> &output;
	public:
		statement_compiler(logging::log &logger, std::vector<uint16_t> &output);

		void operator()(const ast::instruction &instruction) const;
		void operator()(const ast::data_directive &data) const;
		void operator()(const ast::fill_directive &fill) const;
		void operator()(const ast::align_directive &align) const;
		template <typename T> void operator()(const T&) const;
	};

	/*************************************************************************
	 *
	 * base_symbol_visitor
	 *
	 *************************************************************************/
	class base_symbol_visitor {
	protected:
		symbol_table& table;
		uint32_t pc;
	public:
		base_symbol_visitor(symbol_table& table, uint32_t pc=0);
	};

	/*************************************************************************
	 *
	 * build_symbol_table
	 *
	 *************************************************************************/
	class build_symbol_table : public boost::static_visitor<>, public base_symbol_visitor {
		logging::log& logger;
	public:
		build_symbol_table(symbol_table& table, logging::log &logger, uint32_t pc);

		void operator()(const ast::label &label) const;
		void operator()(const ast::binary_operation &expr) const;
		void operator()(const ast::unary_operation &expr) const;
		void operator()(const ast::current_position_operand &expr) const;
		void operator()(const ast::expression_argument &arg) const;
		void operator()(const ast::instruction &instruction) const;
		void operator()(ast::equ_directive &equ) const;
		template <typename T> void operator()(const T &) const {}
	};

	/*************************************************************************
	 *
	 * resolve_symbols
	 *
	 *************************************************************************/
	class resolve_symbols : public boost::static_visitor<>, public base_symbol_visitor {
		logging::log& logger;
		bool allow_forward_refs;
	public:
		resolve_symbols(symbol_table& table, logging::log &logger, uint32_t pc=0,
				bool allow_forward_refs=true);

		void operator()(ast::symbol_operand &expr) const;
		void operator()(ast::binary_operation &expr) const;
		void operator()(ast::unary_operation &expr) const;
		void operator()(ast::current_position_operand &expr) const;
		void operator()(ast::expression_argument &arg) const;
		void operator()(ast::instruction &instruction) const;
		void operator()(ast::equ_directive &equ) const;
		void operator()(ast::fill_directive &fill) const;
		template <typename T> void operator()( T &) const {}
	};

	/*************************************************************************
	 *
	 * compress_expressions
	 *
	 *************************************************************************/
	class compress_expressions : public boost::static_visitor<bool>, public base_symbol_visitor  {
		logging::log &logger;
	public:
		compress_expressions(symbol_table& symbol_table, logging::log &logger, uint32_t pc=0);

		bool operator()(ast::expression_argument &arg) const;
		bool operator()(ast::instruction &instruction) const;
		bool operator()(ast::fill_directive &fill) const;
		bool operator()(ast::align_directive &align) const;
		template <typename T>bool operator()( T &) const { return false; }
	};

	/*************************************************************************
	 *
	 * compiler
	 *
	 *************************************************************************/
	class compiler {
	private:
		logging::log &logger;
		symbol_table& table;
		ast::statement_list &statements;


		void build();
		void resolve();
		void write(std::vector<uint16_t> &output, std::ostream &out, endianness format=endianness::BIG);
		void print_ast(std::ostream &out);
	public:
		compiler(logging::log &logger, symbol_table& table, ast::statement_list &statement);

		void compile(std::ostream &out, compiler_mode mode=compiler_mode::NORMAL, endianness format=endianness::BIG);
	};

	compile_result compile(logging::log &logger, const ast::argument &arg);
	void compile(logging::log &logger, std::vector<uint16_t> &output, const ast::statement &stmt);
}}
