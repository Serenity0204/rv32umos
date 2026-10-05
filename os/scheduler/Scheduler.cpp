#include "Scheduler.hpp"
#include "HAL.hpp"
#include "KernelAlias.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Thread.hpp"

void Scheduler::preempt()
{
    if (K_PROC_MANAGER->activeThreads.empty())
    {
        return;
    }

    int prevIndex = K_PROC_MANAGER->currentThreadIndex;
    int nextIndex = prevIndex;
    std::size_t attempts = 0;
    bool found = false;
    std::size_t count = K_PROC_MANAGER->activeThreads.size();

    do
    {
        nextIndex = (nextIndex + 1) % count;
        attempts++;
        if (K_PROC_MANAGER->activeThreads[nextIndex]->getState() == ThreadState::READY)
        {
            found = true;
            break;
        }
    } while (attempts < count);

    if (!found)
    {
        // No READY thread. Keep running current if it is still runnable
        if (this->checkCurrentThreadRunnable()) return;

        // Check if everyone is terminated -> main loop will exit.
        if (this->checkAllTerminated())
        {
            LOG(SCHEDULER, INFO, "All threads terminated.");
            return;
        }

        // No READY and not all dead: threads are BLOCKED on sync primitives
        // (join/mutex/process-wait) with no one left to wake them -> deadlock.
        LOG(SCHEDULER, ERROR, "Deadlock: no READY threads but not all terminated.");
        return;
    }

    if (nextIndex != prevIndex)
    {
        Thread* nextThread = K_PROC_MANAGER->activeThreads[nextIndex];
        Process* proc = nextThread->getProcess();
        LOG(SCHEDULER, INFO, "Switching to Thread " + std::to_string(nextThread->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")");
        this->contextSwitch(nextIndex);
    }
}

void Scheduler::contextSwitch(std::size_t nextIndex)
{
    STATS.incContextSwitches();

    int prevIndex = K_PROC_MANAGER->currentThreadIndex;
    Thread* prevThread = (prevIndex != -1) ? K_PROC_MANAGER->activeThreads[prevIndex] : nullptr;
    Thread* nextThread = K_PROC_MANAGER->activeThreads[nextIndex];

    // Swap RISC-V context
    if (prevThread != nullptr && prevThread->getState() != ThreadState::TERMINATED)
    {
        prevThread->getRegs() = CPU_HAL->getRegs();
        prevThread->setPC(CPU_HAL->getPC());

        if (prevThread->getState() == ThreadState::RUNNING) prevThread->setState(ThreadState::READY);
    }

    CPU_HAL->getRegs() = nextThread->getRegs();
    CPU_HAL->setPC(nextThread->getPC());
    nextThread->setState(ThreadState::RUNNING);

    // switch page table on process change (or first boot)
    if (prevThread == nullptr || prevThread->getProcess()->getPid() != nextThread->getProcess()->getPid())
        CPU_HAL->setPageTable(nextThread->getProcess()->getPageTable());

    K_PROC_MANAGER->currentThreadIndex = nextIndex;
}

bool Scheduler::checkCurrentThreadRunnable()
{
    if (K_PROC_MANAGER->currentThreadIndex == -1) return false;
    Thread* current = K_PROC_MANAGER->getCurrentThread();
    if (current->getState() == ThreadState::RUNNING) return true;
    return false;
}

bool Scheduler::checkAllTerminated()
{
    for (auto* t : K_PROC_MANAGER->activeThreads)
    {
        if (t->getState() != ThreadState::TERMINATED) return false;
    }
    return true;
}
