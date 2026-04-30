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
    kernelfunction::initKernelSubsystem(RV32UMOS::kernel);
}

void RV32UMOS::destroy()
{
    kernelfunction::destroyKernelSubsystem(RV32UMOS::kernel);
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
            return;
        }

        bool threadDead = false;

        try
        {
            STATS.incInstructions();
            K_HAL->cpu.step();
            K_HAL->cpu.advancePC();
        }
        catch (SyscallException& sys)
        {
            bool prev = Interrupt::disable();
            SyscallStatus status = K_SYSCALLS->dispatch(sys.getSyscallID());

            if (status.error)
            {
                bool killed = K_PROC_MANAGER->killProcess(K_PROC_MANAGER->getCurrentThread()->getProcess()->getPid());
                if (!killed) PANIC("Failed to kill process after Syscall Error!");
                threadDead = true;
            }
            if (!status.error) Interrupt::restore(prev);
            if (!status.error && status.needReschedule) K_SCHEDULER->preempt();
        }
        catch (PageFaultException& pf)
        {
            bool prev = Interrupt::disable();

            bool handled = K_VMM->handlePageFault(pf.getFaultAddr());
            if (!handled)
            {
                bool killed = K_PROC_MANAGER->killProcess(K_PROC_MANAGER->getCurrentThread()->getProcess()->getPid());
                if (!killed) PANIC("KERNEL PANIC: Failed to kill process after Segfault!");
                threadDead = true;
            }
            if (handled) Interrupt::restore(prev);
            if (handled && K_PROC_MANAGER->getCurrentThread()->getState() == ThreadState::BLOCKED) K_SCHEDULER->preempt();
        }
        catch (std::exception& e)
        {
            Interrupt::disable();
            LOG(KERNEL, ERROR, "Unhandled C++ Exception: " + std::string(e.what()));

            bool killed = K_PROC_MANAGER->killProcess(K_PROC_MANAGER->getCurrentThread()->getProcess()->getPid());
            if (!killed) PANIC("Failed to kill process after Exception!");
            threadDead = true;
        }

        if (threadDead)
        {
            K_SCHEDULER->preempt();
            return;
        }
    }
    PANIC("Unexpected error");
}