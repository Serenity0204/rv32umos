#include "Executor.hpp"

// Immediates
void Executor::execLUI(CPU& cpu, Word instr) {}
void Executor::execAUIPC(CPU& cpu, Word instr) {}

// I Type
void Executor::execADDI(CPU& cpu, Word instr) {}
void Executor::execSLTI(CPU& cpu, Word instr) {}
void Executor::execSLTIU(CPU& cpu, Word instr) {}
void Executor::execXORI(CPU& cpu, Word instr) {}
void Executor::execORI(CPU& cpu, Word instr) {}
void Executor::execANDI(CPU& cpu, Word instr) {}
void Executor::execSLLI(CPU& cpu, Word instr) {}
void Executor::execSRLI(CPU& cpu, Word instr) {}
void Executor::execSRAI(CPU& cpu, Word instr) {}

// R type
void Executor::execADD(CPU& cpu, Word instr) {}
void Executor::execSUB(CPU& cpu, Word instr) {}
void Executor::execSLL(CPU& cpu, Word instr) {}
void Executor::execSLT(CPU& cpu, Word instr) {}
void Executor::execSLTU(CPU& cpu, Word instr) {}
void Executor::execXOR(CPU& cpu, Word instr) {}
void Executor::execSRL(CPU& cpu, Word instr) {}
void Executor::execSRA(CPU& cpu, Word instr) {}
void Executor::execOR(CPU& cpu, Word instr) {}
void Executor::execAND(CPU& cpu, Word instr) {}

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

void Executor::execJAL(CPU& cpu, Word instr) {}
void Executor::execJALR(CPU& cpu, Word instr) {}

// Branch
void Executor::execBEQ(CPU& cpu, Word instr) {}
void Executor::execBNE(CPU& cpu, Word instr) {}
void Executor::execBLT(CPU& cpu, Word instr) {}
void Executor::execBGE(CPU& cpu, Word instr) {}
void Executor::execBLTU(CPU& cpu, Word instr) {}
void Executor::execBGEU(CPU& cpu, Word instr) {}