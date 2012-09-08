#pragma once

#include <memory>
#include <boost/optional.hpp>

namespace dcpu { namespace emulator {
	class argument {
	public:
		static std::unique_ptr<argument>&& parse(dcpu &cpu, uint8_t code, bool isA);

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
    }

    class register_argument : public writable_argument {
        registers _register;
    public:
        enum {START = 0, END=0x7, SP=0x1b, PC=0x1c, EX=0x1d};

        register_argument(dcpu &cpu, registers _register);
    };

    class register_indirect_argument : public writable_argument {
        registers _register;
        boost::optional<uint16_t> offset;
    public:
        enum {
		    START=0x8,
		    END=0xf,
		    OFFSET_START=0x10,
		    OFFSET_END=0x17
        };

        register_indirect_argument(dcpu &cpu, registers _register);
        register_indirect_argument(dcpu &cpu, registers _register, uint16_t offset);
    };

    class stack_push_argument : public writable_argument {
    public:
        enum { VALUE = 0x18 };
        stack_push_argument(dcpu &cpu);
    };

    class stack_pop_argument : public writable_argument {
    public:
        enum { VALUE = 0x18 };
        stack_pop_argument(dcpu &cpu);
    };

    class stack_peek_argument : public writable_argument {
    public:
        enum { VALUE = 0x19 };
        stack_peek_argument(dcpu &cpu);
    };

    class stack_pick_argument : public writable_argument {
        uint16_t offset;
    public:
        enum { VALUE = 0x1a };
        stack_pick_argument(dcpu &cpu, uint16_t offset);
    };

    class indirect_next_word_argument : public writable_argument {
        uint16_t next_word;
    public:
        indirect_next_word_argument(dcpu &cpu);
    };

    class next_word_argument : public argument {
        uint16_t next_word;
    public:
        next_word_argument(dcpu &cpu);
		
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
