#pragma once

#include "Common.hpp"
#include "RegFile.hpp"

class Bus;
class Executor;

class CPU
{
public:
    friend class Executor;
    CPU(Bus& bus_ref);
    ~CPU() = default;

    void step();
    void reset();
    void dumpRegisters();

    // PC
    inline Addr getPC() { return this->pc; }
    inline void advancePC() { this->pc += 4; }

    // reg
    inline Word readReg(std::size_t index) { return this->regs[index]; }
    inline void writeReg(std::size_t index, Word value) { this->regs.write(index, value); }
    // memory
    Word load(Addr addr, std::size_t size);

    // status
    inline bool isHalted() { return this->halted; }
    inline void halt() { this->halted = true; }

private:
    bool halted;
    Bus& bus;
    RegFile regs;
    Addr pc;

    Word fetch();
    void execute(Word instr);
};
