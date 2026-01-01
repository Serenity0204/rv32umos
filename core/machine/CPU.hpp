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
    inline Word readReg(size_t index) { return this->regs[index]; }

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
