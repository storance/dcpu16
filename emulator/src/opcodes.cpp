#include "opcodes.hpp"

using namespace std;

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
        BASIC_OPCODE_CASE(set, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(add, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(sub, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(mul, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(mli, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(div, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(dvi, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(mod, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(mdi, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(and, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(bor, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(xor, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(shr, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(asr, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(shl, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(ifb, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(ifc, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(ife, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(ifn, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(ifg, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(ifa, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(ifl, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(ifu, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(adx, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(sbx, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(sti, cpu, arg_a, arg_b)
        BASIC_OPCODE_CASE(std, cpu, arg_a, arg_b)
        default:
            // throw not supported
            break;
        }
    }

    unique_ptr<opcode> opcode::parse_special(dcpu &cpu, uint16_t instruction) {
        uint8_t o = (instruction >> 5) & 0x1f;
        uint8_t a = (instruction >> 10) & 0x3f;

        unique_ptr<argument> arg_a = argument::parse(cpu, a, true);
        
        switch (o) {
        SPECIAL_OPCODE_CASE(jsr, cpu, arg_a)
        SPECIAL_OPCODE_CASE(hcf, cpu, arg_a)
        SPECIAL_OPCODE_CASE(int, cpu, arg_a)
        SPECIAL_OPCODE_CASE(iag, cpu, arg_a)
        SPECIAL_OPCODE_CASE(ias, cpu, arg_a)
        SPECIAL_OPCODE_CASE(rfi, cpu, arg_a)
        SPECIAL_OPCODE_CASE(iaq, cpu, arg_a)
        SPECIAL_OPCODE_CASE(hwn, cpu, arg_a)
        SPECIAL_OPCODE_CASE(hwq, cpu, arg_a)
        SPECIAL_OPCODE_CASE(hwi, cpu, arg_a)
        default:
            // throw not supported
            break;
        }
    }

    opcode::opcode(dcpu &cpu, unique_ptr<argument> &a, std::unique_ptr<argument> &b) : cpu(cpu), a(move(a)), b(move(b)) {

    }

    opcode::opcode(dcpu &cpu, unique_ptr<argument> &a) : cpu(cpu), a(move(a)), b() {

    }

    opcode::~opcode() {

    }

    uint16_t set_opcode::execute() {
        b->set(a->get());

        return CYCLES;
    }

    uint16_t add_opcode::execute() {
        uint32_t result = a->get() + b->get();
        b->set(result);

        cpu.ex = result >> 16;

        return CYCLES;
    }

    uint16_t sub_opcode::execute() {
        uint32_t result = b->get() - a->get();
        b->set(result);

        cpu.ex = result >> 16;

        return CYCLES;
    }

    uint16_t mul_opcode::execute() {
        uint32_t result = b->get() * a->get();
        b->set(result);

        cpu.ex = (result >> 16) & 0xffff;

        return CYCLES;
    }

    uint16_t mli_opcode::execute() {
        int16_t signedA = a->get();
        int16_t signedB = b->get();

        int32_t result = signedA * signedB;
        b->set(result);

        cpu.ex = (result >> 16) & 0xffff;

        return CYCLES;
    }

    uint16_t div_opcode::execute() {
        uint32_t unsignedA = a->get();
        uint32_t unsignedB = b->get();

        if (unsignedA == 0) {
            cpu.ex = 0;
            b->set(0);
        } else {
            uint32_t result = (unsignedB << 16) / unsignedA;
            cpu.ex = result & 0xffff;
            b->set(result >> 16);
        } 

        return CYCLES;
    }

    uint16_t dvi_opcode::execute() {
        int32_t signedA = (int16_t)a->get();
        int32_t signedB = (int16_t)b->get();

        if (signedA == 0) {
            cpu.ex = 0;
            b->set(0);
        } else {
            int32_t result = (signedB << 16) / signedA;
            cpu.ex = result & 0xffff;
            b->set(result >> 16);
        } 

        return CYCLES;
    }

    uint16_t mod_opcode::execute() {
        if (a->get() == 0) {
            cpu.ex = 0;
            b->set(0);
        } else {
            b->set(b->get() % a->get());
        } 

        return CYCLES;
    }

    uint16_t mdi_opcode::execute() {
        int16_t signedA = (int16_t)a->get();
        int16_t signedB = (int16_t)b->get();

        if (signedA == 0) {
            cpu.ex = 0;
            b->set(0);
        } else {
            b->set(signedB % signedA);
        } 

        return CYCLES;
    }

    uint16_t and_opcode::execute() {
        b->set(b->get() & a->get());

        return CYCLES;
    }

    uint16_t bor_opcode::execute() {
        b->set(b->get() | a->get());

        return CYCLES;
    }

    uint16_t xor_opcode::execute() {
        b->set(b->get() ^ a->get());

        return CYCLES;
    }

    uint16_t shr_opcode::execute() {
        return CYCLES;
    }

    uint16_t asr_opcode::execute() {
        return CYCLES;
    }

    uint16_t shl_opcode::execute() {
        return CYCLES;
    }

    uint16_t ifb_opcode::execute() {
        return CYCLES;
    }

    uint16_t ifc_opcode::execute() {
        return CYCLES;
    }

    uint16_t ife_opcode::execute() {
        return CYCLES;
    }

    uint16_t ifn_opcode::execute() {
        return CYCLES;
    }

    uint16_t ifg_opcode::execute() {
        return CYCLES;
    }

    uint16_t ifa_opcode::execute() {
        return CYCLES;
    }

    uint16_t ifl_opcode::execute() {
        return CYCLES;
    }

    uint16_t ifu_opcode::execute() {
        return CYCLES;
    }

    uint16_t adx_opcode::execute() {
        return CYCLES;
    }

    uint16_t sbx_opcode::execute() {
        return CYCLES;
    }

    uint16_t sti_opcode::execute() {
        return CYCLES;
    }

    uint16_t std_opcode::execute() {
        return CYCLES;
    }

    uint16_t jsr_opcode::execute() {
        return CYCLES;
    }

    uint16_t hcf_opcode::execute() {
        return CYCLES;
    }

    uint16_t int_opcode::execute() {
        return CYCLES;
    }

    uint16_t iag_opcode::execute() {
        return CYCLES;
    }

    uint16_t ias_opcode::execute() {
        return CYCLES;
    }

    uint16_t rfi_opcode::execute() {
        return CYCLES;
    }

    uint16_t iaq_opcode::execute() {
        return CYCLES;
    }

    uint16_t hwn_opcode::execute() {
        return CYCLES;
    }

    uint16_t hwq_opcode::execute() {
        return CYCLES;
    }

    uint16_t hwi_opcode::execute() {
        return CYCLES;
    }
}}
