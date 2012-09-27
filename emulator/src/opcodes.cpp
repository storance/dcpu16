#include <stdexcept>
#include <boost/format.hpp>

#include "opcodes.hpp"

using namespace std;
using boost::format;
using boost::str;

#define BASIC_OPCODE_CASE(o) case o ## Opcode ::OPCODE: \
    return unique_ptr<Opcode>(new o ## Opcode (cpu, argA, argB));

#define SPECIAL_OPCODE_CASE(o) case o ## Opcode ::OPCODE: \
    return unique_ptr<Opcode>(new o ## Opcode (cpu, argA));


namespace dcpu { namespace emulator {
    OpcodePtr Opcode::parse(Dcpu &cpu, uint16_t instruction) {
        if ((instruction & 0x1f) != 0) {
            return move(parseBasic(cpu, instruction));
        } else {
            return move(parseSpecial(cpu, instruction));
        }
    }

    OpcodePtr Opcode::parseBasic(Dcpu &cpu, uint16_t instruction) {
        uint8_t o = instruction & 0x1f;
        uint8_t a = (instruction >> 10) & 0x3f;
        uint8_t b = (instruction >> 5) & 0x1f;

        auto argA = Argument::parse(cpu, a, true);
        auto argB = Argument::parse(cpu, b, false);

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
            throw invalid_argument(::str(format("Invalid basic opcode: %02x") % (uint16_t)o));
            break;
        }
    }

    OpcodePtr Opcode::parseSpecial(Dcpu &cpu, uint16_t instruction) {
        uint8_t o = (instruction >> 5) & 0x1f;
        uint8_t a = (instruction >> 10) & 0x3f;

        auto argA = Argument::parse(cpu, a, true);
        
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
            throw invalid_argument(::str(format("Invalid special opcode: %02x") % (uint16_t)o));
            break;
        }
    }

    Opcode::Opcode(Dcpu &cpu, ArgumentPtr &a, ArgumentPtr &b, uint16_t cycles, bool conditional,
            const string &name) : cpu(cpu), a(move(a)), b(move(b)), baseCycles(cycles), conditional(conditional),
            name(name) {

    }

    Opcode::Opcode(Dcpu &cpu, ArgumentPtr &a, uint16_t cycles, bool conditional, const string &name) 
            : cpu(cpu), a(move(a)), b(), baseCycles(cycles), conditional(conditional), name(name) {

    }

    Opcode::~Opcode() {

    }

    uint16_t Opcode::calculateCycles() const {
        return baseCycles + a->getCycles() + (b ? b->getCycles() : 0);
    }

    string Opcode::getName() const {
        return name;
    }
    
    bool Opcode::isConditional() const {
        return conditional;
    }

    string Opcode::str() const {
        if (b) {
            return ::str(format("%s %s, %s") % name % b->str() % a->str());
        } else {
            return ::str(format("%s %s") % name % a->str());
        }
    }

    uint16_t setOpcode::execute() {
        b->set(a->get());

        return calculateCycles();
    }

    uint16_t addOpcode::execute() {
        uint32_t result = a->get() + b->get();
        b->set(result);

        cpu.registers.ex = result >> 16;

        return calculateCycles();
    }

    uint16_t subOpcode::execute() {
        uint32_t result = b->get() - a->get();
        b->set(result);

        cpu.registers.ex = result >> 16;

        return calculateCycles();
    }

    uint16_t mulOpcode::execute() {
        uint32_t result = b->get() * a->get();
        b->set(result);

        cpu.registers.ex = (result >> 16) & 0xffff;

        return calculateCycles();
    }

    uint16_t mliOpcode::execute() {
        int16_t signedA = a->get();
        int16_t signedB = b->get();

        int32_t result = signedA * signedB;
        b->set(result);

        cpu.registers.ex = (result >> 16) & 0xffff;

        return calculateCycles();
    }

    uint16_t divOpcode::execute() {
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

        return calculateCycles();
    }

    uint16_t dviOpcode::execute() {
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

        return calculateCycles();
    }

    uint16_t modOpcode::execute() {
        if (a->get() == 0) {
            cpu.registers.ex = 0;
            b->set(0);
        } else {
            b->set(b->get() % a->get());
        }

        return calculateCycles();
    }

    uint16_t mdiOpcode::execute() {
        int16_t signedA = (int16_t)a->get();
        int16_t signedB = (int16_t)b->get();

        if (signedA == 0) {
            cpu.registers.ex = 0;
            b->set(0);
        } else {
            b->set(signedB % signedA);
        }

        return calculateCycles();
    }

    uint16_t andOpcode::execute() {
        b->set(b->get() & a->get());

        return calculateCycles();
    }

    uint16_t borOpcode::execute() {
        b->set(b->get() | a->get());

        return calculateCycles();
    }

    uint16_t xorOpcode::execute() {
        b->set(b->get() ^ a->get());

        return calculateCycles();
    }

    uint16_t shrOpcode::execute() {
        uint16_t unsignedA = a->get();
        uint16_t unsignedB = b->get();

        b->set(unsignedB >> unsignedA);
        cpu.registers.ex = ((unsignedB << 16) >> unsignedA) & 0xffff;

        return calculateCycles();
    }

    uint16_t asrOpcode::execute() {
        uint16_t unsignedA = a->get();
        int16_t signedB = b->get();

        b->set(signedB >> unsignedA);
        cpu.registers.ex = ((signedB << 16) >> unsignedA) & 0xffff;

        return calculateCycles();
    }

    uint16_t shlOpcode::execute() {
        uint16_t unsignedA = a->get();
        uint16_t unsignedB = b->get();

        uint32_t result = unsignedB << unsignedA;
        b->set(result);
        cpu.registers.ex = (result >> 16) & 0xffff;

        return calculateCycles();
    }

    uint16_t ifbOpcode::execute() {
        if ((b->get() & a->get()) == 0) {
            cpu.skipNextInstruction();
        }

        return calculateCycles();
    }

    uint16_t ifcOpcode::execute() {
        if ((b->get() & a->get()) != 0) {
            cpu.skipNextInstruction();
        }

        return calculateCycles();
    }

    uint16_t ifeOpcode::execute() {
        if (b->get() != a->get()) {
            cpu.skipNextInstruction();
        }

        return calculateCycles();
    }

    uint16_t ifnOpcode::execute() {
        if (b->get() == a->get()) {
            cpu.skipNextInstruction();
        }

        return calculateCycles();
    }

    uint16_t ifgOpcode::execute() {
        if (b->get() <= a->get()) {
            cpu.skipNextInstruction();
        }

        return calculateCycles();
    }

    uint16_t ifaOpcode::execute() {
        int16_t signedB = b->get();
        int16_t signedA = a->get();

        if (signedB <= signedA) {
            cpu.skipNextInstruction();
        }

        return calculateCycles();
    }

    uint16_t iflOpcode::execute() {
        if (b->get() >= a->get()) {
            cpu.skipNextInstruction();
        }

        return calculateCycles();
    }

    uint16_t ifuOpcode::execute() {
        int16_t signedB = b->get();
        int16_t signedA = a->get();

        if (signedB >= signedA) {
            cpu.skipNextInstruction();
        }

        return calculateCycles();
    }

    uint16_t adxOpcode::execute() {
        uint32_t result = b->get() + a->get() + cpu.registers.ex;

        b->set(result);
        cpu.registers.ex = result >> 16;

        return calculateCycles();
    }

    uint16_t sbxOpcode::execute() {
        uint32_t result = b->get() - a->get() + cpu.registers.ex;
        b->set(result);

        cpu.registers.ex = result >> 16;

        return calculateCycles();
    }

    uint16_t stiOpcode::execute() {
        b->set(a->get());

        ++cpu.registers.i;
        ++cpu.registers.j;

        return calculateCycles();
    }

    uint16_t stdOpcode::execute() {
        b->set(a->get());

        --cpu.registers.i;
        --cpu.registers.j;

        return calculateCycles();
    }

    uint16_t jsrOpcode::execute() {
        cpu.stack.push(cpu.registers.pc);
        cpu.registers.pc = a->get();

        return calculateCycles();
    }

    uint16_t hcfOpcode::execute() {
        cpu.catchFire();

        return calculateCycles();
    }

    uint16_t intOpcode::execute() {
        cpu.interrupts.send(a->get());

        return calculateCycles();
    }

    uint16_t iagOpcode::execute() {
        a->set(cpu.registers.ia);

        return calculateCycles();
    }

    uint16_t iasOpcode::execute() {
        cpu.registers.ia = a->get();

        return calculateCycles();
    }

    uint16_t rfiOpcode::execute() {
        cpu.interrupts.disableQueue();
        cpu.registers.a = cpu.stack.pop();
        cpu.registers.pc = cpu.stack.pop();

        return calculateCycles();
    }

    uint16_t iaqOpcode::execute() {
        if (a->get() != 0) {
            cpu.interrupts.enableQueue();
        } else {
            cpu.interrupts.disableQueue();
        }

        return calculateCycles();
    }

    uint16_t hwnOpcode::execute() {
        a->set(cpu.hardwareManager.getCount());

        return calculateCycles();
    }

    uint16_t hwqOpcode::execute() {
        cpu.hardwareManager.query(a->get());

        return calculateCycles();
    }

    uint16_t hwiOpcode::execute() {
        uint16_t extraCycles = cpu.hardwareManager.interrupt(a->get());

        return calculateCycles() + extraCycles;
    }
}}
