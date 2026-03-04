#include "Process.hpp"

Process::Process(int id, std::string name)
{
    this->pcb = new PCB(id, name);
}

Process::~Process()
{
    if (this->pcb != nullptr)
    {
        delete this->pcb;
        this->pcb = nullptr;
    }
}

Thread* Process::createThread(Addr entryPC, Word arg)
{
    if (this->pcb->threads.size() >= MAX_THREADS) return nullptr;

    bool isExecutable = false;

    // check the passed in address is valid
    for (const Segment& seg : this->pcb->segments)
    {
        if (entryPC >= seg.vaddr && entryPC < (seg.vaddr + seg.memSize))
        {
            if (seg.flags & PF_X)
            {
                isExecutable = true;
                break;
            }
        }
    }
    if (!isExecutable) return nullptr;

    Addr stackSize = this->pcb->threads.empty() ? MAIN_STACK_SIZE : THREAD_STACK_SIZE;
    Addr stackTop = this->pcb->nextStackBase;

    // for next thread
    this->pcb->nextStackBase = stackTop - stackSize - GUARD_PAGE_SIZE;

    int tid = static_cast<int>(this->pcb->threads.size());
    Thread* thread = new Thread(tid, this, entryPC, stackTop);

    thread->getRegs().write(10, arg);
    this->pcb->threads.push_back(thread);
    return thread;
}

int Process::createMutex()
{
    Mutex* mutex = new Mutex();
    int id = this->pcb->mutexList.size();
    this->pcb->mutexList.push_back(mutex);
    return id;
}

Mutex* Process::getMutex(int id)
{
    if (id < 0 || static_cast<std::size_t>(id) >= this->pcb->mutexList.size()) return nullptr;
    return this->pcb->mutexList[id];
}

int Process::addFileHandle(FileHandleInterface* handle)
{
    if (handle == nullptr) return -1;

    // find the first ever fd slot
    int fd = -1;
    for (std::size_t i = 0; i < this->pcb->fdTable.size(); ++i)
    {
        if (this->pcb->fdTable[i] == nullptr)
        {
            fd = static_cast<int>(i);
            break;
        }
    }

    // no space
    if (fd == -1) return -1;
    this->pcb->fdTable[fd] = handle;
    return fd;
}

FileHandleInterface* Process::getFileHandle(int fd)
{
    if (fd < 0 || fd >= static_cast<int>(this->pcb->fdTable.size())) return nullptr;
    return this->pcb->fdTable[fd];
}

bool Process::closeFileHandle(int fd)
{
    if (fd < 0 || fd >= static_cast<int>(this->pcb->fdTable.size())) return false;
    FileHandleInterface* h = this->pcb->fdTable[fd];
    if (h == nullptr) return false;
    h->close();
    delete h;
    this->pcb->fdTable[fd] = nullptr;
    return true;
}

Addr Process::sbrk(int increment)
{
    Addr oldBreak = this->pcb->programBreak;
    Addr newBreak = this->pcb->programBreak + increment;

    if (newBreak < HEAP_START || newBreak > HEAP_MAX_LIMIT) return 0;
    this->pcb->programBreak = newBreak;
    return oldBreak;
}