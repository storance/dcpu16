#pragma once

#include <string>
#include <vector>
#include <boost/spirit/include/qi.hpp>
#include "ast.hpp"

#include "../common.hpp"

struct SymbolEntry {
	std::string label;
	word_t offset;
};

class SymbolTable {
	std::vector<SymbolEntry> table;

public:
	void add(const std::string &label, word_t offset);
	word_t *lookup(const std::string &label);
};

typedef struct {
	std::string file;
	uint32_t line;
} location_t;