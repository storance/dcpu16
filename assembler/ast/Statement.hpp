#pragma once

#include <string>
#include <list>
#include <cstdint>

#include "OpcodeDefinition.hpp"
#include "Argument.hpp"


namespace dcpu { 
	class SymbolTable;

	namespace ast {
		enum class LabelType {
			Global,
			Local,
			GlobalNoAttach
		};

		class Statement {
		public:
			lexer::Location _location;

			//virtual void buildSymbolTable()=0;
			//virtual void compile()=0;

			Statement(const lexer::Location&);
			virtual ~Statement();

			virtual std::string str() const=0;
			virtual void buildSymbolTable(SymbolTable& table, std::uint16_t &position) const=0;
		};

		typedef std::unique_ptr<ast::Statement> StatementPtr;
		typedef std::list<StatementPtr> StatementList;

		class Instruction : public Statement {
		public:
			Opcode _opcode;
			ArgumentPtr _a;
			ArgumentPtr _b;

			Instruction(const lexer::Location&, Opcode, ArgumentPtr &a, ArgumentPtr &b);
			Instruction(const lexer::Location&, Opcode, ArgumentPtr &&a, ArgumentPtr &&b);

			virtual std::string str() const;
			virtual void buildSymbolTable(SymbolTable& table, std::uint16_t &position) const;
		};

		class Label : public Statement {
		public:
			LabelType _type;
			std::string _name;

			Label(const lexer::Location&, const std::string&);

			virtual std::string str() const;
			virtual void buildSymbolTable(SymbolTable& table, std::uint16_t &position) const;
		};

		std::string str(const StatementPtr&);
	}
}