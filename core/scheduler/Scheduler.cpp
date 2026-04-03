#include "Scheduler.hpp"
#include "Interrupt.hpp"
#include "KernelInstance.hpp"
#include "Logger.hpp"

void Scheduler::preempt()
{
    bool prev = Interrupt::disable();

    kernel.timerCtx->software.tick();
    if (kernel.systemCtx->activeThreads.empty())
    {
        Interrupt::restore(prev);
        return;
    }

    int prevIndex = kernel.systemCtx->currentThreadIndex;
    int nextIndex = prevIndex;
    std::size_t attempts = 0;
    bool found = false;
    std::size_t count = kernel.systemCtx->activeThreads.size();

    do
    {
        nextIndex = (nextIndex + 1) % count;
        attempts++;
        if (kernel.systemCtx->activeThreads[nextIndex]->getState() == ThreadState::READY)
        {
            found = true;
            break;
        }
    } while (attempts < count);

    if (!found)
    {
        bool canRunCurrentProcess = this->checkCurrentThreadRunnable();
        if (canRunCurrentProcess)
        {
            Interrupt::restore(prev);
            return;
        }
        // Check if everyone is terminated
        bool allDead = this->checkAllTerminated();
        if (allDead)
        {
            LOG(SCHEDULER, INFO, "All threads terminated.");
            kernel.systemCtx->cpu.halt();
            setcontext(&kernel.systemCtx->mainContext);
            return;
        }
        LOG(SCHEDULER, INFO, "System IDLE: All threads blocked. Waiting for interrupts...");
        this->isIdling = true;
        while (!found)
        {
            sigset_t empty_mask;
            sigemptyset(&empty_mask);
            sigsuspend(&empty_mask);

            // tick the soft timer
            kernel.timerCtx->software.tick();

            // Check if the software timer wakes up a thread
            for (std::size_t i = 0; i < count; ++i)
            {
                nextIndex = (nextIndex + 1) % count;
                if (kernel.systemCtx->activeThreads[nextIndex]->getState() == ThreadState::READY)
                {
                    found = true;
                    break;
                }
            }
        }
        this->isIdling = false;
    }

    if (nextIndex != prevIndex)
    {
        Thread* nextThread = kernel.systemCtx->activeThreads[nextIndex];
        Process* proc = nextThread->getProcess();
        LOG(SCHEDULER, INFO, "Switching to Thread " + std::to_string(nextThread->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")");
        this->contextSwitch(nextIndex);
    }
    Interrupt::restore(prev);
}

void Scheduler::contextSwitch(std::size_t nextIndex)
{
    STATS.incContextSwitches();

    int prevIndex = kernel.systemCtx->currentThreadIndex;
    Thread* prevThread = (prevIndex != -1) ? kernel.systemCtx->activeThreads[prevIndex] : nullptr;
    Thread* nextThread = kernel.systemCtx->activeThreads[nextIndex];

    // Layer 1: Swap RISCV State
    if (prevThread != nullptr && prevThread->getState() != ThreadState::TERMINATED)
    {
        prevThread->getRegs() = kernel.systemCtx->cpu.getRegs();
        prevThread->setPC(kernel.systemCtx->cpu.getPC());

        if (prevThread->getState() == ThreadState::RUNNING) prevThread->setState(ThreadState::READY);
    }

    kernel.systemCtx->cpu.getRegs() = nextThread->getRegs();
    kernel.systemCtx->cpu.setPC(nextThread->getPC());
    nextThread->setState(ThreadState::RUNNING);

    // check if it's switch within the same process
    if (prevThread == nullptr || prevThread->getProcess()->getPid() != nextThread->getProcess()->getPid())
        kernel.systemCtx->cpu.setPageTable(nextThread->getProcess()->getPageTable());

    kernel.systemCtx->currentThreadIndex = nextIndex;

    // Layer 2: Swap Host C++ State
    if (prevThread != nullptr)
    {
        swapcontext(&prevThread->hostContext, &nextThread->hostContext);
        return;
    }

    // First Boot! Jump away from the main loop context
    swapcontext(&kernel.systemCtx->mainContext, &nextThread->hostContext);
}

bool Scheduler::checkCurrentThreadRunnable()
{
    Thread* current = kernel.systemCtx->getCurrentThread();
    if (current == nullptr) return false;
    if (current->getState() == ThreadState::RUNNING) return true;
    return false;
}

bool Scheduler::checkAllTerminated()
{
    for (auto* t : kernel.systemCtx->activeThreads)
    {
        if (t->getState() != ThreadState::TERMINATED) return false;
    }
    return true;
}