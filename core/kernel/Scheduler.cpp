#include "Scheduler.hpp"
#include "Logger.hpp"

Scheduler::Scheduler(KernelContext* context) : ctx(context) {}

void Scheduler::yield()
{
    if (this->ctx->processList.empty()) return;

    // Round Robin: Find the next READY process
    int nextIndex = this->ctx->currentProcessIndex;
    std::size_t attempts = 0;
    bool found = false;

    do
    {
        nextIndex = (nextIndex + 1) % this->ctx->processList.size();
        attempts++;
        if (this->ctx->processList[nextIndex]->getState() == ProcessState::READY)
        {
            found = true;
            break;
        }
    } while (attempts <= this->ctx->processList.size());

    if (!found)
    {
        // Check if everyone is terminated
        bool allDead = this->checkAllTerminated();
        if (allDead)
        {
            LOG(SCHEDULER, INFO, "All processes terminated.");
            this->ctx->cpu.halt();
        }
        return;
    }

    // Perform Switch
    if (nextIndex != this->ctx->currentProcessIndex)
    {
        Process* nextProcess = this->ctx->processList[nextIndex];
        LOG(SCHEDULER, INFO, "Switching to PID " + std::to_string(nextProcess->getPid()));
        this->contextSwitch(nextIndex);
    }
}
void Scheduler::contextSwitch(std::size_t nextIndex)
{
    STATS.incContextSwitches();

    Process* nextProcess = this->ctx->processList[nextIndex];

    // Save Current State (if valid)
    if (this->ctx->currentProcessIndex != -1)
    {
        Process* current = this->ctx->processList[this->ctx->currentProcessIndex];
        if (current->getState() == ProcessState::RUNNING)
        {
            current->setState(ProcessState::READY);
            current->getRegs() = this->ctx->cpu.getRegs();
            current->setPC(this->ctx->cpu.getPC());
        }
    }

    this->ctx->cpu.getRegs() = nextProcess->getRegs();
    this->ctx->cpu.setPC(nextProcess->getPC());
    this->ctx->cpu.setPageTable(nextProcess->getPageTable());
    nextProcess->setState(ProcessState::RUNNING);

    this->ctx->currentProcessIndex = nextIndex;
}

bool Scheduler::checkAllTerminated()
{
    for (auto* p : this->ctx->processList)
    {
        if (p->getState() != ProcessState::TERMINATED) return false;
    }
    return true;
}

bool Scheduler::admitProcesses()
{
    // ADMISSION: Move NEW -> READY
    bool hasReady = false;
    for (auto* proc : this->ctx->processList)
    {
        if (proc->getState() == ProcessState::NEW)
        {
            proc->setState(ProcessState::READY);
            hasReady = true;
        }
    }
    return hasReady;
}