#include "Kernel.hpp"
#include "Exception.hpp"
#include "Interrupt.hpp"
#include "Logger.hpp"
#include "ScopedCriticalSection.hpp"
#include "Stats.hpp"
#include "Utils.hpp"

Kernel::Kernel()
    : systemCtx(nullptr),
      storageCtx(nullptr),
      scheduler(nullptr),
      vmm(nullptr),
      syscalls(nullptr),
      loader(nullptr)
{
    this->systemCtx = new SystemContext();
    this->storageCtx = new StorageContext();
    this->storageCtx->init<StubFileSystem, InMemoryDisk, FIFOPolicy>(NUM_DISK_BLOCKS, NUM_SWAP_BLOCKS, this->systemCtx->pmm.getTotalFrames());
    this->scheduler = new Scheduler();
    this->syscalls = new SyscallHandler();
    this->vmm = new VirtualMemoryManager();
    this->loader = new Loader();
    this->timer = new HardwareTimer();
}

Kernel::~Kernel()
{
    delete this->storageCtx;
    delete this->systemCtx;
    delete this->scheduler;
    delete this->vmm;
    delete this->syscalls;
    delete this->loader;
    delete this->timer;
}

bool Kernel::createProcess(const std::string& filename)
{
    return this->loader->loadELF(filename);
}

bool Kernel::killProcess(int pid)
{
    if (pid < 0 || static_cast<size_t>(pid) >= this->systemCtx->processList.size())
    {
        LOG(KERNEL, ERROR, "Killing process with PID: " + std::to_string(pid) + " that does not exist.");
        return false;
    }
    Process* process = this->systemCtx->processList[pid];
    std::vector<Thread*>& allThreads = process->getThreads();
    for (Thread* thread : allThreads)
        thread->setState(ThreadState::TERMINATED);

    LOG(KERNEL, INFO, "Killing Process " + std::to_string(pid) + " (CRASHED)");
    return true;
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

    this->timer->start(TIMER_INTERRUPT_FREQUENCY);
    LOG(KERNEL, INFO, "Simulation started...");
    this->scheduler->preempt();
}

void Kernel::runThread(uint32_t lo, uint32_t hi)
{
    uintptr_t ptr = (static_cast<uintptr_t>(hi) << 32) | lo;
    Kernel* kernel = reinterpret_cast<Kernel*>(ptr);

    Interrupt::enable();

    while (true)
    {
        // atomic check
        bool status = Interrupt::disable();
        if (kernel->systemCtx->cpu.isHalted()) break;
        Interrupt::restore(status);

        try
        {
            STATS.incInstructions();
            kernel->systemCtx->cpu.step();
            kernel->systemCtx->cpu.advancePC();
        }
        catch (SyscallException& sys)
        {
            ScopedCriticalSection lock;
            SyscallStatus status = kernel->syscalls->dispatch(sys.getSyscallID());

            if (status.error)
            {
                bool killed = kernel->killProcess(kernel->systemCtx->getCurrentThread()->getProcess()->getPid());
                if (!killed)
                {
                    LOG(KERNEL, ERROR, "KERNEL PANIC: Failed to kill process after Syscall Error!");
                    kernel->systemCtx->cpu.halt();
                    setcontext(&kernel->systemCtx->mainContext);
                    return;
                }
                kernel->scheduler->preempt();
                // Thread is dead, returning is safe here
                return;
            }
            if (status.needReschedule) kernel->scheduler->preempt();
        }
        catch (PageFaultException& pf)
        {
            ScopedCriticalSection lock;
            bool handled = kernel->vmm->handlePageFault(pf.getFaultAddr());
            if (!handled)
            {
                bool killed = kernel->killProcess(kernel->systemCtx->getCurrentThread()->getProcess()->getPid());
                if (!killed)
                {
                    LOG(KERNEL, ERROR, "KERNEL PANIC: Failed to kill process after Segfault!");
                    kernel->systemCtx->cpu.halt();
                    setcontext(&kernel->systemCtx->mainContext);
                    return;
                }
                kernel->scheduler->preempt();
                // Thread is dead, safe to return.
                return;
            }
        }
        catch (std::exception& e)
        {
            ScopedCriticalSection lock;
            LOG(KERNEL, ERROR, "Unhandled C++ Exception: " + std::string(e.what()));

            bool killed = kernel->killProcess(kernel->systemCtx->getCurrentThread()->getProcess()->getPid());
            if (!killed)
            {
                LOG(KERNEL, ERROR, "KERNEL PANIC: Failed to kill process after Exception!");
                kernel->systemCtx->cpu.halt();
                setcontext(&kernel->systemCtx->mainContext);
                return;
            }
            kernel->scheduler->preempt();
            // Thread is dead, safe to return.
            return;
        }
    }

    // kernel panic
    Interrupt::disable();
    Thread* current = kernel->systemCtx->getCurrentThread();
    if (current != nullptr)
    {
        bool killed = kernel->killProcess(current->getProcess()->getPid());
        if (!killed)
        {
            LOG(KERNEL, ERROR, "KERNEL PANIC: CPU Halted and process kill failed.");
            setcontext(&kernel->systemCtx->mainContext);
            return;
        }
        kernel->scheduler->preempt();
        return;
    }

    LOG(KERNEL, ERROR, "KERNEL PANIC: CPU Halted with no active thread.");
    setcontext(&kernel->systemCtx->mainContext);
}

bool Kernel::isRunning()
{
    return !this->systemCtx->cpu.isHalted();
}