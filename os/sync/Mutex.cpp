#include "Mutex.hpp"

Mutex::Mutex() : owner(nullptr), locked(false)
{
}

bool Mutex::acquire(Thread* thread)
{
    if (this->locked)
    {
        thread->setState(ThreadState::BLOCKED);
        this->waitQueue.push(thread);
        return false;
    }

    this->locked = true;
    this->owner = thread;
    return true;
}
bool Mutex::release(Thread* thread)
{
    // a thread that's not the owner trying to release
    if (thread != this->owner) return false;

    // if empty, set the status directly
    if (this->waitQueue.empty())
    {
        this->owner = nullptr;
        this->locked = false;
        return true;
    }

    this->locked = false;
    this->owner = nullptr;

    while (!this->waitQueue.empty())
    {
        Thread* t = waitQueue.front();
        t->setState(ThreadState::READY);
        waitQueue.pop();
    }
    return true;
}

bool Mutex::isLocked() const { return this->locked; }