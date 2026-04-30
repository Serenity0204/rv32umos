#include "ProcessManager.hpp"
#include "Common.hpp"
#include "KernelPanic.hpp"
#include "Loader.hpp"

Thread* ProcessManager::getCurrentThread()
{
    if (currentThreadIndex == -1) PANIC("Current thread index is -1");
    if (static_cast<std::size_t>(currentThreadIndex) >= activeThreads.size()) PANIC("Current thread index out of bound");
    return this->activeThreads[this->currentThreadIndex];
}

ProcessManager::ProcessManager()
{
    for (int i = 0; i < MAX_PROCESS; ++i)
    {
        Process* p = new Process(i, "");
        p->setActive(false);
        this->processList.push_back(p);
    }
}

ProcessManager::~ProcessManager()
{
    for (auto* p : this->processList) delete p;
    this->processList.clear();

    for (auto* t : this->activeThreads) delete t;
    this->activeThreads.clear();
}

bool ProcessManager::killProcess(int pid)
{
    return Process::terminate(pid, -1, true);
}

bool ProcessManager::createProcess(const std::string& filename)
{
    return Loader::loadELF(filename) != -1;
}
