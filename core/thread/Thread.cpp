#include "Thread.hpp"
#include "KernelInstance.hpp"

Thread::Thread(int id, Process* parent, Addr entry, Addr stack) : tcb(new TCB(id, parent, entry, stack))
{
}

Thread::~Thread()
{
    delete this->tcb;
}

void Thread::setupHostContext(void (*wrapper)())
{
    getcontext(&this->hostContext);

    this->hostStack.resize(8 * 1024 * 1024);
    this->hostContext.uc_stack.ss_sp = this->hostStack.data();
    this->hostContext.uc_stack.ss_size = this->hostStack.size();
    this->hostContext.uc_link = nullptr;

    uintptr_t ptr = reinterpret_cast<uintptr_t>(&KernelInstance::instance());
    uint32_t lo = static_cast<uint32_t>(ptr & 0xFFFFFFFF);
    uint32_t hi = static_cast<uint32_t>((ptr >> 32) & 0xFFFFFFFF);

    makecontext(&this->hostContext, wrapper, 2, lo, hi);
}