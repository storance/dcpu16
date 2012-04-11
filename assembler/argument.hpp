#pragma once

#include <string>

#include "assembler.hpp"

class Argument {
protected:
	location_t location;
	bool nextWordRequired;
	bool labelsResolved;

	Argument(location_t, bool);
public:
	bool isNextWordRequired();
	virtual word_t getNextWord();

	virtual void resolveLabels(SymbolTable &);
	virtual uint8_t encode()=0;
};

class RegisterArgument : public Argument {
protected:
	uint8_t registerType;

	RegisterArgument(location_t, bool, uint8_t);
public:
	RegisterArgument(location_t, uint8_t);

	virtual uint8_t encode();
};

class RegisterPtrArgument : public RegisterArgument {
public:
	RegisterPtrArgument(location_t, uint8_t);

	virtual uint8_t encode();
};

class RegisterOffsetPtrArgument : public RegisterArgument {
protected:
	word_t offset;
public:
	RegisterOffsetPtrArgument(location_t, uint8_t, word_t);

	virtual word_t getNextWord();
	virtual uint8_t encode();
};

class RegisterLabelPtrArgument : public RegisterOffsetPtrArgument{
protected:
	std::string label;
public:
	RegisterLabelPtrArgument(location_t, uint8_t, word_t, const std::string&);

	virtual void resolveLabels(SymbolTable &);
};

class PopStackArgument : public Argument {
public:
	PopStackArgument(location_t);

	virtual uint8_t encode();
};

class PushStackArgument : public Argument {
public:
	PushStackArgument(location_t);

	virtual uint8_t encode();
};

class LiteralArgument : public Argument {
protected:
	word_t literal;

	LiteralArgument(location_t, bool, word_t);
public:
	LiteralArgument(location_t, word_t);

	virtual word_t getNextWord();
	virtual uint8_t encode();
};

class LiteralPtrArgument : public LiteralArgument {
public:
	LiteralPtrArgument(location_t, word_t);

	virtual uint8_t encode();
};

class LabelArgument : public LiteralArgument {
protected:
	std::string label;
public:
	LabelArgument(location_t, const std::string&, word_t);

	virtual void resolveLabels(SymbolTable &);
};

class LabelPtrArgument : public LiteralPtrArgument {
protected:
	std::string label;
public:
	 LabelPtrArgument(location_t, const std::string&, word_t);

	 virtual void resolveLabels(SymbolTable &);
};
