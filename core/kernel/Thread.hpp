#pragma once

#include "Common.hpp"
#include "RegFile.hpp"
#include <string>
#include <vector>

class Process;

enum class ThreadState
{
    NEW,
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

class Thread
{
private:
    struct TCB
    {
        Thread* joiner;
        bool hasBeenJoined;
        int tid;
        int exitCode;

        ThreadState state;
        Addr pc;
        RegFile regs;
        Process* parent;
        Addr stackTop;

        TCB(int id, Process* p, Addr entryPC, Addr stackPtr) : joiner(nullptr),
                                                               hasBeenJoined(false),
                                                               tid(id),
                                                               exitCode(0),
                                                               state(ThreadState::NEW),
                                                               pc(entryPC),
                                                               regs(),
                                                               parent(p),
                                                               stackTop(stackPtr)
        {
            this->regs.reset();
            this->regs.write(2, stackPtr);
        }
    };

public:
    Thread(int id, Process* parent, Addr entry, Addr stack);

    ~Thread();
    inline int getTid() const { return this->tcb->tid; }
    inline ThreadState getState() const { return this->tcb->state; }
    inline void setState(ThreadState s) { this->tcb->state = s; }

    inline Addr getPC() const { return this->tcb->pc; }
    inline void setPC(Addr pc) { this->tcb->pc = pc; }
    inline Addr getStackTop() const { return this->tcb->stackTop; }

    inline RegFile& getRegs() { return this->tcb->regs; }
    inline Process* getProcess() const { return this->tcb->parent; }

    inline Thread* getJoiner() const { return this->tcb->joiner; }
    inline void setJoiner(Thread* t) { this->tcb->joiner = t; }

    inline void setHasBeenJoined(bool status) { this->tcb->hasBeenJoined = status; }
    inline bool getHasBeenJoined() const { return this->tcb->hasBeenJoined; }

    inline void setExitCode(int code) { this->tcb->exitCode = code; }
    inline int getExitCode() const { return this->tcb->exitCode; }

private:
    TCB* tcb;
};