#pragma once

#include "dcpu.hpp"
#include "argument.hpp"
#include <memory>
#include <boost/optional.hpp>

#define BASIC_OPCODE_CASE(o, cpu, a, b) case o ## _opcode ::OPCODE: \
	return unique_ptr<opcode>(new o ## _opcode (cpu, a, b));

#define SPECIAL_OPCODE_CASE(o, cpu, a) case o::OPCODE: \
	return unique_ptr<opcode>(new o(cpu, a));

#define DECLARE_BASIC_OPCODE(name, value) class name ## _opcode : public opcode { \
public: \
	enum { OPCODE = value }; \
	name ## _opcode(dcpu &cpu, std::unique_ptr<argument> &a, std::unique_ptr<argument> &b) : opcode(cpu, a, b) {} \
	virtual uint16_t execute(); \
};

#define DECLARE_SPECIAL_OPCODE(name, value) class name ## _opcode : public opcode { \
public: \
	enum { OPCODE = value }; \
	name ## _opcode(dcpu &cpu, std::unique_ptr<argument> &a) : opcode(cpu, a) {} \
	virtual uint16_t execute(); \
};

namespace dcpu { namespace emulator {
	class opcode {
	protected:
		dcpu &cpu;
		std::unique_ptr<argument> a, b;
	public:
		static std::unique_ptr<opcode> parse(dcpu &cpu, uint16_t instruction);

		opcode(dcpu &cpu, std::unique_ptr<argument> &a, std::unique_ptr<argument> &b);
		opcode(dcpu &cpu, std::unique_ptr<argument> &a);
		virtual ~opcode();
		virtual uint16_t execute()=0;
	private:
		static std::unique_ptr<opcode> parse_basic(dcpu &cpu, uint16_t instruction);
		static std::unique_ptr<opcode> parse_special(dcpu &cpu, uint16_t instruction);
	};

	DECLARE_BASIC_OPCODE(set, 0x1)
	DECLARE_BASIC_OPCODE(add, 0x2)
	DECLARE_BASIC_OPCODE(sub, 0x3)
	DECLARE_BASIC_OPCODE(mul, 0x4)
	DECLARE_BASIC_OPCODE(mli, 0x5)
}}
