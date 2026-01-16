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

void Kernel::runFirstProcess()
{
    this->ctx.currentProcessIndex = 0;
    Process* first = this->ctx.processList[0];
    this->ctx.cpu.getRegs() = first->getRegs();
    this->ctx.cpu.setPC(first->getPC());

    this->ctx.cpu.setPageTable(first->getPageTable());
    this->ctx.cpu.enableVM(true);
    first->setState(ProcessState::RUNNING);
}

void Kernel::run()
{
    // ADMISSION: Move NEW -> READY
    bool hasReady = this->scheduler.admitProcesses();
    if (!hasReady)
    {
        LOG(KERNEL, WARNING, "No READY processes.");
        return;
    }

    // start the first process
    this->runFirstProcess();

    LOG(KERNEL, INFO, "Simulation started...");
    // for context switching only
    uint64_t instructions = 0;

    while (!this->ctx.cpu.isHalted())
    {
        try
        {
            this->ctx.cpu.step();
            STATS.incInstructions();
            instructions++;
            this->ctx.cpu.advancePC();
        }
        catch (SyscallException& sys)
        {
            // will handle pc increment individually, exit will yield
            if (this->syscalls.dispatch(sys.getSyscallID()))
                this->scheduler.yield();
        }
        catch (PageFaultException& pf)
        {
            // If VMM returns false, it was a crash -> reschedule
            if (!this->vmm.handlePageFault(pf.getFaultAddr()))
                this->scheduler.yield();
        }
        catch (std::exception& e)
        {
            LOG(KERNEL, ERROR, "Unexpected exception: " + std::string(e.what()));
            this->ctx.cpu.halt();
            break;
        }

        if (instructions % TIME_QUANTUM == 0) this->scheduler.yield();
    }

    STATS.printSummary();
}