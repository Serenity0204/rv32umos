#include "Kernel.hpp"
#include "Exception.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Utils.hpp"

Kernel::Kernel()
    : scheduler(&ctx), vmm(&ctx), syscalls(&ctx), loader(&ctx)
{
}

bool Kernel::createProcess(const std::string& filename)
{
    return this->loader.loadELF(filename);
}

bool Kernel::killProcess(int pid)
{
    if (pid < 0 || static_cast<size_t>(pid) >= this->ctx.processList.size())
    {
        LOG(KERNEL, ERROR, "Killing process with PID: " + std::to_string(pid) + " that does not exist.");
        return false;
    }
    Process* process = this->ctx.processList[pid];
    std::vector<Thread*>& allThreads = process->getThreads();
    for (Thread* thread : allThreads)
        thread->setState(ThreadState::TERMINATED);

    LOG(KERNEL, INFO, "Killing Process " + std::to_string(pid) + " (CRASHED)");
    return true;
}

void Kernel::run()
{
    // ADMISSION: Move NEW -> READY
    bool hasReady = !this->ctx.activeThreads.empty();

    if (!hasReady)
    {
        LOG(KERNEL, WARNING, "No READY processes.");
        return;
    }

    this->ctx.cpu.enableVM(true);

    // start the first process
    this->scheduler.yield();

    LOG(KERNEL, INFO, "Simulation started...");

    while (!this->ctx.cpu.isHalted())
    {
        try
        {
            STATS.incInstructions();
            this->ctx.cpu.step();
            this->ctx.timer.tick(USER_MODE_TICK_TIME);
            this->ctx.cpu.advancePC();
        }
        catch (SyscallException& sys)
        {
            this->ctx.timer.tick(ENTER_KERNEL_MODE_TIME);

            // will handle pc increment individually, exit will yield
            SyscallStatus status = this->syscalls.dispatch(sys.getSyscallID());
            if (status.needReschedule)
            {
                this->scheduler.yield();
                continue;
            }
        }
        catch (PageFaultException& pf)
        {
            this->ctx.timer.tick(ENTER_KERNEL_MODE_TIME);
            bool pageFaultHandled = this->vmm.handlePageFault(pf.getFaultAddr());
            // If VMM returns false, it was a crash -> reschedule
            bool crashed = !pageFaultHandled;
            if (crashed)
            {
                // terminate all threads related to this segfault process(current process)
                bool killed = this->killProcess(this->ctx.getCurrentThread()->getProcess()->getPid());

                // if not killed, kernel panic
                if (!killed) this->ctx.cpu.halt();

                // after killing, yield
                this->scheduler.yield();
                continue;
            }
        }
        catch (std::exception& e)
        {
            LOG(KERNEL, ERROR, "Unexpected exception: " + std::string(e.what()));
            this->ctx.cpu.halt();
            break;
        }

        if (this->ctx.timer.isInterruptPending())
        {
            LOG(KERNEL, INFO, "Timer Interrupt.");
            this->scheduler.yield();
        }
    }

    STATS.printSummary();
}