#include "PhysicalMemoryManager.hpp"

void PhysicalMemoryManager::init()
{
    for (Addr frame = MEMORY_BASE; frame < MEMORY_BASE + MEMORY_SIZE; frame += PAGE_SIZE) this->freeFrames.push_back(frame);
}

Addr PhysicalMemoryManager::allocateFrame()
{
    // no swap yet, throw error
    if (this->freeFrames.empty()) throw std::runtime_error("PANIC: Out of Physical Memory!");

    Addr frame = this->freeFrames.back();
    this->freeFrames.pop_back();
    return frame;
}
void PhysicalMemoryManager::freeFrame(Addr frame)
{
    this->freeFrames.push_back(frame);
}