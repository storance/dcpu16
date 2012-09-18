#pragma once

#include "dcpu.hpp"
#include <memory>
#include <vector>
#include <functional>

#define HANDLE_ARGUMENT(arg, cpu, code, isA) if (arg::matches(code, isA)) { \
    return arg::create(cpu, code, isA); \
}

namespace dcpu { namespace emulator {
	class argument {
	public:
		static std::unique_ptr<argument> parse(dcpu &cpu, uint8_t code, bool isA);

		virtual ~argument();

		virtual uint16_t get()=0;
		virtual void set(uint16_t)=0;
	};


    class writable_argument : public argument {
    private:
        uint16_t *address;
    public:
        writable_argument();
        writable_argument(uint16_t *address);

		virtual uint16_t get();
		virtual void set(uint16_t);
    protected:
        void set_address(uint16_t *address);
    };

    class register_argument : public writable_argument {
        enum {START = 0, END=0x7, SP=0x1b, PC=0x1c, EX=0x1d};

        registers _register;
        register_argument(dcpu &cpu, registers _register);

    public:
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class register_indirect_argument : public writable_argument {
        enum { START=0x8, END=0xf };

        registers _register;
        register_indirect_argument(dcpu &cpu, registers _register);
    public:
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class register_indirect_offset_argument : public writable_argument {
        enum { START = 0x10, END=0x17 };

        registers _register;
        uint16_t offset;
    
        register_indirect_offset_argument(dcpu &cpu, registers _register, uint16_t offset);
    public:
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class stack_push_argument : public writable_argument {
        enum { VALUE = 0x18 };
        
        stack_push_argument(dcpu &cpu);
    public:
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class stack_pop_argument : public writable_argument {
        enum { VALUE = 0x18 };
        
        stack_pop_argument(dcpu &cpu);
    public:
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class stack_peek_argument : public writable_argument {
        enum { VALUE = 0x19 };
        
        stack_peek_argument(dcpu &cpu);
    public:
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class stack_pick_argument : public writable_argument {
        enum { VALUE = 0x1a };
        
        uint16_t offset;
        stack_pick_argument(dcpu &cpu, uint16_t offset);
    public:
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class indirect_next_word_argument : public writable_argument {
        enum { VALUE = 0x1e };

        uint16_t next_word;
        indirect_next_word_argument(dcpu &cpu);
    public:
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

	class literal_argument : public argument {
        enum { NEXT_WORD = 0x1f, START=0x20, END=0x3f};

		uint16_t value;
    public:
        literal_argument(uint16_t value);

        virtual uint16_t get();
        virtual void set(uint16_t);

        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
	};
}}
