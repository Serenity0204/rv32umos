#pragma once
#include "Common.hpp"
#include "PTE.hpp"
#include "RegFile.hpp"
#include <string>
#include <vector>

// for ELF
struct Segment
{
    Addr vaddr;
    size_t memSize;
    size_t fileSize;
    size_t fileOffset;
    uint32_t flags;
};

class Process
{
public:
    enum class ProcessState : int
    {
        NEW,
        READY,
        RUNNING,
        TERMINATED
    };

private:
    struct PCB
    {
        // info
        int pid;
        std::string name;
        ProcessState state;

        // hardware context
        Addr pc;
        RegFile regs;
        PageTable* pageTable;
        // elf segments
        std::vector<Segment> segments;

        PCB(int id, std::string n) : pid(id), name(n), state(ProcessState::NEW), pc(MEMORY_BASE)
        {
            this->regs.reset();
            this->regs.write(2, STACK_TOP);
            this->pageTable = new PageTable();
        }
        ~PCB()
        {
            if (this->pageTable != nullptr)
            {
                delete this->pageTable;
                this->pageTable = nullptr;
            }
        }
    };

public:
    Process(int id, std::string name);
    ~Process();

    inline int getPid() const { return this->pcb->pid; }
    inline std::string getName() const { return this->pcb->name; }
    inline ProcessState getState() const { return this->pcb->state; }
    inline void setState(ProcessState newState) { this->pcb->state = newState; }
    inline Addr getPC() const { return this->pcb->pc; }
    inline void setPC(Addr newPC) { this->pcb->pc = newPC; }
    inline RegFile& getRegs() { return this->pcb->regs; }
    inline PageTable* getPageTable() { return this->pcb->pageTable; }
    inline std::vector<Segment>& getSegments() { return this->pcb->segments; }

private:
    PCB* pcb;
};