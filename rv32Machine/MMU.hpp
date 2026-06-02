#pragma once
#include "Common.hpp"
#include "Memory.hpp"
#include "PTE.hpp"

class MMU
{
public:
    MMU();
    ~MMU() = default;

    // virtual memory
    Word loadVirtualMemory(Addr vaddr, std::size_t size);
    void storeVirtualMemory(Addr vaddr, std::size_t size, Word value);

    // physical memory direct access
    Word loadPhysicalMemory(Addr paddr, std::size_t size);
    void storePhysicalMemory(Addr paddr, std::size_t size, Word value);

    // fetch instruction
    Word fetch(Addr vaddr);

    // Fault check API for Machine
    inline bool hasFault() const { return this->fault; }
    inline Addr getFaultAddr() const { return faultAddr; }
    inline void clearFault()
    {
        this->fault = false;
        this->faultAddr = 0;
    }

    // Control
    void setPageTable(PageTable* table);
    void enableVM(bool enabled);
    void reset();
    void setMemory(Memory* memory);

private:
    Memory* memory = nullptr;
    PageTable* currentTable;
    bool vmEnabled;

    // fault handling
    bool fault = false;
    Addr faultAddr = 0;

    Addr translate(Addr vaddr, bool isWrite, bool isExec);
};