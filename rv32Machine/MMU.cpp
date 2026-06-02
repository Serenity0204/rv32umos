#include "MMU.hpp"
#include "Logger.hpp"
#include "Utils.hpp"

MMU::MMU()
{
    this->reset();
}

void MMU::setMemory(Memory* memory)
{
    this->memory = memory;
}

void MMU::reset()
{
    this->vmEnabled = false;
    this->currentTable = nullptr;
}

void MMU::setPageTable(PageTable* table)
{
    this->currentTable = table;
}

void MMU::enableVM(bool enabled)
{
    this->vmEnabled = enabled;
}

Word MMU::loadVirtualMemory(Addr vaddr, std::size_t size)
{
    Addr paddr = this->translate(vaddr, false, false);
    if (this->fault) return 0;
    return this->memory->load(paddr, size);
}

void MMU::storeVirtualMemory(Addr vaddr, std::size_t size, Word value)
{
    Addr paddr = this->translate(vaddr, true, false);
    if (this->fault) return;
    this->memory->store(paddr, size, value);
}

Word MMU::loadPhysicalMemory(Addr paddr, std::size_t size)
{
    return this->memory->load(paddr, size);
}

void MMU::storePhysicalMemory(Addr paddr, std::size_t size, Word value)
{
    this->memory->store(paddr, size, value);
}

Word MMU::fetch(Addr vaddr)
{
    // Fetch is always 4 bytes
    Addr paddr = this->translate(vaddr, false, true);
    if (this->fault) return 0;
    return this->memory->load(paddr, 4);
}

Addr MMU::translate(Addr vaddr, bool isWrite, bool isExec)
{
    if (!this->vmEnabled) return vaddr;

    uint32_t vpn = (vaddr >> 12);
    uint32_t offset = (vaddr & 0xFFF);

    if (this->currentTable == nullptr || this->currentTable->count(vpn) == 0)
    {
        this->fault = true;
        this->faultAddr = vaddr;
        return 0;
    }

    PTE& pte = this->currentTable->at(vpn);

    if (!pte.valid)
    {
        this->fault = true;
        this->faultAddr = vaddr;
        return 0;
    }

    if (isExec && !pte.canExecute)
    {
        LOG(MMU, ERROR, "Execution Violation at " + Utils::toHex(vaddr));
        this->fault = true;
        this->faultAddr = vaddr;
        return 0;
    }
    if (isWrite && !pte.canWrite)
    {
        LOG(MMU, ERROR, "Write Violation at " + Utils::toHex(vaddr));
        this->fault = true;
        this->faultAddr = vaddr;
        return 0;
    }
    if (!isWrite && !isExec && !pte.canRead)
    {
        LOG(MMU, ERROR, "Read Violation at " + Utils::toHex(vaddr));
        this->fault = true;
        this->faultAddr = vaddr;
        return 0;
    }

    pte.referenced = true;
    if (isWrite) pte.dirty = true;

    return (pte.ppn << 12) | offset;
}