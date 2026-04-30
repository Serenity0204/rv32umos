#include "Scheduler.hpp"
#include "Interrupt.hpp"
#include "KernelAlias.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Thread.hpp"

void Scheduler::preempt()
{
    bool prev = Interrupt::disable();

    K_ALARM->tick();

    if (K_PROC_MANAGER->activeThreads.empty())
    {
        Interrupt::restore(prev);
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
            void* dummy_sp = nullptr;
            context_switch(&dummy_sp, this->mainStackPtr);
            return;
        }

        LOG(SCHEDULER, INFO, "System IDLE: All threads blocked. Waiting for interrupts...");
        this->isIdling = true;

        while (!found)
        {
            sigset_t empty_mask;
            sigemptyset(&empty_mask);

            // Sleep until the POSIX Timer fires a signal!
            sigsuspend(&empty_mask);

            // Tick the soft timer
            K_ALARM->tick();

            // Check if the software timer woke up a thread
            for (std::size_t i = 0; i < count; ++i)
            {
                nextIndex = (nextIndex + 1) % count;
                if (K_PROC_MANAGER->activeThreads[nextIndex]->getState() == ThreadState::READY)
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
        Thread* nextThread = K_PROC_MANAGER->activeThreads[nextIndex];
        Process* proc = nextThread->getProcess();
        LOG(SCHEDULER, INFO, "Switching to Thread " + std::to_string(nextThread->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")");
        this->contextSwitch(nextIndex);
    }
    Interrupt::restore(prev);
}

void Scheduler::contextSwitch(std::size_t nextIndex)
{
    STATS.incContextSwitches();

    int prevIndex = K_PROC_MANAGER->currentThreadIndex;
    Thread* prevThread = (prevIndex != -1) ? K_PROC_MANAGER->activeThreads[prevIndex] : nullptr;
    Thread* nextThread = K_PROC_MANAGER->activeThreads[nextIndex];

    // Layer 1: Swap RISC-V State
    if (prevThread != nullptr && prevThread->getState() != ThreadState::TERMINATED)
    {
        prevThread->getRegs() = K_HAL->cpu.getRegs();
        prevThread->setPC(K_HAL->cpu.getPC());

        if (prevThread->getState() == ThreadState::RUNNING) prevThread->setState(ThreadState::READY);
    }

    K_HAL->cpu.getRegs() = nextThread->getRegs();
    K_HAL->cpu.setPC(nextThread->getPC());
    nextThread->setState(ThreadState::RUNNING);

    // check if it's switch within the same process
    if (prevThread == nullptr || prevThread->getProcess()->getPid() != nextThread->getProcess()->getPid())
        K_HAL->cpu.setPageTable(nextThread->getProcess()->getPageTable());

    K_PROC_MANAGER->currentThreadIndex = nextIndex;

    // Layer 2: Swap Host C++ State
    if (prevThread != nullptr)
    {
        context_switch(&prevThread->hostStackPointer, nextThread->hostStackPointer);
        return;
    }

    // First Boot! Jump away from the main loop context
    context_switch(&this->mainStackPtr, nextThread->hostStackPointer);
}

bool Scheduler::checkCurrentThreadRunnable()
{
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

void Scheduler::sleepCurrentThread(int delayMs, const std::string& reason)
{
    Thread* current = K_PROC_MANAGER->getCurrentThread();
    if (current->getState() == ThreadState::BLOCKED)
    {
        // the thread is already blocked Natively extend its underlying timer.
        bool extended = K_ALARM->extendTimer(current->sleepTimerId, delayMs);
        if (extended)
            LOG(SCHEDULER, INFO, "Extended sleep for Thread " + std::to_string(current->getTid()) + " by " + std::to_string(delayMs) + "ms for " + reason);
        return;
    }

    current->setState(ThreadState::BLOCKED);
    LOG(SCHEDULER, INFO, "Thread " + std::to_string(current->getTid()) + " BLOCKED for " + reason + " (" + std::to_string(delayMs) + "ms)");

    current->sleepTimerId = K_ALARM->registerTimer(delayMs, [current, reason]()
    {
        current->setState(ThreadState::READY);
        current->sleepTimerId = 0;
        LOG(SCHEDULER, INFO, reason + " Complete: Waking up Thread " + std::to_string(current->getTid()));
    });
}