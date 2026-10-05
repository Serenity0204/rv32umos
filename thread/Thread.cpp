#include "Thread.hpp"

Thread::Thread(int id, Process* parent, Addr entry, Addr stack) : tcb(new TCB(id, parent, entry, stack))
{
}

Thread::~Thread()
{
    delete this->tcb;
}