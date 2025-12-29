#pragma once
#include "CPU.hpp"
#include "Common.hpp"
#include "Decoder.hpp"
#include "Utils.hpp"
#include <limits>

class CPU;

class Executor
{
public:
    Executor() = delete;
    ~Executor() = delete;
    Executor(const Executor&) = delete;
    Executor& operator=(const Executor&) = delete;

    // Immediates
    static void execLUI(CPU& cpu, Word instr);
    static void execAUIPC(CPU& cpu, Word instr);

    // I type
    static void execADDI(CPU& cpu, Word instr);
    static void execSLTI(CPU& cpu, Word instr);
    static void execSLTIU(CPU& cpu, Word instr);
    static void execXORI(CPU& cpu, Word instr);
    static void execORI(CPU& cpu, Word instr);
    static void execANDI(CPU& cpu, Word instr);
    static void execSLLI(CPU& cpu, Word instr);
    static void execSRLI(CPU& cpu, Word instr);
    static void execSRAI(CPU& cpu, Word instr);

    // R type
    static void execADD(CPU& cpu, Word instr);
    static void execSUB(CPU& cpu, Word instr);
    static void execSLL(CPU& cpu, Word instr);
    static void execSLT(CPU& cpu, Word instr);
    static void execSLTU(CPU& cpu, Word instr);
    static void execXOR(CPU& cpu, Word instr);
    static void execSRL(CPU& cpu, Word instr);
    static void execSRA(CPU& cpu, Word instr);
    static void execOR(CPU& cpu, Word instr);
    static void execAND(CPU& cpu, Word instr);

    // Others
    static void execFENCE(CPU& cpu, Word instr);
    static void execFENCEI(CPU& cpu, Word instr);

    // // CSR
    // static void execCSRRW(CPU& cpu, Word instr);
    // static void execCSRRS(CPU& cpu, Word instr);
    // static void execCSRRC(CPU& cpu, Word instr);
    // static void execCSRRWI(CPU& cpu, Word instr);
    // static void execCSRRSI(CPU& cpu, Word instr);
    // static void execCSRRCI(CPU& cpu, Word instr);

    // // System
    // static void execECALL(CPU& cpu, Word instr);
    // static void execEBREAK(CPU& cpu, Word instr);
    // static void execSRET(CPU& cpu, Word instr);
    // static void execURET(CPU& cpu, Word instr);
    // static void execMRET(CPU& cpu, Word instr);
    // static void execWFI(CPU& cpu, Word instr);
    // static void execSFENCEVMA(CPU& cpu, Word instr);

    // Load
    static void execLB(CPU& cpu, Word instr);
    static void execLH(CPU& cpu, Word instr);
    static void execLW(CPU& cpu, Word instr);
    static void execLBU(CPU& cpu, Word instr);
    static void execLHU(CPU& cpu, Word instr);

    // Store
    static void execSB(CPU& cpu, Word instr);
    static void execSH(CPU& cpu, Word instr);
    static void execSW(CPU& cpu, Word instr);

    // Jump
    static void execJAL(CPU& cpu, Word instr);
    static void execJALR(CPU& cpu, Word instr);

    // Branch
    static void execBEQ(CPU& cpu, Word instr);
    static void execBNE(CPU& cpu, Word instr);
    static void execBLT(CPU& cpu, Word instr);
    static void execBGE(CPU& cpu, Word instr);
    static void execBLTU(CPU& cpu, Word instr);
    static void execBGEU(CPU& cpu, Word instr);

    // M Extension
    static void execMUL(CPU& cpu, Word instr);
    static void execMULH(CPU& cpu, Word instr);
    static void execMULHSU(CPU& cpu, Word instr);
    static void execMULHU(CPU& cpu, Word instr);
    static void execDIV(CPU& cpu, Word instr);
    static void execDIVU(CPU& cpu, Word instr);
    static void execREM(CPU& cpu, Word instr);
    static void execREMU(CPU& cpu, Word instr);
};
