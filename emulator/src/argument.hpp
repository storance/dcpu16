#pragma once

#include <memory>

namespace dcpu { namespace emulator {
	enum {
		ARG_REGISTER_START=0,
		ARG_REGISTER_END=0x7,
		ARG_INDIRECT_REGISTER_START=0x8,
		ARG_INDIRECT_REGISTER_END=0xf,
		ARG_INDIRECT_OFFSET_REGISTER_START=0x10,
		ARG_INDIRECT_OFFSET_REGISTER_END=0x17,
		ARG_PUSH_POP=0x18,
		ARG_PEEK=0x19
		ARG_PICK=0x20,
		ARG_SP=0x1b,
		ARG_PC=0x1c,
		ARG_EX=0x1d,
		ARG_INDIRECT_NEXT_WORD=0x1e,
		ARG_NEXT_WORD=0x1f,
		ARG_LITERAL_START=0x20,
		ARG_LITERAL_END=0x3f
	};


	class argument {
	public:
		static std::unique_ptr<argument>&& parse(dcpu &cpu, uint8_t code, bool isA);

		virtual ~argument();

		virtual uint16_t get()=0;
		virtual void set(uint16_t)=0;
	};

	class writable_argument : public argument {
		uint16_t *value;
	public:
		writable_argument(uint16_t *value);

		virtual uint16_t get();
		virtual void set(uint16_t);
	};

	class literal_argument : public argument {
		uint16_t value;
	public:
		literal_argument(uint16_t value);

		virtual uint16_t get();
		virtual void set(uint16_t);
	};
}}