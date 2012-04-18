#include "Parser.hpp"

#include <iostream>
#include <iterator>
#include <map>

#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace dcpu::lexer;
using namespace dcpu::ast;

namespace dcpu { namespace parser {

// TODO: switch to initializer list
static map<std::string, OpcodeDefinition> opcodes = {
	{"set", OpcodeDefinition(Opcode::SET, 2)},
	{"add", OpcodeDefinition(Opcode::ADD, 2)},
	{"sub", OpcodeDefinition(Opcode::SUB, 2)},
	{"mul", OpcodeDefinition(Opcode::MUL, 2)},
	{"div", OpcodeDefinition(Opcode::DIV, 2)},
	{"mod", OpcodeDefinition(Opcode::MOD, 2)},
	{"shr", OpcodeDefinition(Opcode::SHR, 2)},
	{"shl", OpcodeDefinition(Opcode::SHL, 2)},
	{"and", OpcodeDefinition(Opcode::AND, 2)},
	{"bor", OpcodeDefinition(Opcode::BOR, 2)},
	{"xor", OpcodeDefinition(Opcode::XOR, 2)},
	{"ife", OpcodeDefinition(Opcode::IFE, 2)},
	{"ifn", OpcodeDefinition(Opcode::IFN, 2)},
	{"ifg", OpcodeDefinition(Opcode::IFG, 2)},
	{"ifb", OpcodeDefinition(Opcode::IFB, 2)},
	{"jsr", OpcodeDefinition(Opcode::JSR, 1)},
	{"jmp", OpcodeDefinition(Opcode::JMP, 1)},
	{"push", OpcodeDefinition(Opcode::PUSH, 1)},
	{"pop", OpcodeDefinition(Opcode::POP, 1)}
};

static map<std::string, RegisterDefinition> registers = {
	{"a",  RegisterDefinition(Register::A, true, true)},
	{"b",  RegisterDefinition(Register::B, true, true)},
	{"c",  RegisterDefinition(Register::C, true, true)},
	{"x",  RegisterDefinition(Register::X, true, true)},
	{"y",  RegisterDefinition(Register::Y, true, true)},
	{"z",  RegisterDefinition(Register::Z, true, true)},
	{"i",  RegisterDefinition(Register::I, true, true)},
	{"j",  RegisterDefinition(Register::J, true, true)},
	{"sp", RegisterDefinition(Register::SP, true, false)},
	{"pc", RegisterDefinition(Register::PC, false, false)},
	{"o",  RegisterDefinition(Register::O, false, false)}
};


Parser::Parser(Iterator start, Iterator end, ErrorHandler &errorHandler)
	: current(start), end(end), errorHandler(errorHandler) {
}

boost::optional<OpcodeDefinition> Parser::lookupOpcode(std::string opcodeName) {
	boost::algorithm::to_lower(opcodeName);

	auto it = opcodes.find(opcodeName);
	if (it == opcodes.end()) {
		return boost::optional<OpcodeDefinition>();
	}

	return boost::optional<OpcodeDefinition>(it->second);
}

boost::optional<RegisterDefinition> Parser::lookupRegister(std::string registerName) {
	boost::algorithm::to_lower(registerName);

	auto it = registers.find(registerName);
	if (it == registers.end()) {
		return boost::optional<RegisterDefinition>();
	}

	return boost::optional<RegisterDefinition>(it->second);
}

template<typename Predicate> Parser::Iterator Parser::findToken(Predicate pred) {
	for (auto it = current; it != end; it++) {
		auto currentToken = *it;

		if (pred(*it)) {
			return it;
		}
	}

	return end;
}

string Parser::concatTokens(token_type initial, Iterator end, bool inclusive) {
	string content = initial->content;

	Iterator realEnd = inclusive ? ++end : end;
	for (; current != realEnd; current++) {
		if ((*current)->isComment()) {
			continue;
		}

		content += (*current)->content;
	}

	return content;
}

bool Parser::isEndOfStatement(Iterator it) {
	return it == end || (*it)->isNewline() || (*it)->isEOI();
}

boost::optional<Instruction> Parser::parseInstruction(token_type currentToken) {
	boost::optional<OpcodeDefinition> opcodeDef = lookupOpcode(currentToken->content);

	if (!opcodeDef) {
		return boost::optional<Instruction>();
	}

	Argument a, b;
	if (opcodeDef->numArgs > 0) {
		a = parseArgument(nextToken());	
	} 

	if (opcodeDef->numArgs > 1) {
		auto token = nextToken();

		if (!token->isCharacter(',')) {
			if (token->isNewline()) {
				errorHandler.error(token->location, 
					str(boost::format("Instruction %s requires 2 arguments.") % currentToken->content));

				b = Argument(nullptr);
			} else {
				errorHandler.error(token->location, 
					str(boost::format("Unexpected '%s', expected ','.") % token->content)); 

				b = parseArgument(token);
			}
		} else {
			b = parseArgument(nextToken());
		}
	}
	
	return Instruction(opcodeDef->opcode, a, b);
}

Argument Parser::parseArgument(token_type currentToken) {
	if (currentToken->isCharacter(',')) {
		errorHandler.error(currentToken->location, "Unexpected ',', expected an instruction argument.");
		return Argument(nullptr);
	}

	auto argumentEnd = findToken([](token_type token) {
		return token->isCharacter(',') || token->isNewline() || token->isEOI();
	});

	if (currentToken->isIdentifier()) {
		auto registerDef = lookupRegister(currentToken->content);
		if (registerDef) {
			return Argument(registerDef->registerType);
		}
	}

	errorHandler.error(currentToken->location, str(boost::format("Unrecognized instruction argument '%s'") 
		% concatTokens(currentToken, argumentEnd, false)));
	return Argument(nullptr);
}

boost::optional<Label> Parser::parseLabel(token_type currentToken) {
	if (currentToken->isCharacter(':')) {
		auto idToken = nextToken(false);
		if (idToken->isIdentifier()) {
			return Label(idToken->content);
		}

		auto nextIdentifier = findToken([](token_type token) {
			return token->isIdentifier() || token->isNewline();
		});
		if (isEndOfStatement(nextIdentifier)) {
			errorHandler.error(currentToken->location, "Unexpected ':', expected a label or instruction .");
			return boost::optional<Label>();
		}

		string labelName = concatTokens(idToken, nextIdentifier, true);
		errorHandler.error(currentToken->location, str(boost::format("Invalid label definition '%s'") % labelName));

		return boost::optional<Label>();
	} else if (currentToken->isIdentifier()) {
		auto colonToken = nextToken(false);
		if (colonToken->isCharacter(':')) {
			return Label(currentToken->content);
		}

		auto nextColon = findToken([](token_type token) {
			return token->isCharacter(':') || token->isNewline();
		});
		if (isEndOfStatement(nextColon)) {
			rewind();
			return boost::optional<Label>();
		}

		string labelName = concatTokens(colonToken, nextColon, false);
		errorHandler.error(currentToken->location, str(boost::format("Invalid label definition '%s'") % labelName));

		return boost::optional<Label>();
	}
}

boost::optional<Statement> Parser::parseStatement() {
	token_type currentToken = nextToken(true, true);

	if (currentToken->isEOI()) {
		return boost::optional<Statement>();
	}

	if (!currentToken->isCharacter(':') && !currentToken->isIdentifier()) {
		auto nextValidToken = findToken([](token_type token) {
			return token->isCharacter(':') || token->isIdentifier();
		});

		string invalidContent = concatTokens(currentToken, nextValidToken, false);
		errorHandler.error(currentToken->location,
			str(boost::format("Unexpected '%s', expected a label or instruction.") % invalidContent));

		return parseStatement();
	}

	boost::optional<Label> label = parseLabel(currentToken);
	if (label) {
		return boost::optional<Statement>(*label);
	}

	boost::optional<Instruction> instruction = parseInstruction(currentToken);
	if (instruction) {
		return boost::optional<Statement>(*instruction);
	}

	errorHandler.error(currentToken->location,
		str(boost::format("Unrecognized instruction '%s'.") % currentToken->content));

	advanceToEndOfLine();
	return parseStatement();
}

void Parser::advanceToEndOfLine() {
	while (current != end) {
		if ((*current)->isNewline() || (*current)->isEOI()) {
			break;
		}
	}
}

void Parser::rewind() {
	--current;
}

token_type Parser::nextToken(bool skipWsComments, bool skipNewline) {
	while(current != end) {
		token_type token = *current;
		++current;

		if (skipWsComments && (token->isWhitespace() || token->isComment())) {
			continue;
		}

		if (skipNewline && token->isNewline()) {
			continue;
		}

		return token;
	}

	// Return the end of input token
	auto eoi = end; --eoi;
	return *eoi;
}

void Parser::parse() {
	while (current != end) {
		auto statement = parseStatement();

		if (!statement) {
			break;
		}

		statements.push_back(*statement);
	}
}

} }
