#pragma once

#include <string>
#include <memory>

#include "dcpu.hpp"
#include "argument.hpp"

#define DECLARE_BASIC_OPCODE(name, value, cycles, conditional) class name ## Opcode : public Opcode { \
public: \
	enum { OPCODE = value }; \
	name ## Opcode(Dcpu &cpu, ArgumentPtr &a, ArgumentPtr &b) \
		: Opcode(cpu, a, b, cycles, conditional, #name) {} \
	virtual uint16_t execute(); \
};

#define DECLARE_SPECIAL_OPCODE(name, value, cycles) class name ## Opcode : public Opcode { \
public: \
	enum { OPCODE = value }; \
	name ## Opcode(Dcpu &cpu, ArgumentPtr &a) : Opcode(cpu, a, cycles, false, #name) {} \
	virtual uint16_t execute(); \
};

namespace dcpu { namespace emulator {
	class Opcode;
	typedef std::unique_ptr<Opcode> OpcodePtr;

	class Opcode {
		static OpcodePtr parseBasic(Dcpu &cpu, uint16_t instruction);
		static OpcodePtr parseSpecial(Dcpu &cpu, uint16_t instruction);
	protected:
		Dcpu &cpu;
		ArgumentPtr a, b;
		uint16_t baseCycles;
		bool conditional;
		std::string name;

		Opcode(Dcpu &cpu, ArgumentPtr &a, ArgumentPtr &b, uint16_t cycles, bool conditional,
				const std::string &name);
		Opcode(Dcpu &cpu, ArgumentPtr &a, uint16_t cycles, bool conditional, const std::string &name);

		uint16_t calculateCycles() const;
	public:
		static OpcodePtr parse(Dcpu &cpu, uint16_t instruction);

		virtual ~Opcode();

		std::string getName() const;
		bool isConditional() const;
		std::string str() const;

		virtual uint16_t execute()=0;
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
