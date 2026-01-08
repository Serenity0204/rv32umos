#pragma once

#include <cstdint>
#include <iostream>
#include <unordered_map>

// riscv32i
using Byte = uint8_t;
using Half = uint16_t;
using Word = uint32_t;
using Addr = uint32_t;
using PageTable = std::unordered_map<Addr, Addr>;

// Memory
const Addr MEMORY_BASE = 0x00000000;
// 128MB
const Addr MEMORY_SIZE = 1024 * 1024 * 128;

// 1kb
const Addr PAGE_SIZE = 4096;

// 0x88000000
const Addr STACK_TOP = MEMORY_BASE + MEMORY_SIZE;

// 8MB below top
const Addr STACK_LIMIT = STACK_TOP - (8 * 1024 * 1024);