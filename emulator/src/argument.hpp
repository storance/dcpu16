#pragma once

#include "dcpu.hpp"
#include <memory>
#include <string>

namespace dcpu { namespace emulator {
    class Argument;
    typedef std::unique_ptr<Argument> ArgumentPtr;

	class Argument {
	public:
		static ArgumentPtr parse(Dcpu &cpu, uint8_t code, bool isA);

		virtual ~Argument();

		virtual uint16_t get()  const=0;
		virtual void set(uint16_t)=0;
        virtual uint16_t getCycles() const;
        virtual std::string str() const=0;
	};

    class ReadOnlyArgument : public Argument {
    protected:
        uint16_t value;
        ReadOnlyArgument(uint16_t value);
    public:
        virtual uint16_t get() const;
        virtual void set(uint16_t);
    };

    class WritableArgument : public Argument {
    protected:
        uint16_t &value;
        WritableArgument(uint16_t &value);
    public:
        virtual uint16_t get() const;
        virtual void set(uint16_t);
    };

    class RegisterArgument : public WritableArgument {
        enum {START = 0, END=0x7, SP=0x1b, PC=0x1c, EX=0x1d};

        registers _register;
    public:
        RegisterArgument(Dcpu &cpu, registers _register);
        virtual std::string str() const;
    
        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

    class RegisterIndirectArgument : public WritableArgument {
        enum { START=0x8, END=0xf };

        registers _register;
    public:
        RegisterIndirectArgument(Dcpu &cpu, registers _register);
        virtual std::string str() const;
    
        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

    class RegisterIndirectOffsetArgument : public WritableArgument {
        enum { START = 0x10, END=0x17 };

        registers _register;
        uint16_t offset;
    public:
        RegisterIndirectOffsetArgument(Dcpu &cpu, registers _register, uint16_t offset);

        virtual uint16_t getCycles() const;
        virtual std::string str() const;
    
        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

    class StackPushArgument : public WritableArgument {
        enum { VALUE = 0x18 };
        
    public:
        StackPushArgument(Dcpu &cpu);
        virtual std::string str() const;

        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

    class StackPopArgument : public WritableArgument {
        enum { VALUE = 0x18 };
        
    public:
        StackPopArgument(Dcpu &cpu);
        virtual std::string str() const;

        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

    class StackPeekArgument : public WritableArgument {
        enum { VALUE = 0x19 };
    
    public:    
        StackPeekArgument(Dcpu &cpu);
        virtual std::string str() const;
    
        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

    class StackPickArgument : public WritableArgument {
        enum { VALUE = 0x1a };
        
        uint16_t offset;
    public:
        StackPickArgument(Dcpu &cpu, uint16_t offset);
        virtual std::string str() const;

        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

    class IndirectNextWordArgument : public WritableArgument {
        enum { VALUE = 0x1e };

        uint16_t nextWord;
    public:
        IndirectNextWordArgument(Dcpu &cpu, uint16_t nextWord);
        virtual std::string str() const;

        virtual uint16_t getCycles() const;
    
        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

    class NextWordArgument : public ReadOnlyArgument {
        enum { VALUE = 0x1f };
    public:
        NextWordArgument(uint16_t value);
        virtual std::string str() const;

        virtual uint16_t getCycles() const;

        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
    };

	class LiteralArgument : public ReadOnlyArgument {
        enum { START=0x20, END=0x3f};
    public:
        LiteralArgument(uint16_t value);
        virtual std::string str() const;

        static bool matches(uint8_t code, bool isA);
        static ArgumentPtr create(Dcpu &cpu, uint8_t code, bool isA);
	};
}}
