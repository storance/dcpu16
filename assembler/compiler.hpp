#pragma once

#include <list>

#include "ast.hpp"

class Compiler {
private:
	uint32_t pos;
	std::vector<word_t> assembledWords;

	//void buildSymbolTable(const std::list<ast::statement> &);
	//void resolveLabels();
public:
	void compile(const std::list<ast::statement>&);
};
