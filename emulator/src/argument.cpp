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
     * argument
     *
     *************************************************************************/

	unique_ptr<argument> argument::parse(dcpu &cpu, uint8_t code, bool isA) {
        HANDLE_ARGUMENT(register_argument, cpu, code, isA)
        HANDLE_ARGUMENT(register_indirect_argument, cpu, code, isA)
        HANDLE_ARGUMENT(register_indirect_offset_argument, cpu, code, isA)
        HANDLE_ARGUMENT(stack_push_argument, cpu, code, isA)
        HANDLE_ARGUMENT(stack_pop_argument, cpu, code, isA)
        HANDLE_ARGUMENT(stack_peek_argument, cpu, code, isA)
        HANDLE_ARGUMENT(stack_pick_argument, cpu, code, isA)
        HANDLE_ARGUMENT(indirect_next_word_argument, cpu, code, isA)
        HANDLE_ARGUMENT(next_word_argument, cpu, code, isA)
        HANDLE_ARGUMENT(literal_argument, cpu, code, isA)

        throw invalid_argument(str(format("Invalid argument code: %02x") % code));
	}

	argument::~argument() {

	}

    uint16_t argument::get_cycles() const {
        return 0;
    }

    /*************************************************************************
     *
     * writable_argument
     *
     *************************************************************************/
	writable_argument::writable_argument(uint16_t &value) : value(value) {

	}

	uint16_t writable_argument::get()  const {
		return value;
	}

	void writable_argument::set(uint16_t value) {
		this->value = value;
	}

    /*************************************************************************
     *
     * readonly_argument
     *
     *************************************************************************/

    readonly_argument::readonly_argument(uint16_t value) : value(value) {

    }

    uint16_t readonly_argument::get() const {
        return value;
    }
    
    void readonly_argument::set(uint16_t) {
        // no-op
    }

    /*************************************************************************
     *
     * register_argument
     *
     *************************************************************************/
    
    register_argument::register_argument(dcpu &cpu, registers _register) 
            : writable_argument(cpu.registers[_register]), _register(_register) {
    }

    string register_argument::to_str() const {
        return str(format("%s") % _register);
    }

    bool register_argument::matches(uint8_t code, bool isA) {
        return (code >= START && code <= END) || code == PC || code == EX || code == SP;
    }

    unique_ptr<argument> register_argument::create(dcpu &cpu, uint8_t code, bool isA) {
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

        return unique_ptr<argument>(new register_argument(cpu, reg));
    }

    /*************************************************************************
     *
     * register_indirect_argument
     *
     *************************************************************************/
    
    register_indirect_argument::register_indirect_argument(dcpu &cpu, registers _register) 
            : writable_argument(cpu.registers.indirect(_register)), _register(_register) {
    }

    string register_indirect_argument::to_str() const {
        return str(format("[%s]") % _register);
    }
 
    bool register_indirect_argument::matches(uint8_t code, bool isA) {
        return (code >= START && code <= END);
    }
    
    unique_ptr<argument> register_indirect_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        registers reg = static_cast<registers>(code - START);
        
        return unique_ptr<argument>(new register_indirect_argument(cpu, reg));
    }

    /*************************************************************************
     *
     * register_indirect_offset_argument
     *
     *************************************************************************/

    register_indirect_offset_argument::register_indirect_offset_argument(dcpu &cpu, registers _register,
            uint16_t offset) : writable_argument(cpu.registers.indirect(_register,  offset)), _register(_register),
            offset(offset) {
    }

    uint16_t register_indirect_offset_argument::get_cycles() const {
        return 1;
    }

    string register_indirect_offset_argument::to_str() const {
        return str(format("[%s + %d]") % _register % offset);
    }

    bool register_indirect_offset_argument::matches(uint8_t code, bool isA) {
        return (code >= START && code <= END);
    }
    
    unique_ptr<argument> register_indirect_offset_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        registers reg = static_cast<registers>(code - START);
        
        return unique_ptr<argument>(new register_indirect_offset_argument(cpu, reg, cpu.get_next_word()));
    }

    /*************************************************************************
     *
     * stack_push_argument
     *
     *************************************************************************/
    
    stack_push_argument::stack_push_argument(dcpu &cpu) : writable_argument(cpu.stack.push()) {

    }

    string stack_push_argument::to_str() const {
        return "PUSH";
    }

    bool stack_push_argument::matches(uint8_t code, bool isA) {
        return !isA && code == VALUE;
    }
    
    unique_ptr<argument> stack_push_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        return unique_ptr<argument>(new stack_push_argument(cpu));
    }

    /*************************************************************************
     *
     * stack_pop_argument
     *
     *************************************************************************/
    stack_pop_argument::stack_pop_argument(dcpu &cpu) : writable_argument(cpu.stack.pop()) {

    }

    string stack_pop_argument::to_str() const {
        return "POP";
    }

    bool stack_pop_argument::matches(uint8_t code, bool isA) {
        return isA && code == VALUE;
    }
    
    unique_ptr<argument> stack_pop_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        return unique_ptr<argument>(new stack_pop_argument(cpu));
    }
    
    /*************************************************************************
     *
     * stack_peek_argument
     *
     *************************************************************************/
    stack_peek_argument::stack_peek_argument(dcpu &cpu) : writable_argument(cpu.stack.peek()) {

    }

    string stack_peek_argument::to_str() const {
        return "PEEK";
    }

    bool stack_peek_argument::matches(uint8_t code, bool isA) {
        return code == VALUE;
    }
    
    unique_ptr<argument> stack_peek_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        return unique_ptr<argument>(new stack_peek_argument(cpu));
    }
    
    /*************************************************************************
     *
     * stack_pick_argument
     *
     *************************************************************************/
    stack_pick_argument::stack_pick_argument(dcpu &cpu, uint16_t offset) 
        : writable_argument(cpu.stack.pick(offset)), offset(offset)  {

    }

    string stack_pick_argument::to_str() const {
        return str(format("PICK %d") % offset);
    }

    bool stack_pick_argument::matches(uint8_t code, bool isA) {
        return code == VALUE;
    }
    
    unique_ptr<argument> stack_pick_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        return unique_ptr<argument>(new stack_pick_argument(cpu, cpu.get_next_word()));
    }

    /*************************************************************************
     *
     * indirect_next_word_argument
     *
     *************************************************************************/
    indirect_next_word_argument::indirect_next_word_argument(dcpu &cpu, uint16_t next_word) 
            : writable_argument(cpu.memory[next_word]), next_word(next_word) {
    }

    uint16_t indirect_next_word_argument::get_cycles() const {
        return 1;
    }

    string indirect_next_word_argument::to_str() const {
        return str(format("[%d]") % next_word);
    }

    bool indirect_next_word_argument::matches(uint8_t code, bool isA) {
        return code == VALUE;
    }
    
    unique_ptr<argument> indirect_next_word_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        return unique_ptr<argument>(new indirect_next_word_argument(cpu, cpu.get_next_word()));
    }

    /*************************************************************************
     *
     * next_word_argument
     *
     *************************************************************************/
    next_word_argument::next_word_argument(uint16_t value) : readonly_argument(value) {

    }

    uint16_t next_word_argument::get_cycles() const {
        return 1;
    }

    string next_word_argument::to_str() const {
        return str(format("%d") % value);
    }

    bool next_word_argument::matches(uint8_t code, bool isA) {
        return code == VALUE;
    }

    unique_ptr<argument> next_word_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        return unique_ptr<argument>(new next_word_argument(cpu.get_next_word()));
    }
    
    /*************************************************************************
     *
     * literal_argument
     *
     *************************************************************************/

	literal_argument::literal_argument(uint16_t value) : readonly_argument(value) {

	}

    string literal_argument::to_str() const {
        return str(format("%d") % value);
    }

    bool literal_argument::matches(uint8_t code, bool isA) {
        return code >= START && code <= END;
    }
    
    unique_ptr<argument> literal_argument::create(dcpu &cpu, uint8_t code, bool isA) {
        uint16_t value = code - START - 1;

        return unique_ptr<argument>(new literal_argument(value));
    }
}}
