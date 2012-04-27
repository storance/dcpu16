#pragma once

#include <string>
#include <list>
#include <map>
#include <cstdint>

#include "ast/Statement.hpp"

namespace dcpu {
	class DuplicateLabelError : public std::exception {
		std::string message;
	public:
		DuplicateLabelError(const dcpu::ast::Label &duplicateLabel, const dcpu::ast::Label &existingLabel);
		virtual ~DuplicateLabelError() throw();

		virtual const char *what() const throw();
	};

	class NoGlobalLabelError : public std::exception {
		std::string message;
	public:
		NoGlobalLabelError(const std::string &labelName);
		virtual ~NoGlobalLabelError() throw();

		virtual const char *what() const throw();
	};

	struct SymbolEntry {
		ast::Label label;
		std::uint16_t position;

		SymbolEntry(const ast::Label &label, std::uint16_t position);
	};

	class SymbolTable {
		std::list<SymbolEntry> _entries;
		std::map<std::string, SymbolEntry&> _lookupTable;

		SymbolEntry &getLastGlobalLabel(const std::string &labelName);
	public:
		std::string getFullLabelName(const std::string &label);
		void add(ast::Label label, std::uint16_t position);
		std::uint16_t *lookup(const std::string &labelName);
	};
}