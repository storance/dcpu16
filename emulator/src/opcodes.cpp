#include <stdexcept>

#include <boost/format.hpp>

#include "opcodes.hpp"

using namespace std;
using boost::format;
using boost::str;

#define BASIC_OPCODE_CASE(o) case o ## _opcode ::OPCODE: \
    return std::unique_ptr<opcode>(new o ## _opcode (cpu, arg_a, arg_b));

#define SPECIAL_OPCODE_CASE(o) case o ## _opcode ::OPCODE: \
    return std::unique_ptr<opcode>(new o ## _opcode (cpu, arg_a));


namespace dcpu { namespace emulator {
    unique_ptr<opcode> opcode::parse(dcpu &cpu, uint16_t instruction) {
        if ((instruction & 0x1f) != 0) {
            return move(parse_basic(cpu, instruction));
        } else {
            return move(parse_special(cpu, instruction));
        }
    }

    unique_ptr<opcode> opcode::parse_basic(dcpu &cpu, uint16_t instruction) {
        uint8_t o = instruction & 0x1f;
        uint8_t a = (instruction >> 10) & 0x3f;
        uint8_t b = (instruction >> 5) & 0x1f;

        unique_ptr<argument> arg_a = argument::parse(cpu, a, true);
        unique_ptr<argument> arg_b = argument::parse(cpu, b, false);

        switch (o) {
        BASIC_OPCODE_CASE(set)
        BASIC_OPCODE_CASE(add)
        BASIC_OPCODE_CASE(sub)
        BASIC_OPCODE_CASE(mul)
        BASIC_OPCODE_CASE(mli)
        BASIC_OPCODE_CASE(div)
        BASIC_OPCODE_CASE(dvi)
        BASIC_OPCODE_CASE(mod)
        BASIC_OPCODE_CASE(mdi)
        BASIC_OPCODE_CASE(and)
        BASIC_OPCODE_CASE(bor)
        BASIC_OPCODE_CASE(xor)
        BASIC_OPCODE_CASE(shr)
        BASIC_OPCODE_CASE(asr)
        BASIC_OPCODE_CASE(shl)
        BASIC_OPCODE_CASE(ifb)
        BASIC_OPCODE_CASE(ifc)
        BASIC_OPCODE_CASE(ife)
        BASIC_OPCODE_CASE(ifn)
        BASIC_OPCODE_CASE(ifg)
        BASIC_OPCODE_CASE(ifa)
        BASIC_OPCODE_CASE(ifl)
        BASIC_OPCODE_CASE(ifu)
        BASIC_OPCODE_CASE(adx)
        BASIC_OPCODE_CASE(sbx)
        BASIC_OPCODE_CASE(sti)
        BASIC_OPCODE_CASE(std)
        default:
            throw invalid_argument(str(format("Invalid basic opcode: %02x") % instruction));
            break;
        }
    }

    unique_ptr<opcode> opcode::parse_special(dcpu &cpu, uint16_t instruction) {
        uint8_t o = (instruction >> 5) & 0x1f;
        uint8_t a = (instruction >> 10) & 0x3f;

        unique_ptr<argument> arg_a = argument::parse(cpu, a, true);
        
        switch (o) {
        SPECIAL_OPCODE_CASE(jsr)
        SPECIAL_OPCODE_CASE(hcf)
        SPECIAL_OPCODE_CASE(int)
        SPECIAL_OPCODE_CASE(iag)
        SPECIAL_OPCODE_CASE(ias)
        SPECIAL_OPCODE_CASE(rfi)
        SPECIAL_OPCODE_CASE(iaq)
        SPECIAL_OPCODE_CASE(hwn)
        SPECIAL_OPCODE_CASE(hwq)
        SPECIAL_OPCODE_CASE(hwi)
        default:
            throw invalid_argument(str(format("Invalid special opcode: %02x") % instruction));
            break;
        }
    }

    opcode::opcode(dcpu &cpu, unique_ptr<argument> &a, std::unique_ptr<argument> &b, uint16_t cycles, bool is_cond,
            const std::string &name) : cpu(cpu), a(move(a)), b(move(b)), base_cycles(cycles), is_cond(is_cond),
            name(name) {

    }

    opcode::opcode(dcpu &cpu, unique_ptr<argument> &a, uint16_t cycles, bool is_cond, const std::string &name) 
            : cpu(cpu), a(move(a)), b(), base_cycles(cycles), is_cond(is_cond), name(name) {

    }

    opcode::~opcode() {

    }

    uint16_t opcode::calculate_cycles() const {
        return base_cycles + a->get_cycles() + (b ? b->get_cycles() : 0);
    }

    std::string opcode::get_name() const {
        return name;
    }
    
    bool opcode::is_conditional() const {
        return is_cond;
    }

    std::string opcode::to_str() const {
        if (b) {
            return str(format("%s %s, %s") % name % b->to_str() % a->to_str());
        } else {
            return str(format("%s %s") % name % a->to_str());
        }
    }

    uint16_t set_opcode::execute() {
        b->set(a->get());

        return calculate_cycles();
    }

    uint16_t add_opcode::execute() {
        uint32_t result = a->get() + b->get();
        b->set(result);

        cpu.registers.ex = result >> 16;

        return calculate_cycles();
    }

    uint16_t sub_opcode::execute() {
        uint32_t result = b->get() - a->get();
        b->set(result);

        cpu.registers.ex = result >> 16;

        return calculate_cycles();
    }

    uint16_t mul_opcode::execute() {
        uint32_t result = b->get() * a->get();
        b->set(result);

        cpu.registers.ex = (result >> 16) & 0xffff;

        return calculate_cycles();
    }

    uint16_t mli_opcode::execute() {
        int16_t signedA = a->get();
        int16_t signedB = b->get();

        int32_t result = signedA * signedB;
        b->set(result);

        cpu.registers.ex = (result >> 16) & 0xffff;

        return calculate_cycles();
    }

    uint16_t div_opcode::execute() {
        uint32_t unsignedA = a->get();
        uint32_t unsignedB = b->get();

        if (unsignedA == 0) {
            cpu.registers.ex = 0;
            b->set(0);
        } else {
            uint32_t result = (unsignedB << 16) / unsignedA;
            cpu.registers.ex = result & 0xffff;
            b->set(result >> 16);
        }

        return calculate_cycles();
    }

    uint16_t dvi_opcode::execute() {
        int32_t signedA = (int16_t)a->get();
        int32_t signedB = (int16_t)b->get();

        if (signedA == 0) {
            cpu.registers.ex = 0;
            b->set(0);
        } else {
            int32_t result = (signedB << 16) / signedA;
            cpu.registers.ex = result & 0xffff;
            b->set(result >> 16);
        }

        return calculate_cycles();
    }

    uint16_t mod_opcode::execute() {
        if (a->get() == 0) {
            cpu.registers.ex = 0;
            b->set(0);
        } else {
            b->set(b->get() % a->get());
        }

        return calculate_cycles();
    }

    uint16_t mdi_opcode::execute() {
        int16_t signedA = (int16_t)a->get();
        int16_t signedB = (int16_t)b->get();

        if (signedA == 0) {
            cpu.registers.ex = 0;
            b->set(0);
        } else {
            b->set(signedB % signedA);
        }

        return calculate_cycles();
    }

    uint16_t and_opcode::execute() {
        b->set(b->get() & a->get());

        return calculate_cycles();
    }

    uint16_t bor_opcode::execute() {
        b->set(b->get() | a->get());

        return calculate_cycles();
    }

    uint16_t xor_opcode::execute() {
        b->set(b->get() ^ a->get());

        return calculate_cycles();
    }

    uint16_t shr_opcode::execute() {
        uint16_t unsignedA = a->get();
        uint16_t unsignedB = b->get();

        b->set(unsignedB >> unsignedA);
        cpu.registers.ex = ((unsignedB << 16) >> unsignedA) & 0xffff;

        return calculate_cycles();
    }

    uint16_t asr_opcode::execute() {
        uint16_t unsignedA = a->get();
        int16_t signedB = b->get();

        b->set(signedB >> unsignedA);
        cpu.registers.ex = ((signedB << 16) >> unsignedA) & 0xffff;

        return calculate_cycles();
    }

    uint16_t shl_opcode::execute() {
        uint16_t unsignedA = a->get();
        uint16_t unsignedB = b->get();

        uint32_t result = unsignedB << unsignedA;
        b->set(result);
        cpu.registers.ex = (result >> 16) & 0xffff;

        return calculate_cycles();
    }

    uint16_t ifb_opcode::execute() {
        if ((b->get() & a->get()) == 0) {
            cpu.skip_next_instruction();
        }

        return calculate_cycles();
    }

    uint16_t ifc_opcode::execute() {
        if ((b->get() & a->get()) != 0) {
            cpu.skip_next_instruction();
        }

        return calculate_cycles();
    }

    uint16_t ife_opcode::execute() {
        if (b->get() != a->get()) {
            cpu.skip_next_instruction();
        }

        return calculate_cycles();
    }

    uint16_t ifn_opcode::execute() {
        if (b->get() == a->get()) {
            cpu.skip_next_instruction();
        }

        return calculate_cycles();
    }

    uint16_t ifg_opcode::execute() {
        if (b->get() <= a->get()) {
            cpu.skip_next_instruction();
        }

        return calculate_cycles();
    }

    uint16_t ifa_opcode::execute() {
        int16_t signedB = b->get();
        int16_t signedA = a->get();

        if (signedB <= signedA) {
            cpu.skip_next_instruction();
        }

        return calculate_cycles();
    }

    uint16_t ifl_opcode::execute() {
        if (b->get() >= a->get()) {
            cpu.skip_next_instruction();
        }

        return calculate_cycles();
    }

    uint16_t ifu_opcode::execute() {
        int16_t signedB = b->get();
        int16_t signedA = a->get();

        if (signedB >= signedA) {
            cpu.skip_next_instruction();
        }

        return calculate_cycles();
    }

    uint16_t adx_opcode::execute() {
        uint32_t result = b->get() + a->get() + cpu.registers.ex;

        b->set(result);
        cpu.registers.ex = result >> 16;

        return calculate_cycles();
    }

    uint16_t sbx_opcode::execute() {
        uint32_t result = b->get() - a->get() + cpu.registers.ex;
        b->set(result);

        cpu.registers.ex = result >> 16;

        return calculate_cycles();
    }

    uint16_t sti_opcode::execute() {
        b->set(a->get());

        ++cpu.registers.i;
        ++cpu.registers.j;

        return calculate_cycles();
    }

    uint16_t std_opcode::execute() {
        b->set(a->get());

        --cpu.registers.i;
        --cpu.registers.j;

        return calculate_cycles();
    }

    uint16_t jsr_opcode::execute() {
        cpu.stack.push(cpu.registers.pc);
        cpu.registers.pc = a->get();

        return calculate_cycles();
    }

    uint16_t hcf_opcode::execute() {
        cpu.catch_fire();

        return calculate_cycles();
    }

    uint16_t int_opcode::execute() {
        cpu.interrupt_handler.send(a->get());

        return calculate_cycles();
    }

    uint16_t iag_opcode::execute() {
        a->set(cpu.registers.ia);

        return calculate_cycles();
    }

    uint16_t ias_opcode::execute() {
        cpu.registers.ia = a->get();

        return calculate_cycles();
    }

    uint16_t rfi_opcode::execute() {
        cpu.interrupt_handler.disable_queue();
        cpu.registers.a = cpu.stack.pop();
        cpu.registers.pc = cpu.stack.pop();

        return calculate_cycles();
    }

    uint16_t iaq_opcode::execute() {
        if (a->get() != 0) {
            cpu.interrupt_handler.enable_queue();
        } else {
            cpu.interrupt_handler.disable_queue();
        }

        return calculate_cycles();
    }

    uint16_t hwn_opcode::execute() {
        a->set(cpu.hardware_manager.get_count());

        return calculate_cycles();
    }

    uint16_t hwq_opcode::execute() {
        cpu.hardware_manager.query(a->get());

        return calculate_cycles();
    }

    uint16_t hwi_opcode::execute() {
        uint16_t extra_cycles = cpu.hardware_manager.interrupt(a->get());

        return calculate_cycles() + extra_cycles;
    }
}}
