#include "Scheduler.hpp"
#include "Logger.hpp"

Scheduler::Scheduler(KernelContext* context) : ctx(context) {}

void Scheduler::yield()
{
    if (this->ctx->activeThreads.empty()) return;

    // Round Robin: Find the next READY thread
    int nextIndex = this->ctx->currentThreadIndex;
    std::size_t attempts = 0;
    bool found = false;
    std::size_t count = this->ctx->activeThreads.size();

    do
    {
        nextIndex = (nextIndex + 1) % count;
        attempts++;
        if (this->ctx->activeThreads[nextIndex]->getState() == ThreadState::READY)
        {
            found = true;
            break;
        }
    } while (attempts <= count);

    if (!found)
    {
        bool canRunCurrentProcess = this->checkCurrentThreadRunnable();
        if (canRunCurrentProcess)
        {
            this->ctx->timer.reset();
            return;
        }

        // Check if everyone is terminated
        bool allDead = this->checkAllTerminated();
        if (allDead)
        {
            LOG(SCHEDULER, INFO, "All threads terminated.");
            this->ctx->cpu.halt();
        }
        return;
    }

    // reset the timer
    this->ctx->timer.reset();

    // Perform Switch
    if (nextIndex != this->ctx->currentThreadIndex)
    {
        Thread* nextThread = this->ctx->activeThreads[nextIndex];
        Process* proc = nextThread->getProcess();
        LOG(SCHEDULER, INFO, "Switching to Thread " + std::to_string(nextThread->getTid()) + " (PID " + std::to_string(proc->getPid()) + ")");
        this->contextSwitch(nextIndex);
    }
}

void Scheduler::contextSwitch(std::size_t nextIndex)
{
    STATS.incContextSwitches();
    this->ctx->timer.tick(CONTEXT_SWITCH_TIME);

    Thread* nextThread = this->ctx->activeThreads[nextIndex];
    Thread* currentThread = this->ctx->getCurrentThread();

    // Save Current State (if valid)
    if (currentThread != nullptr && currentThread->getState() != ThreadState::TERMINATED)
    {
        currentThread->getRegs() = this->ctx->cpu.getRegs();
        currentThread->setPC(this->ctx->cpu.getPC());

        if (currentThread->getState() == ThreadState::RUNNING) currentThread->setState(ThreadState::READY);
    }

    this->ctx->cpu.getRegs() = nextThread->getRegs();
    this->ctx->cpu.setPC(nextThread->getPC());
    nextThread->setState(ThreadState::RUNNING);

    // check if it's switch within the same process
    if (currentThread == nullptr || currentThread->getProcess()->getPid() != nextThread->getProcess()->getPid())
        this->ctx->cpu.setPageTable(nextThread->getProcess()->getPageTable());

    this->ctx->currentThreadIndex = nextIndex;
}

bool Scheduler::checkCurrentThreadRunnable()
{
    Thread* current = this->ctx->getCurrentThread();
    if (current == nullptr) return false;
    if (current->getState() == ThreadState::RUNNING) return true;
    return false;
}

bool Scheduler::checkAllTerminated()
{
    for (auto* t : this->ctx->activeThreads)
    {
        if (t->getState() != ThreadState::TERMINATED) return false;
    }
    return true;
}
