#include "RV32UMOS.hpp"
#include "Exception.hpp"
#include "Interrupt.hpp"
#include "KernelAlias.hpp"
#include "KernelPanic.hpp"
#include "KernelService.hpp"
#include "Logger.hpp"
#include "Stats.hpp"

Kernel* RV32UMOS::kernel = nullptr;

void RV32UMOS::init()
{
    RV32UMOS::kernel = new Kernel();
    Kernel::initKernelSubsystem(RV32UMOS::kernel);
}

void RV32UMOS::destroy()
{
    Kernel::destroyKernelSubsystem(RV32UMOS::kernel);
    delete RV32UMOS::kernel;
    RV32UMOS::kernel = nullptr;
}

void RV32UMOS::reset()
{
    RV32UMOS::destroy();
    RV32UMOS::init();
}

bool RV32UMOS::loadApplication(const std::string& filename)
{
    return K_PROC_MANAGER->createProcess(filename);
}

void RV32UMOS::start()
{
    bool hasReady = !K_PROC_MANAGER->activeThreads.empty();
    if (!hasReady)
    {
        LOG(KERNEL, WARNING, "No READY processes.");
        return;
    }

    K_HAL->cpu.enableVM(true);

    Interrupt::enable();
    Interrupt::init(K_SCHEDULER);
    Interrupt::disable();

    K_HAL->timer.start(TIMER_INTERRUPT_FREQUENCY);
    LOG(KERNEL, INFO, "rv32umos Booting...");
    K_SCHEDULER->preempt();
    K_HAL->timer.stop();
}

void RV32UMOS::runThread()
{
    Interrupt::enable();

    while (true)
    {
        // atomic check
        bool prev = Interrupt::disable();
        Thread* self = K_PROC_MANAGER->getCurrentThread();
        self->getProcess()->incrementInstruction();
        if (self->getState() == ThreadState::TERMINATED)
        {
            Interrupt::restore(prev);
            K_SCHEDULER->preempt();
        }

        try
        {
            STATS.incInstructions();
            K_HAL->cpu.step();
            K_HAL->cpu.advancePC();
        }
        catch (SyscallException& sys)
        {
            K_KERNEL->handleSyscall(sys);
        }
        catch (PageFaultException& pf)
        {
            K_KERNEL->handlePageFault(pf);
        }
        catch (std::exception& e)
        {
            PANIC("Unhandled C++ Exception: " + std::string(e.what()));
        }
    }
    PANIC("Unexpected error");
}