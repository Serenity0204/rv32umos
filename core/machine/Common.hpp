#pragma once

#include <cstdint>
#include <iostream>

// riscv32i
using Byte = uint8_t;
using Half = uint16_t;
using Word = uint32_t;
using Reg = uint32_t;
using Addr = uint32_t;

// Memory
const Addr MEMORY_BASE = 0x80000000;
// 128MB
const Addr MEMORY_SIZE = 1024 * 1024 * 128;