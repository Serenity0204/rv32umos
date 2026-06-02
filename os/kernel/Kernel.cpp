#include "Kernel.hpp"
#include "Interrupt.hpp"
#include "KernelAlias.hpp"
#include "KernelPanic.hpp"
#include "KernelService.hpp"

void Kernel::initKernelSubsystem(Kernel* kernel)
{
    // init HAL
    kernel->hal = new HAL();
    KernelService::registerService("hal", kernel->hal);

    // init kernel subsystem
    kernel->pmm = new PhysicalMemoryManager();
    kernel->pmm->init(&kernel->hal->physicalRAM);

    kernel->procManager = new ProcessManager();
    kernel->scheduler = new Scheduler();
    kernel->syscalls = new SyscallHandler();
    kernel->vmm = new VirtualMemoryManager();
    kernel->alarm = new Alarm();

    KernelService::registerService("pmm", kernel->pmm);
    KernelService::registerService("procManager", kernel->procManager);
    KernelService::registerService("scheduler", kernel->scheduler);
    KernelService::registerService("syscalls", kernel->syscalls);
    KernelService::registerService("vmm", kernel->vmm);
    KernelService::registerService("alarm", kernel->alarm);

    // init filesystem and swap
    kernel->pageReplacementPolicy = new PageReplacementPolicyImpl();
    kernel->pageReplacementPolicy->init(kernel->pmm->getTotalFrames());
    kernel->vfs = new VFSImpl(kernel->hal->disk);
    kernel->swap = new SwapManager(kernel->hal->disk, NUM_DISK_BLOCKS, NUM_SWAP_BLOCKS);

    KernelService::registerService("prp", kernel->pageReplacementPolicy);
    KernelService::registerService("vfs", kernel->vfs);
    KernelService::registerService("swap", kernel->swap);

    // register itself
    KernelService::registerService("kernel", kernel);
}

void Kernel::destroyKernelSubsystem(Kernel* kernel)
{
    delete kernel->swap;
    delete kernel->vfs;
    delete kernel->pageReplacementPolicy;

    delete kernel->vmm;
    delete kernel->syscalls;
    delete kernel->scheduler;
    delete kernel->procManager;
    delete kernel->alarm;
    delete kernel->pmm;

    delete kernel->hal;
    KernelService::clear();
    Interrupt::init(nullptr);
}

void Kernel::handleSyscall(SyscallException& sys)
{
    bool prev = Interrupt::disable();
    SyscallStatus status = K_SYSCALLS->dispatch(sys.getSyscallID());

    if (status.error)
    {
        bool killed = K_PROC_MANAGER->killProcess(K_PROC_MANAGER->getCurrentThread()->getProcess()->getPid());
        if (!killed) PANIC("Failed to kill process after Syscall Error!");
        K_SCHEDULER->preempt();
        // never comeback here
    }
    Interrupt::restore(prev);
    if (status.needReschedule) K_SCHEDULER->preempt();
}

void Kernel::handlePageFault(PageFaultException& pf)
{
    bool prev = Interrupt::disable();
    bool handled = K_VMM->handlePageFault(pf.getFaultAddr());
    if (!handled)
    {
        bool killed = K_PROC_MANAGER->killProcess(K_PROC_MANAGER->getCurrentThread()->getProcess()->getPid());
        if (!killed) PANIC("KERNEL PANIC: Failed to kill process after Segfault!");
        // never comeback here
    }
    Interrupt::restore(prev);
    if (K_PROC_MANAGER->getCurrentThread()->getState() == ThreadState::BLOCKED) K_SCHEDULER->preempt();
}
