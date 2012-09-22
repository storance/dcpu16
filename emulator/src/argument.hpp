#pragma once

#include "dcpu.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace dcpu { namespace emulator {
	class argument {
	public:
		static std::unique_ptr<argument> parse(dcpu &cpu, uint8_t code, bool isA);

		virtual ~argument();

		virtual uint16_t get()  const=0;
		virtual void set(uint16_t)=0;
        virtual uint16_t get_cycles() const;
	};

    class readonly_argument : public argument {
        uint16_t value;
    protected:
        readonly_argument(uint16_t value);
    public:
        virtual uint16_t get() const;
        virtual void set(uint16_t);
    };

    class writable_argument : public argument {
        uint16_t &value;
    protected:
        writable_argument(uint16_t &value);
    public:
        virtual uint16_t get() const;
        virtual void set(uint16_t);
    };

    class register_argument : public writable_argument {
        enum {START = 0, END=0x7, SP=0x1b, PC=0x1c, EX=0x1d};

        registers _register;
    public:
        register_argument(dcpu &cpu, registers _register);
    
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class register_indirect_argument : public writable_argument {
        enum { START=0x8, END=0xf };

        registers _register;
    public:
        register_indirect_argument(dcpu &cpu, registers _register);
    
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class register_indirect_offset_argument : public writable_argument {
        enum { START = 0x10, END=0x17 };

        registers _register;
        uint16_t offset;
    public:
        register_indirect_offset_argument(dcpu &cpu, registers _register, uint16_t offset);

        virtual uint16_t get_cycles() const;
    
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class stack_push_argument : public writable_argument {
        enum { VALUE = 0x18 };
        
    public:
        stack_push_argument(dcpu &cpu);

        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class stack_pop_argument : public writable_argument {
        enum { VALUE = 0x18 };
        
    public:
        stack_pop_argument(dcpu &cpu);

        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class stack_peek_argument : public writable_argument {
        enum { VALUE = 0x19 };
    
    public:    
        stack_peek_argument(dcpu &cpu);
    
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class stack_pick_argument : public writable_argument {
        enum { VALUE = 0x1a };
        
        uint16_t offset;
    public:
        stack_pick_argument(dcpu &cpu, uint16_t offset);

        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class indirect_next_word_argument : public writable_argument {
        enum { VALUE = 0x1e };

        uint16_t next_word;
    public:
        indirect_next_word_argument(dcpu &cpu, uint16_t next_word);

        virtual uint16_t get_cycles() const;
    
        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

    class next_word_argument : public readonly_argument {
        enum { VALUE = 0x1f };
    public:
        next_word_argument(uint16_t value);

        virtual uint16_t get_cycles() const;

        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
    };

	class literal_argument : public readonly_argument {
        enum { START=0x20, END=0x3f};
    public:
        literal_argument(uint16_t value);

        static bool matches(uint8_t code, bool isA);
        static std::unique_ptr<argument> create(dcpu &cpu, uint8_t code, bool isA);
	};
}}
