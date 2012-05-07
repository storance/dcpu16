#pragma once

#include <string>
#include <list>
#include <map>
#include <memory>
#include <cstdint>

#include "error_handler.hpp"
#include "statement.hpp"

namespace dcpu {
	class duplicate_symbol_error : public std::exception {
		std::string message;
	public:
		duplicate_symbol_error(const std::string &name, lexer::location_ptr &first_location);
		virtual ~duplicate_symbol_error() throw();

		virtual const char *what() const throw();
	};

	class no_global_label_error : public std::exception {
		std::string message;
	public:
		no_global_label_error(const std::string &name);
		virtual ~no_global_label_error() throw();

		virtual const char *what() const throw();
	};

	class undefined_symbol_error : public std::exception {
		std::string message;
	public:
		undefined_symbol_error(const std::string &name);
		virtual ~undefined_symbol_error() throw();

		virtual const char *what() const throw();
	};

	struct symbol : public ast::locatable {
		std::string name;
		std::uint16_t offset;
		bool global;

		symbol(const lexer::location_ptr &location, const std::string name, bool global, std::uint16_t offset);
	};

	class symbol_table {
		std::list<symbol> symbols;
		std::map<std::string, symbol&> lookup_table;

		symbol *last_global();
		symbol *last_global_before(std::uint16_t offset);

		std::string resolve_full_name(const std::string &name, std::uint16_t offset);
	public:
		void add(const ast::label &label, std::uint16_t offset);
		std::uint16_t *lookup(const std::string &name, std::uint16_t offset);
		void compress_after(std::uint16_t offset);

		void build(const ast::statement_list &statements, error_handler_ptr &error_handler);
		void resolve(const ast::statement_list &statements, error_handler_ptr &error_handler);
		void dump();
	};

	class base_symbol_visitor {
	public:
		std::uint16_t pc;
		symbol_table* table;

		base_symbol_visitor(symbol_table *table);
		base_symbol_visitor(std::uint16_t pc, symbol_table *table);
	};

	class build_symbol_table : public boost::static_visitor<>, public base_symbol_visitor {
		error_handler_ptr error_handler;
	public:
		build_symbol_table(error_handler_ptr &error_handler, symbol_table *table);

		void operator()(const ast::label &label);
		template <typename T> void operator()( const T &);
	};

	class resolve_symbols : public boost::static_visitor<>, public base_symbol_visitor {
		error_handler_ptr error_handler;
	public:
		resolve_symbols(error_handler_ptr &error_handler, symbol_table *table);
		resolve_symbols(std::uint16_t pc, error_handler_ptr &error_handler, symbol_table *table);

		void operator()(ast::symbol_operand &expr);
		void operator()(ast::binary_operation &expr);
		void operator()(ast::unary_operation &expr);
		void operator()(ast::current_position_operand &expr);
		void operator()(ast::expression_argument &arg);
		void operator()(ast::instruction &instruction);
		template <typename T> void operator()( const T &);
	};

	class update_current_position : public boost::static_visitor<> {
		std::uint16_t pc;
	public:
		update_current_position(std::uint16_t pc);

		void operator()(ast::current_position_operand &expr) const;
		void operator()(ast::binary_operation &expr) const;
		void operator()(ast::unary_operation &expr) const;
		template <typename T> void operator()( const T &) const;
	};

	class compress_expressions : public boost::static_visitor<bool>, public base_symbol_visitor  {
	public:
		compress_expressions(symbol_table* symbol_table);
		compress_expressions(std::uint16_t pc, symbol_table* symbol_table);

		bool operator()(ast::expression_argument &arg);
		bool operator()(ast::instruction &instruction);
		template <typename T>bool operator()( const T &);
	};
}
