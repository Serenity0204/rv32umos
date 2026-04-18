#include "Kernel.hpp"
#include "Exception.hpp"
#include "Interrupt.hpp"
#include "KernelInstance.hpp"
#include "Logger.hpp"
#include "ScopedCriticalSection.hpp"
#include "Stats.hpp"
#include "Utils.hpp"

Kernel::Kernel()
    : systemCtx(nullptr),
      storageCtx(nullptr),
      timerCtx(nullptr),
      scheduler(nullptr),
      vmm(nullptr),
      syscalls(nullptr),
      loader(nullptr)
{
    this->systemCtx = new SystemContext();
    this->storageCtx = new StorageContext();
    this->storageCtx->init<StubFileSystem, InMemoryDisk, FIFOPolicy>(NUM_DISK_BLOCKS, NUM_SWAP_BLOCKS, this->systemCtx->pmm.getTotalFrames());
    this->timerCtx = new TimerContext();
    this->scheduler = new Scheduler();
    this->syscalls = new SyscallHandler();
    this->vmm = new VirtualMemoryManager();
    this->loader = new Loader();
}

Kernel::~Kernel()
{
    delete this->storageCtx;
    delete this->systemCtx;
    delete this->timerCtx;
    delete this->scheduler;
    delete this->vmm;
    delete this->syscalls;
    delete this->loader;
}

bool Kernel::createProcess(const std::string& filename)
{
    return this->loader->loadELF(filename) != -1;
}

bool Kernel::killProcess(int pid)
{
    return Process::terminate(pid, -1, true);
}

void Kernel::init()
{
    bool hasReady = !this->systemCtx->activeThreads.empty();

    if (!hasReady)
    {
        LOG(KERNEL, WARNING, "No READY processes.");
        return;
    }

    this->systemCtx->cpu.enableVM(true);
    Interrupt::init(this->scheduler);

    Interrupt::disable();

    this->timerCtx->hardware.start(TIMER_INTERRUPT_FREQUENCY);
    LOG(KERNEL, INFO, "Simulation started...");
    this->scheduler->preempt();
}

void Kernel::runThread()
{
    Interrupt::enable();

    while (true)
    {
        // atomic check
        bool status = Interrupt::disable();
        if (kernel.systemCtx->cpu.isHalted()) break;
        Interrupt::restore(status);

        bool threadDead = false;

        try
        {
            STATS.incInstructions();
            kernel.systemCtx->cpu.step();
            kernel.systemCtx->cpu.advancePC();
        }
        catch (SyscallException& sys)
        {
            bool prev = Interrupt::disable();
            SyscallStatus status = kernel.syscalls->dispatch(sys.getSyscallID());

            if (status.error)
            {
                bool killed = kernel.killProcess(kernel.systemCtx->getCurrentThread()->getProcess()->getPid());
                if (!killed)
                {
                    LOG(KERNEL, ERROR, "KERNEL PANIC: Failed to kill process after Syscall Error!");
                    kernel.systemCtx->cpu.halt();
                    // jump to main
                    void* dummy_sp = nullptr;
                    context_switch(&dummy_sp, kernel.systemCtx->mainStackPointer);
                    return;
                }
                threadDead = true;
            }
            if (!status.error) Interrupt::restore(prev);
            if (!status.error && status.needReschedule) kernel.scheduler->preempt();
        }
        catch (PageFaultException& pf)
        {
            bool prev = Interrupt::disable();

            bool handled = kernel.vmm->handlePageFault(pf.getFaultAddr());
            if (!handled)
            {
                bool killed = kernel.killProcess(kernel.systemCtx->getCurrentThread()->getProcess()->getPid());
                if (!killed)
                {
                    LOG(KERNEL, ERROR, "KERNEL PANIC: Failed to kill process after Segfault!");
                    kernel.systemCtx->cpu.halt();
                    void* dummy_sp = nullptr;
                    context_switch(&dummy_sp, kernel.systemCtx->mainStackPointer);
                    return;
                }
                threadDead = true;
            }
            if (handled) Interrupt::restore(prev);
            if (handled && kernel.systemCtx->getCurrentThread()->getState() == ThreadState::BLOCKED) kernel.scheduler->preempt();
        }
        catch (std::exception& e)
        {
            Interrupt::disable();
            LOG(KERNEL, ERROR, "Unhandled C++ Exception: " + std::string(e.what()));

            bool killed = kernel.killProcess(kernel.systemCtx->getCurrentThread()->getProcess()->getPid());
            if (!killed)
            {
                LOG(KERNEL, ERROR, "KERNEL PANIC: Failed to kill process after Exception!");
                kernel.systemCtx->cpu.halt();
                void* dummy_sp = nullptr;
                context_switch(&dummy_sp, kernel.systemCtx->mainStackPointer);
                return;
            }
            threadDead = true;
        }

        if (threadDead)
        {
            kernel.scheduler->preempt();
            // Thread is dead, returning is safe here
            return;
        }
    }

    // kernel panic
    Interrupt::disable();
    Thread* current = kernel.systemCtx->getCurrentThread();
    if (current != nullptr)
    {
        bool killed = kernel.killProcess(current->getProcess()->getPid());
        if (!killed)
        {
            LOG(KERNEL, ERROR, "KERNEL PANIC: CPU Halted and process kill failed.");
            void* dummy_sp = nullptr;
            context_switch(&dummy_sp, kernel.systemCtx->mainStackPointer);
            return;
        }
        kernel.scheduler->preempt();
        return;
    }

    LOG(KERNEL, ERROR, "KERNEL PANIC: CPU Halted with no active thread.");
    void* dummy_sp = nullptr;
    context_switch(&dummy_sp, kernel.systemCtx->mainStackPointer);
}

bool Kernel::isRunning()
{
    return !this->systemCtx->cpu.isHalted();
}