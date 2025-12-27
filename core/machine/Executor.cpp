#include "Executor.hpp"

// Immediates
void Executor::execLUI(CPU& cpu, Word instr) {}
void Executor::execAUIPC(CPU& cpu, Word instr) {}

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

// Others
void Executor::execFENCE(CPU& cpu, Word instr) {}
void Executor::execFENCEI(CPU& cpu, Word instr) {}

// CSR
void Executor::execCSRRW(CPU& cpu, Word instr) {}
void Executor::execCSRRS(CPU& cpu, Word instr) {}
void Executor::execCSRRC(CPU& cpu, Word instr) {}
void Executor::execCSRRWI(CPU& cpu, Word instr) {}
void Executor::execCSRRSI(CPU& cpu, Word instr) {}
void Executor::execCSRRCI(CPU& cpu, Word instr) {}

// --- System ---
void Executor::execECALL(CPU& cpu, Word instr) {}
void Executor::execEBREAK(CPU& cpu, Word instr) {}
void Executor::execSRET(CPU& cpu, Word instr) {}
void Executor::execURET(CPU& cpu, Word instr) {}
void Executor::execMRET(CPU& cpu, Word instr) {}
void Executor::execWFI(CPU& cpu, Word instr) {}
void Executor::execSFENCEVMA(CPU& cpu, Word instr) {}

// Load
void Executor::execLB(CPU& cpu, Word instr) {}
void Executor::execLH(CPU& cpu, Word instr) {}
void Executor::execLW(CPU& cpu, Word instr) {}
void Executor::execLBU(CPU& cpu, Word instr) {}
void Executor::execLHU(CPU& cpu, Word instr) {}

// Store
void Executor::execSB(CPU& cpu, Word instr) {}
void Executor::execSH(CPU& cpu, Word instr) {}
void Executor::execSW(CPU& cpu, Word instr) {}

// Jump
void Executor::execJAL(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    Word imm = Decoder::immJ(instr);

    // already pc + 4
    Addr returnAddrPC = cpu.pc;
    Addr currentPC = cpu.pc - 4;

    if (rd != 0) cpu.regs.write(rd, returnAddrPC);

    Addr nextPC = currentPC + imm;
    cpu.pc = nextPC;
}

void Executor::execJALR(CPU& cpu, Word instr)
{
    Word rd = Decoder::rd(instr);
    Word rs1 = Decoder::rs1(instr);
    // JALR is I type
    Word imm = Decoder::immI(instr);
    Word val1 = cpu.regs[rs1];

    // already pc + 4
    Addr returnAddrPC = cpu.pc;

    if (rd != 0) cpu.regs.write(rd, returnAddrPC);
    Addr nextPC = (val1 + imm) & ~1;
    cpu.pc = nextPC;
}

// Branch
void Executor::execBEQ(CPU& cpu, Word instr) {}
void Executor::execBNE(CPU& cpu, Word instr) {}
void Executor::execBLT(CPU& cpu, Word instr) {}
void Executor::execBGE(CPU& cpu, Word instr) {}
void Executor::execBLTU(CPU& cpu, Word instr) {}
void Executor::execBGEU(CPU& cpu, Word instr) {}