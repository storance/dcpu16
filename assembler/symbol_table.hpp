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

	enum class symbol_type {
		GLOBAL_LABEL,
		LOCAL_LABEL,
		CURRENT_LOCATION,
		EQU
	};

	typedef boost::variant<std::uint16_t, ast::expression*> symbol_data;

	struct symbol : public ast::locatable {
		symbol_type type;
		std::string name;
		symbol_data data;

		symbol(const lexer::location_ptr &location, symbol_type type, const std::string name, std::uint16_t offset);

		void make_equ(ast::expression &expr);

		bool has_offset();
		std::uint16_t& offset();
		bool is_evaluatable();
		ast::evaluated_expression evaluate(const lexer::location_ptr &location);
	};

	class symbol_table {
		std::list<symbol> symbols;
		std::map<std::string, symbol&> lookup_table;

		symbol *last_global_before(std::uint16_t offset);

		std::string resolve_full_name(const std::string &name, std::uint16_t offset);
		std::string name_for_location(const lexer::location_ptr &location);
		void add_symbol(const symbol &_symbol);
	public:
		void add_label(const ast::label &label, std::uint16_t offset);
		void add_location(const lexer::location_ptr &location, std::uint16_t offset);
		void equ(ast::expression &expr);
		symbol *lookup(const std::string &name, std::uint16_t offset);
		symbol *lookup(const lexer::location_ptr &location, std::uint16_t offset);
		void compress_after(std::uint16_t offset);

		void build(const ast::statement_list &statements, error_handler_ptr &error_handler);
		void resolve(ast::statement_list &statements, error_handler_ptr &error_handler);
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
		void operator()(const ast::binary_operation &expr);
		void operator()(const ast::unary_operation &expr);
		void operator()(const ast::current_position_operand &expr);
		void operator()(const ast::expression_argument &arg);
		void operator()(const ast::instruction &instruction);
		template <typename T> void operator()(const T &);
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
		template <typename T> void operator()( T &);
	};

	class compress_expressions : public boost::static_visitor<bool>, public base_symbol_visitor  {
	public:
		compress_expressions(symbol_table* symbol_table);
		compress_expressions(std::uint16_t pc, symbol_table* symbol_table);

		bool operator()(ast::expression_argument &arg);
		bool operator()(ast::instruction &instruction);
		template <typename T>bool operator()( T &);
	};
}
