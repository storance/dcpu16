#include <stdexcept>
#include <boost/format.hpp>

#include "argument.hpp"

using namespace std;
using boost::format;
using boost::str;

#define HANDLE_ARGUMENT(arg, cpu, code, isA) if (arg::matches(code, isA)) { \
    return arg::create(cpu, code, isA); \
}

namespace dcpu { namespace emulator {
    /*************************************************************************
     *
     * Argument
     *
     *************************************************************************/

	ArgumentPtr Argument::parse(Dcpu &cpu, uint8_t code, bool isA) {
        HANDLE_ARGUMENT(RegisterArgument, cpu, code, isA)
        HANDLE_ARGUMENT(RegisterIndirectArgument, cpu, code, isA)
        HANDLE_ARGUMENT(RegisterIndirectOffsetArgument, cpu, code, isA)
        HANDLE_ARGUMENT(StackPushArgument, cpu, code, isA)
        HANDLE_ARGUMENT(StackPopArgument, cpu, code, isA)
        HANDLE_ARGUMENT(StackPeekArgument, cpu, code, isA)
        HANDLE_ARGUMENT(StackPickArgument, cpu, code, isA)
        HANDLE_ARGUMENT(IndirectNextWordArgument, cpu, code, isA)
        HANDLE_ARGUMENT(NextWordArgument, cpu, code, isA)
        HANDLE_ARGUMENT(LiteralArgument, cpu, code, isA)

        throw invalid_argument(::str(format("Invalid Argument code: %02x") % code));
	}

	Argument::~Argument() {

	}

    uint16_t Argument::getCycles() const {
        return 0;
    }

    /*************************************************************************
     *
     * WritableArgument
     *
     *************************************************************************/
	WritableArgument::WritableArgument(uint16_t &value) : value(value) {

	}

	uint16_t WritableArgument::get()  const {
		return value;
	}

	void WritableArgument::set(uint16_t value) {
		this->value = value;
	}

    /*************************************************************************
     *
     * ReadOnlyArgument
     *
     *************************************************************************/

    ReadOnlyArgument::ReadOnlyArgument(uint16_t value) : value(value) {

    }

    uint16_t ReadOnlyArgument::get() const {
        return value;
    }
    
    void ReadOnlyArgument::set(uint16_t) {
        // no-op
    }

    /*************************************************************************
     *
     * RegisterArgument
     *
     *************************************************************************/
    
    RegisterArgument::RegisterArgument(Dcpu &cpu, registers _register) 
            : WritableArgument(cpu.registers[_register]), _register(_register) {
    }

    string RegisterArgument::str() const {
        return ::str(format("%s") % _register);
    }

    bool RegisterArgument::matches(uint8_t code, bool isA) {
        return (code >= START && code <= END) || code == PC || code == EX || code == SP;
    }

    ArgumentPtr RegisterArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        registers reg;
        if (code >= START && code <= END) {
            reg =  static_cast<registers>(code - START);
        } else if (code == SP) {
            reg = registers::SP;
        } else if (code == PC) {
            reg = registers::PC;
        } else if (code == EX) {
            reg = registers::EX;
        } else {
            // invalid arg
        }

        return ArgumentPtr(new RegisterArgument(cpu, reg));
    }

    /*************************************************************************
     *
     * RegisterIndirectArgument
     *
     *************************************************************************/
    
    RegisterIndirectArgument::RegisterIndirectArgument(Dcpu &cpu, registers _register) 
            : WritableArgument(cpu.registers.indirect(_register)), _register(_register) {
    }

    string RegisterIndirectArgument::str() const {
        return ::str(format("[%s]") % _register);
    }
 
    bool RegisterIndirectArgument::matches(uint8_t code, bool isA) {
        return (code >= START && code <= END);
    }
    
    ArgumentPtr RegisterIndirectArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        registers reg = static_cast<registers>(code - START);
        
        return ArgumentPtr(new RegisterIndirectArgument(cpu, reg));
    }

    /*************************************************************************
     *
     * RegisterIndirectOffsetArgument
     *
     *************************************************************************/

    RegisterIndirectOffsetArgument::RegisterIndirectOffsetArgument(Dcpu &cpu, registers _register,
            uint16_t offset) : WritableArgument(cpu.registers.indirect(_register,  offset)), _register(_register),
            offset(offset) {
    }

    uint16_t RegisterIndirectOffsetArgument::getCycles() const {
        return 1;
    }

    string RegisterIndirectOffsetArgument::str() const {
        return ::str(format("[%s + %d]") % _register % offset);
    }

    bool RegisterIndirectOffsetArgument::matches(uint8_t code, bool isA) {
        return (code >= START && code <= END);
    }
    
    ArgumentPtr RegisterIndirectOffsetArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        registers reg = static_cast<registers>(code - START);
        
        return ArgumentPtr(new RegisterIndirectOffsetArgument(cpu, reg, cpu.getNextWord()));
    }

    /*************************************************************************
     *
     * StackPushArgument
     *
     *************************************************************************/
    
    StackPushArgument::StackPushArgument(Dcpu &cpu) : WritableArgument(cpu.stack.push()) {

    }

    string StackPushArgument::str() const {
        return "PUSH";
    }

    bool StackPushArgument::matches(uint8_t code, bool isA) {
        return !isA && code == VALUE;
    }
    
    ArgumentPtr StackPushArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        return ArgumentPtr(new StackPushArgument(cpu));
    }

    /*************************************************************************
     *
     * StackPopArgument
     *
     *************************************************************************/
    StackPopArgument::StackPopArgument(Dcpu &cpu) : WritableArgument(cpu.stack.pop()) {

    }

    string StackPopArgument::str() const {
        return "POP";
    }

    bool StackPopArgument::matches(uint8_t code, bool isA) {
        return isA && code == VALUE;
    }
    
    ArgumentPtr StackPopArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        return ArgumentPtr(new StackPopArgument(cpu));
    }
    
    /*************************************************************************
     *
     * StackPeekArgument
     *
     *************************************************************************/
    StackPeekArgument::StackPeekArgument(Dcpu &cpu) : WritableArgument(cpu.stack.peek()) {

    }

    string StackPeekArgument::str() const {
        return "PEEK";
    }

    bool StackPeekArgument::matches(uint8_t code, bool isA) {
        return code == VALUE;
    }
    
    ArgumentPtr StackPeekArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        return ArgumentPtr(new StackPeekArgument(cpu));
    }
    
    /*************************************************************************
     *
     * StackPickArgument
     *
     *************************************************************************/
    StackPickArgument::StackPickArgument(Dcpu &cpu, uint16_t offset) 
        : WritableArgument(cpu.stack.pick(offset)), offset(offset)  {

    }

    string StackPickArgument::str() const {
        return ::str(format("PICK %d") % offset);
    }

    bool StackPickArgument::matches(uint8_t code, bool isA) {
        return code == VALUE;
    }
    
    ArgumentPtr StackPickArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        return ArgumentPtr(new StackPickArgument(cpu, cpu.getNextWord()));
    }

    /*************************************************************************
     *
     * IndirectNextWordArgument
     *
     *************************************************************************/
    IndirectNextWordArgument::IndirectNextWordArgument(Dcpu &cpu, uint16_t nextWord) 
            : WritableArgument(cpu.memory[nextWord]), nextWord(nextWord) {
    }

    uint16_t IndirectNextWordArgument::getCycles() const {
        return 1;
    }

    string IndirectNextWordArgument::str() const {
        return ::str(format("[%d]") % nextWord);
    }

    bool IndirectNextWordArgument::matches(uint8_t code, bool isA) {
        return code == VALUE;
    }
    
    ArgumentPtr IndirectNextWordArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        return ArgumentPtr(new IndirectNextWordArgument(cpu, cpu.getNextWord()));
    }

    /*************************************************************************
     *
     * NextWordArgument
     *
     *************************************************************************/
    NextWordArgument::NextWordArgument(uint16_t value) : ReadOnlyArgument(value) {

    }

    uint16_t NextWordArgument::getCycles() const {
        return 1;
    }

    string NextWordArgument::str() const {
        return ::str(format("%d") % value);
    }

    bool NextWordArgument::matches(uint8_t code, bool isA) {
        return code == VALUE;
    }

    ArgumentPtr NextWordArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        return ArgumentPtr(new NextWordArgument(cpu.getNextWord()));
    }
    
    /*************************************************************************
     *
     * LiteralArgument
     *
     *************************************************************************/

	LiteralArgument::LiteralArgument(uint16_t value) : ReadOnlyArgument(value) {

	}

    string LiteralArgument::str() const {
        return ::str(format("%d") % value);
    }

    bool LiteralArgument::matches(uint8_t code, bool isA) {
        return code >= START && code <= END;
    }
    
    ArgumentPtr LiteralArgument::create(Dcpu &cpu, uint8_t code, bool isA) {
        uint16_t value = code - START - 1;

        return ArgumentPtr(new LiteralArgument(value));
    }
}}
