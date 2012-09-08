#include "argument.hpp"

namespace dcpu { namespace emulator {
	std::unique_ptr<argument>&& argument::parse(dcpu &cpu, uint8_t code, bool isA) {
		if (code >= ARG_REGISTER_START && code <= ARG_REGISTER_END) {
			return new writable_argument(cpu->registers + code);
		} else if (code >= ARG_INDIRECT_REGISTER_START && code <= ARG_INDIRECT_REGISTER_END) {
			uint16_t offset = cpu->registers[code- ARG_INDIRECT_REGISTER_START];
			return new writable_argument(cpu->memory + offset);
		} else if (code >= ARG_INDIRECT_OFFSET_REGISTER_START && code <= ARG_INDIRECT_OFFSET_REGISTER_END) {
			uint16_t offset = cpu->registers[code - ARG_INDIRECT_OFFSET_REGISTER_START];
			return new writable_argument(cpu->memory + offset);
		} else if (code == ARG_PUSH_POP && isA) {
			return new writable_argument(cpu->memory + dcpu->sp++);
		} else if (code == ARG_PUSH_POP && !isA) {
			return new writable_argument(cpu->memory + --dcpu->sp);
		} else if (code == ARG_PEEK) {
			return new writable_argument(cpu->memory + dcpu->sp);
		} else if (code == ARG_PICK) {
			uint16_t offset = dcpu->sp + cpu->get_next_word();
			return new writable_argument(cpu->memory + offset);
		} else if (code == ARG_SP) {
			return new writable_argument(&cpu->sp);
		} else if (code == ARG_PC) {
			return new writable_argument(&cpu->pc);
		} else if (code == ARG_EX) {
			return new writable_argument(&cpu->ex);
		} else if (code == ARG_INDIRECT_NEXT_WORD) {
			return new writable_argument(cpu->memory + cpu->get_next_word());
		} else if (code == ARG_NEXT_WORD) {
			return new literal_argument(cpu->get_next_word());
		} else if (code >= ARG_LITERAL_START && code <= ARG_LITERAL_END && isA) {
			return new literal_argument(code - ARG_LITERAL_START - 1);
		}

		// TODO: throw exception
	}

	argument::~argument() {

	}

    writable_argument::writable_argument() : address(nullptr) {

    }

	writable_argument::writable_argument(uint16_t* address) : address(address) {

	}

	uint16_t writable_argument::get() {
        assert address;

		return *address;
	}

	void writable_argument::set(uint16_t value) {
        assert address;

		*this->address = address;
	}

    
    register_argument::register_argument(dcpu &cpu, registers _register) 
        : writable_argument(), _register(_register) {
        set_address(cpu->registers + static_cast<uint16_t>(_register));
    }

    
    register_indirect_argument::register_indirect_argument(dcpu &cpu, registers _register)
        : writable_argument(), _register(_register), offset() {
        uint16_t register_value = cpu->read_register(_register);
        set_address(cpu->memory + register_value);
    }

    register_indirect_argument::register_indirect_argument(dcpu &cpu, registers _register, uint16_t offset)
        : writable_argument(), _register(_register), offset(offset) {

        uint16_t register_value = cpu->read_register(_register);
        set_address(cpu->memory + register_value + offset);
    }

    
    stack_push_argument::stack_push_argument(dcpu &cpu) : writable_argument(cpu->memory + cpu->sp++) {

    }

    stack_pop_argument::stack_pop_argument(dcpu &cpu) : writable_argument(cpu->memory + --cpu->sp) {

    }
    
    stack_peek_argument::stack_push_argument(dcpu &cpu) : writable_argument(cpu->memory + cpu->sp) {

    }
    
    stack_push_argument::stack_pick_argument(dcpu &cpu, uint16_t offset) 
        : writable_argument(cpu->memory + cpu->sp + offset), offset(offset)  {

    }

    indirect_next_word_argument::indirect_next_word_argument(dcpu &cpu) 
        : writable_argument(), next_word(cpu->get_next_word()) {
        set_address(cpu->memory + next_word);
    }
    
    next_word_argument::next_word_argument(dcpu &cpu) 
        : next_word(cpu->get_next_word()) {
    }

    uint16_t next_word_argument::get() {
        return next_word;
    }

    void next_word_argument::set(uint16_t value) {
        // no-op
    }

	literal_argument::literal_argument(uint16_t value) : value(value) {

	}

	uint16_t literal_argument::get() {
		return value;
	}

	void literal_argument::set(uint16_t value) {
		// no-op
	}
}}
