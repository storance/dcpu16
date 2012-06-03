#pragma once

#include <string>
#include <list>
#include <map>
#include <memory>
#include <cstdint>

#include "log.hpp"
#include "statement.hpp"

namespace dcpu { namespace assembler {
	/*************************************************************************
	 *
	 * duplicate_symbol_error
	 *
	 *************************************************************************/
	class duplicate_symbol_error : public std::exception {
		std::string message;
	public:
		duplicate_symbol_error(const std::string &name, location_ptr &first_location);
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



	/*************************************************************************
	 *
	 * symbol
	 *
	 *************************************************************************/
	struct symbol : public locatable {
		enum class symbol_type {
			GLOBAL_LABEL,
			LOCAL_LABEL,
			CURRENT_LOCATION,
			EQU
		};

		symbol_type type;
		std::string name;
		uint16_t offset;
		expression *equ_expr;

		symbol(const location_ptr &location, symbol_type type, const std::string name, uint16_t offset);
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
		std::string name_for_location(const location_ptr &location);
		void add_symbol(const symbol &_symbol);
	public:
		void add_label(const label &label, uint16_t offset);
		void add_location(const location_ptr &location, uint16_t offset);
		void equ(expression &expr);

		symbol *lookup(const std::string &name, uint16_t offset);
		symbol *lookup(const location_ptr &location, uint16_t offset);

		void update_after(uint16_t offset, int amount);
		void dump(log &logger, std::ostream &out);
	};
}}
