#include "RegFile.hpp"
#include <assert.h>

RegFile::RegFile()
{
    this->reset();
}

void RegFile::reset()
{
    this->regs.fill(0);
}

Word RegFile::read(std::size_t index) const
{
    assert(index < 32);
    return this->regs[index];
}

void RegFile::write(std::size_t index, Word value)
{
    assert(index < 32);

    // RISC-V: x0 is hard-wired to zero
    if (index == 0) return;
    this->regs[index] = value;
}

Word RegFile::operator[](std::size_t index) const
{
    return this->read(index);
}
