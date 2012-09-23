#pragma once

#include <string>
#include <memory>

#include "dcpu.hpp"
#include "argument.hpp"

#define DECLARE_BASIC_OPCODE(name, value, cycles, is_cond) class name ## _opcode : public opcode { \
public: \
	enum { OPCODE = value }; \
	name ## _opcode(dcpu &cpu, std::unique_ptr<argument> &a, std::unique_ptr<argument> &b) \
		: opcode(cpu, a, b, cycles, is_cond, #name) {} \
	virtual uint16_t execute(); \
};

#define DECLARE_SPECIAL_OPCODE(name, value, cycles) class name ## _opcode : public opcode { \
public: \
	enum { OPCODE = value }; \
	name ## _opcode(dcpu &cpu, std::unique_ptr<argument> &a) : opcode(cpu, a, cycles, is_cond, #name) {} \
	virtual uint16_t execute(); \
};

namespace dcpu { namespace emulator {
	class opcode {
		static std::unique_ptr<opcode> parse_basic(dcpu &cpu, uint16_t instruction);
		static std::unique_ptr<opcode> parse_special(dcpu &cpu, uint16_t instruction);
	protected:
		dcpu &cpu;
		std::unique_ptr<argument> a, b;
		uint16_t base_cycles;
		bool is_cond;
		std::string name;

		opcode(dcpu &cpu, std::unique_ptr<argument> &a, std::unique_ptr<argument> &b, uint16_t cycles, bool is_cond,
				const std::string &name);
		opcode(dcpu &cpu, std::unique_ptr<argument> &a, uint16_t cycles, bool is_cond, const std::string &name);

		uint16_t calculate_cycles() const;
	public:
		static std::unique_ptr<opcode> parse(dcpu &cpu, uint16_t instruction);

		virtual ~opcode();

		std::string get_name() const;
		bool is_conditional() const;

		virtual uint16_t execute()=0;

		std::string to_str() const;
	};

	DECLARE_BASIC_OPCODE(set, 0x01, 1, false)
	DECLARE_BASIC_OPCODE(add, 0x02, 2, false)
	DECLARE_BASIC_OPCODE(sub, 0x03, 2, false)
	DECLARE_BASIC_OPCODE(mul, 0x04, 2, false)
	DECLARE_BASIC_OPCODE(mli, 0x05, 2, false)
	DECLARE_BASIC_OPCODE(div, 0x06, 3, false)
	DECLARE_BASIC_OPCODE(dvi, 0x07, 3, false)
	DECLARE_BASIC_OPCODE(mod, 0x08, 3, false)
	DECLARE_BASIC_OPCODE(mdi, 0x09, 3, false)
	DECLARE_BASIC_OPCODE(and, 0x0a, 1, false)
	DECLARE_BASIC_OPCODE(bor, 0x0b, 1, false)
	DECLARE_BASIC_OPCODE(xor, 0x0c, 1, false)
	DECLARE_BASIC_OPCODE(shr, 0x0d, 1, false)
	DECLARE_BASIC_OPCODE(asr, 0x0e, 1, false)
	DECLARE_BASIC_OPCODE(shl, 0x0f, 1, false)
	DECLARE_BASIC_OPCODE(ifb, 0x10, 2, true)
	DECLARE_BASIC_OPCODE(ifc, 0x11, 2, true)
	DECLARE_BASIC_OPCODE(ife, 0x12, 2, true)
	DECLARE_BASIC_OPCODE(ifn, 0x13, 2, true)
	DECLARE_BASIC_OPCODE(ifg, 0x14, 2, true)
	DECLARE_BASIC_OPCODE(ifa, 0x15, 2, true)
	DECLARE_BASIC_OPCODE(ifl, 0x16, 2, true)
	DECLARE_BASIC_OPCODE(ifu, 0x17, 2, true)
	DECLARE_BASIC_OPCODE(adx, 0x1a, 3, true)
	DECLARE_BASIC_OPCODE(sbx, 0x1b, 3, true)
	DECLARE_BASIC_OPCODE(sti, 0x1e, 2, true)
	DECLARE_BASIC_OPCODE(std, 0x1f, 2, true)

	DECLARE_SPECIAL_OPCODE(jsr, 0x01, 3)
	DECLARE_SPECIAL_OPCODE(hcf, 0x07, 1)
	DECLARE_SPECIAL_OPCODE(int, 0x08, 4)
	DECLARE_SPECIAL_OPCODE(iag, 0x09, 1)
	DECLARE_SPECIAL_OPCODE(ias, 0x0a, 1)
	DECLARE_SPECIAL_OPCODE(rfi, 0x0b, 3)
	DECLARE_SPECIAL_OPCODE(iaq, 0x0c, 2)
	DECLARE_SPECIAL_OPCODE(hwn, 0x10, 2)
	DECLARE_SPECIAL_OPCODE(hwq, 0x11, 4)
	DECLARE_SPECIAL_OPCODE(hwi, 0x12, 4)
}}
