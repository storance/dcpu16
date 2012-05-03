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
	};
}
