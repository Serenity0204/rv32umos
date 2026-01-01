#include "Executor.hpp"
#include "Bus.hpp"
#include "CPU.hpp"
#include "Decoder.hpp"
#include "Exception.hpp"
#include "Utils.hpp"
#include <limits>

// Immediates
void Executor::execLUI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word imm = Decoder::immU(instr);
    cpu.regs.write(rd, imm);
}
void Executor::execAUIPC(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word imm = Decoder::immU(instr);
    Word currentPC = cpu.pc;
    cpu.regs.write(rd, currentPC + imm);
}

// I Type
void Executor::execADDI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Word val1 = cpu.regs[rs1];
    Word result = val1 + imm;
    cpu.regs.write(rd, result);
}

void Executor::execSLTI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);

    Word imm = Decoder::immI(instr);
    int32_t val1 = static_cast<int32_t>(cpu.regs[rs1]);
    int32_t imm_signed = static_cast<int32_t>(imm);

    Word result = (val1 < imm_signed) ? 1 : 0;
    cpu.regs.write(rd, result);
}

void Executor::execSLTIU(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);

    Word imm = Decoder::immI(instr);
    Word val1 = cpu.regs[rs1];

    Word result = (val1 < imm) ? 1 : 0;
    cpu.regs.write(rd, result);
}

void Executor::execXORI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Word val1 = cpu.regs[rs1];
    Word result = val1 ^ imm;
    cpu.regs.write(rd, result);
}

void Executor::execORI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Word val1 = cpu.regs[rs1];
    Word result = val1 | imm;
    cpu.regs.write(rd, result);
}

void Executor::execANDI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Word val1 = cpu.regs[rs1];
    Word result = val1 & imm;
    cpu.regs.write(rd, result);
}
void Executor::execSLLI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);

    Word val1 = cpu.regs[rs1];
    Word shamt = Decoder::shamtI(instr);
    Word result = val1 << shamt;
    cpu.regs.write(rd, result);
}
void Executor::execSRLI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);

    Word val1 = cpu.regs[rs1];
    Word shamt = Decoder::shamtI(instr);
    Word result = val1 >> shamt;
    cpu.regs.write(rd, result);
}
void Executor::execSRAI(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);

    int32_t val1 = static_cast<int32_t>(cpu.regs[rs1]);
    Word shamt = Decoder::shamtI(instr);

    Word result = static_cast<Word>(val1 >> shamt);
    cpu.regs.write(rd, result);
}

// R type
void Executor::execADD(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];
    Word result = val1 + val2;
    cpu.regs.write(rd, result);
}

void Executor::execSUB(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];
    Word result = val1 - val2;
    cpu.regs.write(rd, result);
}

void Executor::execSLL(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];
    Word shamt = Decoder::shamtR(val2);
    Word result = val1 << shamt;

    cpu.regs.write(rd, result);
}

void Executor::execSLT(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    int32_t val1 = static_cast<int32_t>(cpu.regs[rs1]);
    int32_t val2 = static_cast<int32_t>(cpu.regs[rs2]);

    Word result = (val1 < val2) ? 1 : 0;

    cpu.regs.write(rd, result);
}

void Executor::execSLTU(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];
    Word result = (val1 < val2) ? 1 : 0;

    cpu.regs.write(rd, result);
}

void Executor::execXOR(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];
    Word result = val1 ^ val2;
    cpu.regs.write(rd, result);
}
void Executor::execSRL(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];
    Word shamt = Decoder::shamtR(val2);
    Word result = val1 >> shamt;
    cpu.regs.write(rd, result);
}

void Executor::execSRA(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    int32_t val1 = static_cast<int32_t>(cpu.regs[rs1]);
    Word val2 = cpu.regs[rs2];
    Word shamt = Decoder::shamtR(val2);
    Word result = static_cast<Word>(val1 >> shamt);
    cpu.regs.write(rd, result);
}

void Executor::execOR(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];
    Word result = val1 | val2;
    cpu.regs.write(rd, result);
}
void Executor::execAND(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];
    Word result = val1 & val2;
    cpu.regs.write(rd, result);
}

// // Others
// void Executor::execFENCE(CPU& cpu, Word instr) {}
// void Executor::execFENCEI(CPU& cpu, Word instr) {}

// // CSR
// void Executor::execCSRRW(CPU& cpu, Word instr) {}
// void Executor::execCSRRS(CPU& cpu, Word instr) {}
// void Executor::execCSRRC(CPU& cpu, Word instr) {}
// void Executor::execCSRRWI(CPU& cpu, Word instr) {}
// void Executor::execCSRRSI(CPU& cpu, Word instr) {}
// void Executor::execCSRRCI(CPU& cpu, Word instr) {}

// // --- System ---
void Executor::execECALL(CPU& cpu)
{
    Word syscallID = cpu.regs[17];

    switch (syscallID)
    {
    case 0:
        throw SyscallException("exit syscall");
    default:
        throw SyscallException("Unknown syscall ID: " + std::to_string(syscallID));
    }
}
// void Executor::execEBREAK(CPU& cpu, Word instr) {}
// void Executor::execSRET(CPU& cpu, Word instr) {}
// void Executor::execURET(CPU& cpu, Word instr) {}
// void Executor::execMRET(CPU& cpu, Word instr) {}
// void Executor::execWFI(CPU& cpu, Word instr) {}
// void Executor::execSFENCEVMA(CPU& cpu, Word instr) {}

// Load
void Executor::execLB(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Addr addr = cpu.regs[rs1] + imm;

    Word val = cpu.bus.load(addr, 1);

    int32_t signedVal = static_cast<int32_t>(static_cast<int8_t>(val));
    cpu.regs.write(rd, signedVal);
}
void Executor::execLH(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Addr addr = cpu.regs[rs1] + imm;

    Word val = cpu.bus.load(addr, 2);
    int32_t signedVal = static_cast<int32_t>(static_cast<int16_t>(val));
    cpu.regs.write(rd, signedVal);
}

void Executor::execLW(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Addr addr = cpu.regs[rs1] + imm;

    Word val = cpu.bus.load(addr, 4);
    cpu.regs.write(rd, val);
}
void Executor::execLBU(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Addr addr = cpu.regs[rs1] + imm;

    Word val = cpu.bus.load(addr, 1);
    cpu.regs.write(rd, val & 0xFF);
}
void Executor::execLHU(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word imm = Decoder::immI(instr);
    Addr addr = cpu.regs[rs1] + imm;

    Word val = cpu.bus.load(addr, 2);
    cpu.regs.write(rd, val & 0xFFFF);
}

// Store
void Executor::execSB(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immS(instr);

    Addr addr = cpu.regs[rs1] + imm;
    Word val = cpu.regs[rs2];
    cpu.bus.store(addr, 1, val);
}
void Executor::execSH(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immS(instr);

    Addr addr = cpu.regs[rs1] + imm;
    Word val = cpu.regs[rs2];
    cpu.bus.store(addr, 2, val);
}
void Executor::execSW(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immS(instr);

    Addr addr = cpu.regs[rs1] + imm;
    Word val = cpu.regs[rs2];
    cpu.bus.store(addr, 4, val);
}

// Jump
void Executor::execJAL(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    Word imm = Decoder::immJ(instr);

    Addr returnAddrPC = cpu.pc + 4;
    Addr currentPC = cpu.pc;

    if (rd != 0) cpu.regs.write(rd, returnAddrPC);

    Addr nextPC = currentPC + imm - 4;
    cpu.pc = nextPC;
}

void Executor::execJALR(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    Word rs1 = Decoder::rs1(instr);
    // JALR is I type
    Word imm = Decoder::immI(instr);
    Word val1 = cpu.regs[rs1];

    Addr returnAddrPC = cpu.pc + 4;

    if (rd != 0) cpu.regs.write(rd, returnAddrPC);
    Addr nextPC = ((val1 + imm) & ~1) - 4;
    cpu.pc = nextPC;
}

// Branch
void Executor::execBEQ(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immB(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];

    // current PC already + 4, need to - 4 back
    if (val1 == val2)
    {
        Addr nextPC = (cpu.pc - 4) + imm;
        cpu.pc = nextPC;
    }
}
void Executor::execBNE(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immB(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];

    // current PC already + 4, need to - 4 back
    if (val1 != val2)
    {
        Addr nextPC = (cpu.pc - 4) + imm;
        cpu.pc = nextPC;
    }
}
void Executor::execBLT(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immB(instr);

    int32_t val1 = static_cast<int32_t>(cpu.regs[rs1]);
    int32_t val2 = static_cast<int32_t>(cpu.regs[rs2]);

    // current PC already + 4, need to - 4 back
    if (val1 < val2)
    {
        Addr nextPC = (cpu.pc - 4) + imm;
        cpu.pc = nextPC;
    }
}
void Executor::execBGE(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immB(instr);

    int32_t val1 = static_cast<int32_t>(cpu.regs[rs1]);
    int32_t val2 = static_cast<int32_t>(cpu.regs[rs2]);

    // current PC already + 4, need to - 4 back
    if (val1 >= val2)
    {
        Addr nextPC = (cpu.pc - 4) + imm;
        cpu.pc = nextPC;
    }
}
void Executor::execBLTU(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immB(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];

    // current PC already + 4, need to - 4 back
    if (val1 < val2)
    {
        Addr nextPC = (cpu.pc - 4) + imm;
        cpu.pc = nextPC;
    }
}
void Executor::execBGEU(CPU& cpu, Word instr)
{
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);
    Word imm = Decoder::immB(instr);
    Word val1 = cpu.regs[rs1];
    Word val2 = cpu.regs[rs2];

    // current PC already + 4, need to - 4 back
    if (val1 >= val2)
    {
        Addr nextPC = (cpu.pc - 4) + imm;
        cpu.pc = nextPC;
    }
}

// M Extension
void Executor::execMUL(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    Word result = cpu.regs[rs1] * cpu.regs[rs2];
    cpu.regs.write(rd, result);
}

void Executor::execMULH(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    int64_t op1 = static_cast<int32_t>(cpu.regs[rs1]);
    int64_t op2 = static_cast<int32_t>(cpu.regs[rs2]);
    int64_t result = op1 * op2;

    cpu.regs.write(rd, static_cast<Word>(result >> 32));
}

void Executor::execMULHSU(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    int64_t op1 = static_cast<int32_t>(cpu.regs[rs1]);
    uint64_t op2 = static_cast<Word>(cpu.regs[rs2]);

    int64_t result = op1 * static_cast<int64_t>(op2);
    cpu.regs.write(rd, static_cast<Word>(result >> 32));
}

void Executor::execMULHU(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    uint64_t op1 = static_cast<Word>(cpu.regs[rs1]);
    uint64_t op2 = static_cast<Word>(cpu.regs[rs2]);
    uint64_t result = op1 * op2;

    cpu.regs.write(rd, static_cast<Word>(result >> 32));
}

void Executor::execDIV(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    int32_t dividend = static_cast<int32_t>(cpu.regs[rs1]);
    int32_t divisor = static_cast<int32_t>(cpu.regs[rs2]);

    // Corner Case 1: Division by Zero
    if (divisor == 0)
    {
        cpu.regs.write(rd, -1);
        return;
    }

    // Corner Case 2: Signed Overflow (INT_MIN / -1)
    if (dividend == std::numeric_limits<int32_t>::min() && divisor == -1)
    {
        cpu.regs.write(rd, static_cast<Word>(dividend));
        return;
    }

    cpu.regs.write(rd, static_cast<Word>(dividend / divisor));
}

void Executor::execDIVU(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    Word dividend = cpu.regs[rs1];
    Word divisor = cpu.regs[rs2];

    // Corner Case: Division by Zero
    if (divisor == 0)
    {
        cpu.regs.write(rd, 0xFFFFFFFF);
        return;
    }

    cpu.regs.write(rd, dividend / divisor);
}

void Executor::execREM(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    int32_t dividend = static_cast<int32_t>(cpu.regs[rs1]);
    int32_t divisor = static_cast<int32_t>(cpu.regs[rs2]);

    // Corner Case 1: Division by Zero
    if (divisor == 0)
    {
        cpu.regs.write(rd, static_cast<Word>(dividend));
        return;
    }

    // Corner Case 2: Signed Overflow (INT_MIN / -1)
    if (dividend == std::numeric_limits<int32_t>::min() && divisor == -1)
    {
        cpu.regs.write(rd, 0);
        return;
    }

    cpu.regs.write(rd, static_cast<Word>(dividend % divisor));
}

void Executor::execREMU(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    if (rd == 0) return;
    Word rs1 = Decoder::rs1(instr);
    Word rs2 = Decoder::rs2(instr);

    Word dividend = cpu.regs[rs1];
    Word divisor = cpu.regs[rs2];

    // Corner Case: Division by Zero
    if (divisor == 0)
    {
        cpu.regs.write(rd, dividend);
        return;
    }

    cpu.regs.write(rd, dividend % divisor);
}