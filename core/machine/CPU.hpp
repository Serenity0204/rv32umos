#pragma once
#include "Bus.hpp"
#include "Common.hpp"
#include "Decoder.hpp"
#include "Executor.hpp"
#include "RegFile.hpp"
#include "Utils.hpp"

class CPU
{
public:
    friend class Executor;
    CPU(Bus& bus_ref);
    ~CPU() = default;

    void step();
    void reset();
    void dumpRegisters();

private:
    Bus& bus;
    RegFile regs;
    Addr pc;

    Word fetch();
    void execute(Word instr);
};
