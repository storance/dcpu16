#pragma once

#include <vector>
#include <cstdint>

#include "symbol_table.hpp"

namespace dcpu { namespace assembler {
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
		log &logger;
		bool position_a;
		bool inside_indirect;
		bool force_next_word;

		compile_result compile_register(boost::optional<std::int32_t>, std::uint8_t, std::uint8_t, std::uint8_t) const;
	public:
		expression_compiler(log &logger, const expression_argument &arg);

		compile_result operator()(const evaluated_expression& expr) const;
		template <typename T> compile_result operator()(const T& expr) const;
	};

	/*************************************************************************
	 *
	 * argument_compiler
	 *
	 *************************************************************************/
	class argument_compiler : public boost::static_visitor<compile_result> {
		log &logger;
	public:
		argument_compiler(log &logger);

		compile_result operator()(const expression_argument &arg) const;
		compile_result operator()(const stack_argument &arg) const;
	};

	/*************************************************************************
	 *
	 * base_symbol_visitor
	 *
	 *************************************************************************/
	class statement_compiler : public boost::static_visitor<> {
		log &logger;
		std::vector<uint16_t> &output;
	public:
		statement_compiler(log &logger, std::vector<uint16_t> &output);

		void operator()(const instruction &instruction) const;
		void operator()(const data_directive &data) const;
		void operator()(const fill_directive &fill) const;
		void operator()(const align_directive &align) const;
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
		log& logger;
	public:
		build_symbol_table(symbol_table& table, log &logger, uint32_t pc);

		void operator()(const label &label) const;
		void operator()(const binary_operation &expr) const;
		void operator()(const unary_operation &expr) const;
		void operator()(const current_position_operand &expr) const;
		void operator()(const expression_argument &arg) const;
		void operator()(const instruction &instruction) const;
		void operator()(equ_directive &equ) const;
		template <typename T> void operator()(const T &) const {}
	};

	/*************************************************************************
	 *
	 * resolve_symbols
	 *
	 *************************************************************************/
	class resolve_symbols : public boost::static_visitor<>, public base_symbol_visitor {
		log& logger;
		bool allow_forward_refs;
	public:
		resolve_symbols(symbol_table& table, log &logger, uint32_t pc=0,
				bool allow_forward_refs=true);

		void operator()(symbol_operand &expr) const;
		void operator()(binary_operation &expr) const;
		void operator()(unary_operation &expr) const;
		void operator()(current_position_operand &expr) const;
		void operator()(expression_argument &arg) const;
		void operator()(instruction &instruction) const;
		void operator()(equ_directive &equ) const;
		void operator()(fill_directive &fill) const;
		template <typename T> void operator()( T &) const {}
	};

	/*************************************************************************
	 *
	 * compress_expressions
	 *
	 *************************************************************************/
	class compress_expressions : public boost::static_visitor<bool>, public base_symbol_visitor  {
		log &logger;
	public:
		compress_expressions(symbol_table& symbol_table, log &logger, uint32_t pc=0);

		bool operator()(expression_argument &arg) const;
		bool operator()(instruction &instruction) const;
		bool operator()(fill_directive &fill) const;
		bool operator()(align_directive &align) const;
		template <typename T>bool operator()( T &) const { return false; }
	};

	/*************************************************************************
	 *
	 * compiler
	 *
	 *************************************************************************/
	class compiler {
	private:
		log &logger;
		symbol_table& table;
		statement_list &statements;


		void build();
		void resolve();
		void write(std::vector<uint16_t> &output, std::ostream &out, endianness format=endianness::BIG);
		void print_ast(std::ostream &out);
	public:
		compiler(log &logger, symbol_table& table, statement_list &statement);

		void compile(std::ostream &out, compiler_mode mode=compiler_mode::NORMAL, endianness format=endianness::BIG);
	};

	compile_result compile(log &logger, const argument &arg);
	void compile(log &logger, std::vector<uint16_t> &output, const statement &stmt);
}}
