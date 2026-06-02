#pragma once

#include "Common.hpp"
#include "Device.hpp"
#include "MMU.hpp"
#include "RegFile.hpp"

class Executor;

enum class TrapType
{
    None,
    Syscall,
    PageFault
};

// value holds syscallID or fault address
struct Trap
{
    TrapType type = TrapType::None;
    Word value = 0;
};

class Machine : public Device
{
public:
    friend class Executor;
    Machine();
    ~Machine() = default;

    void step();
    void reset();
    void dumpRegisters();

    // PC
    inline Addr getPC() { return this->pc; }
    inline void advancePC() { this->pc += 4; }
    inline void setPC(Addr newPc) { this->pc = newPc; }

    // Trap handling
    inline void setTrap(TrapType type, Word value)
    {
        this->trap.type = type;
        this->trap.value = value;
    }
    inline bool hasTrap() const { return this->trap.type != TrapType::None; }
    inline Trap getTrap() const { return this->trap; }
    inline void clearTrap()
    {
        this->trap.type = TrapType::None;
        this->trap.value = 0;
    }

    // memory
    inline Word loadVirtualMemory(Addr vaddr, std::size_t size)
    {
        Word val = this->mmu.loadVirtualMemory(vaddr, size);
        if (this->mmu.hasFault())
        {
            this->setTrap(TrapType::PageFault, this->mmu.getFaultAddr());
            this->mmu.clearFault();
        }
        return val;
    }
    inline void storeVirtualMemory(Addr vaddr, std::size_t size, Word value)
    {
        this->mmu.storeVirtualMemory(vaddr, size, value);
        if (this->mmu.hasFault())
        {
            this->setTrap(TrapType::PageFault, this->mmu.getFaultAddr());
            this->mmu.clearFault();
        }
    }
    inline Word loadPhysicalMemory(Addr paddr, std::size_t size) { return this->mmu.loadPhysicalMemory(paddr, size); }
    inline void storePhysicalMemory(Addr paddr, std::size_t size, Word value) { this->mmu.storePhysicalMemory(paddr, size, value); }

    // reg
    inline Word readReg(std::size_t index) { return this->regs[index]; }
    inline void writeReg(std::size_t index, Word value) { this->regs.write(index, value); }
    inline RegFile& getRegs() { return this->regs; }

    // VM Control
    inline void setPageTable(PageTable* table) { this->mmu.setPageTable(table); }
    inline void enableVM(bool enabled) { this->mmu.enableVM(enabled); }
    inline void setMemory(Memory* memPtr) { this->mmu.setMemory(memPtr); }

    DeviceType getType() const override;

private:
    // kernel related
    RegFile regs;
    MMU mmu;
    Addr pc;
    Trap trap;

    Word fetch();
    void execute(Word instr);
};
