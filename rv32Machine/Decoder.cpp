#include "Decoder.hpp"
#include "Utils.hpp"

Opcode Decoder::opcode(Word instr)
{
    return static_cast<Opcode>(Utils::extractBits(instr, 0, 6));
}

uint32_t Decoder::rd(Word instr)
{
    return Utils::extractBits(instr, 7, 11);
}

Func3 Decoder::func3(Word instr)
{
    return static_cast<Func3>(Utils::extractBits(instr, 12, 14));
}

uint32_t Decoder::rs1(Word instr)
{
    return Utils::extractBits(instr, 15, 19);
}

uint32_t Decoder::rs2(Word instr)
{
    return Utils::extractBits(instr, 20, 24);
}

Func7 Decoder::func7(Word instr)
{
    return static_cast<Func7>(Utils::extractBits(instr, 25, 31));
}

uint32_t Decoder::immI(Word instr)
{
    uint32_t imm = Utils::extractBits(instr, 20, 31);
    return Utils::signExtend(imm, 12);
}

uint32_t Decoder::immS(Word instr)
{
    uint32_t imm_11_5 = Utils::extractBits(instr, 25, 31);
    uint32_t imm_4_0 = Utils::extractBits(instr, 7, 11);

    uint32_t imm = (imm_11_5 << 5) | imm_4_0;
    return Utils::signExtend(imm, 12);
}

uint32_t Decoder::immB(Word instr)
{
    uint32_t imm12 = Utils::extractBits(instr, 31, 31);
    uint32_t imm10_5 = Utils::extractBits(instr, 25, 30);
    uint32_t imm4_1 = Utils::extractBits(instr, 8, 11);
    uint32_t imm11 = Utils::extractBits(instr, 7, 7);

    uint32_t imm = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);

    return Utils::signExtend(imm, 13);
}

uint32_t Decoder::immU(Word instr)
{
    return Utils::extractBits(instr, 12, 31) << 12;
}

uint32_t Decoder::immJ(Word instr)
{
    uint32_t imm20 = Utils::extractBits(instr, 31, 31);
    uint32_t imm10_1 = Utils::extractBits(instr, 21, 30);
    uint32_t imm11 = Utils::extractBits(instr, 20, 20);
    uint32_t imm19_12 = Utils::extractBits(instr, 12, 19);

    uint32_t imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);

    return Utils::signExtend(imm, 21);
}

uint32_t Decoder::shamtI(Word instr)
{
    return Utils::extractBits(instr, 20, 24);
}

uint32_t Decoder::shamtR(Word val)
{
    return Utils::extractBits(val, 0, 4);
}