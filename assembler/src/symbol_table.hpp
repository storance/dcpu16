#pragma once

#include <string>
#include <list>
#include <map>
#include <memory>
#include <cstdint>

#include "log.hpp"
#include "statement.hpp"

namespace dcpu {
	/*************************************************************************
	 *
	 * duplicate_symbol_error
	 *
	 *************************************************************************/
	class duplicate_symbol_error : public std::exception {
		std::string message;
	public:
		duplicate_symbol_error(const std::string &name, lexer::location_ptr &first_location);
		virtual ~duplicate_symbol_error() throw();

		virtual const char *what() const throw();
	};

	/*************************************************************************
	 *
	 * no_global_label_error
	 *
	 *************************************************************************/
	class no_global_label_error : public std::exception {
		std::string message;
	public:
		no_global_label_error(const std::string &name);
		virtual ~no_global_label_error() throw();

		virtual const char *what() const throw();
	};

	/*************************************************************************
	 *
	 * undefined_symbol_error
	 *
	 *************************************************************************/
	class undefined_symbol_error : public std::exception {
		std::string message;
	public:
		undefined_symbol_error(const std::string &name);
		virtual ~undefined_symbol_error() throw();

		virtual const char *what() const throw();
	};

	enum class symbol_type {
		GLOBAL_LABEL,
		LOCAL_LABEL,
		CURRENT_LOCATION,
		EQU
	};

	/*************************************************************************
	 *
	 * symbol
	 *
	 *************************************************************************/
	struct symbol : public ast::locatable {
		symbol_type type;
		std::string name;
		uint16_t offset;
		ast::expression *equ_expr;

		symbol(const lexer::location_ptr &location, symbol_type type, const std::string name, uint16_t offset);
	};

	/*************************************************************************
	 *
	 * symbol_table
	 *
	 *************************************************************************/
	class symbol_table {
		std::list<symbol> symbols;
		std::map<std::string, symbol&> lookup_table;

		symbol *last_global_before(uint16_t offset);

		std::string full_name(const std::string &name, uint16_t offset);
		std::string name_for_location(const lexer::location_ptr &location);
		void add_symbol(const symbol &_symbol);
	public:
		void add_label(const ast::label &label, uint16_t offset);
		void add_location(const lexer::location_ptr &location, uint16_t offset);
		void equ(ast::expression &expr);

		symbol *lookup(const std::string &name, uint16_t offset);
		symbol *lookup(const lexer::location_ptr &location, uint16_t offset);

		void build(ast::statement_list &statements, logging::log &logger);
		void resolve(ast::statement_list &statements, logging::log &logger);
		void update_after(uint16_t offset, int amount);
		void dump();
	};

	/*************************************************************************
	 *
	 * base_symbol_visitor
	 *
	 *************************************************************************/
	class base_symbol_visitor {
	protected:
		symbol_table* table;
		uint32_t pc;
	public:
		base_symbol_visitor(symbol_table *table, uint32_t pc=0);
	};

	/*************************************************************************
	 *
	 * build_symbol_table
	 *
	 *************************************************************************/
	class build_symbol_table : public boost::static_visitor<>, public base_symbol_visitor {
		logging::log& logger;
	public:
		build_symbol_table(symbol_table *table, logging::log &logger, uint32_t pc);

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
		resolve_symbols(symbol_table *table, logging::log &logger, uint32_t pc=0,
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
	public:
		compress_expressions(symbol_table* symbol_table, uint32_t pc=0);

		bool operator()(ast::expression_argument &arg) const;
		bool operator()(ast::instruction &instruction) const;
		bool operator()(ast::fill_directive &fill) const;
		template <typename T>bool operator()( T &) const { return false; }
	};
}
