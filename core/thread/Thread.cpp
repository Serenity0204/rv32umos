#include "Thread.hpp"
#include "KernelInstance.hpp"

Thread::Thread(int id, Process* parent, Addr entry, Addr stack) : tcb(new TCB(id, parent, entry, stack))
{
}

Thread::~Thread()
{
    delete this->tcb;
}

void Thread::setupHostContext(void (*func)())
{
    // Host side context switch setup
    this->hostStack.resize(HOST_STACK_SIZE);
    uintptr_t sp_addr = reinterpret_cast<uintptr_t>(this->hostStack.data() + this->hostStack.size());
    sp_addr &= ~0xF;
    uint64_t* sp = reinterpret_cast<uint64_t*>(sp_addr);

    *(--sp) = 0;
    *(--sp) = reinterpret_cast<uint64_t>(func);
    for (int i = 0; i < 6; ++i) *(--sp) = 0;
    this->hostStackPointer = sp;
}