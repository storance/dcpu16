#pragma once

#include "argument.hpp"
#include <memory>
#include <boost/optional.hpp>

namespace dcpu { namespace emulator {
	class opcode {
	protected:
		dcpu &cpu;
		std::unique_ptr<argument> a, b;
	public:
		static std::unique_ptr<opcode>&& parse(dcpu &cpu, uint16_t instruction);

		opcode(dcpu &cpu, std::unique_ptr<argument> &&a, std::unique_ptr<argument> &&b);
		opcode(dcpu &cpu, std::unique_ptr<argument> &&a);
		virtual ~opcode();
		virtual uint16_t execute()=0;
	private:
		static std::unique_ptr<opcode>&& parse_basic(dcpu &cpu, uint16_t instruction);
		static std::unique_ptr<opcode>&& parse_special(dcpu &cpu, uint16_t instruction);
	};

	class set_opcode: public opcode {
	public:
		enum { OPCODE = 0x1 };

		virtual uint16_t execute();
	};

	class add_opcode: public opcode {
	public:
		enum { OPCODE = 0x2 };

		virtual uint16_t execute();
	};
}}