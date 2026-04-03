#pragma once
#include "Common.hpp"

enum class Opcode : uint8_t
{
    R_Type = 0x33,
    I_Type = 0x13,
    B_Type = 0x63,
    STORE = 0x23,
    LOAD = 0x03,
    AUIPC = 0x17,
    LUI = 0x37,
    JALR = 0x67,
    JAL = 0x6F,
    SYSTEM = 0x73,
    MISC_MEM = 0x0F,
};

enum class Func3 : uint8_t
{
    ADD_SUB = 0x0,
    SLL = 0x1,
    SLT = 0x2,
    SLTU = 0x3,
    XOR = 0x4,
    SRL_SRA = 0x5,
    OR = 0x6,
    AND = 0x7,

    BEQ = 0x0,
    BNE = 0x1,
    BLT = 0x4,
    BGE = 0x5,
    BLTU = 0x6,
    BGEU = 0x7,

    LB = 0x0,
    LH = 0x1,
    LW = 0x2,
    LBU = 0x4,
    LHU = 0x5,

    SB = 0x0,
    SH = 0x1,
    SW = 0x2,

    JALR = 0x0,

    ECALL_EBREAK = 0x0,
    CSRRW = 0x1,
    CSRRS = 0x2,
    CSRRC = 0x3,
    CSRRWI = 0x5,
    CSRRSI = 0x6,
    CSRRCI = 0x7,

    FENCE = 0x0,
    FENCE_I = 0x1,

    // M Extension
    MUL = 0x0,
    MULH = 0x1,
    MULHSU = 0x2,
    MULHU = 0x3,
    DIV = 0x4,
    DIVU = 0x5,
    REM = 0x6,
    REMU = 0x7,
};

enum class Func7 : uint8_t
{
    ADD_SRL = 0x00,
    SUB_SRA = 0x20,
    M_TYPE = 0x01,
};

class Decoder
{
public:
    Decoder() = delete;
    ~Decoder() = delete;
    Decoder(const Decoder& other) = delete;
    Decoder& operator=(const Decoder& other) = delete;

    static Opcode opcode(Word instr);
    static uint32_t rd(Word instr);
    static uint32_t rs1(Word instr);
    static uint32_t rs2(Word instr);
    static Func3 func3(Word instr);
    static Func7 func7(Word instr);
    static uint32_t immI(Word instr);
    static uint32_t immS(Word instr);
    static uint32_t immB(Word instr);
    static uint32_t immU(Word instr);
    static uint32_t immJ(Word instr);
    static uint32_t shamtI(Word instr);
    static uint32_t shamtR(Word val);
};