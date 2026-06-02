#include "Kernel.hpp"
#include "HAL.hpp"
#include "Interrupt.hpp"
#include "KernelAlias.hpp"
#include "KernelPanic.hpp"
#include "KernelService.hpp"
#include "SystemConfig.hpp"

void Kernel::initKernelSubsystem(Kernel* kernel, HAL* hal)
{
    // init HAL
    kernel->hal = hal;
    KernelService::registerService("hal", kernel->hal);

    // init kernel subsystem
    kernel->pmm = new PhysicalMemoryManager();
    kernel->pmm->init(MEMORY_HAL);

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
    kernel->vfs = new VFSImpl(DISK_HAL);
    kernel->swap = new SwapManager(DISK_HAL, NUM_DISK_BLOCKS, NUM_SWAP_BLOCKS);

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

    KernelService::clear();
    Interrupt::init(nullptr);
}

void Kernel::handleSyscall(SyscallID id)
{
    bool prev = INTERRUPT_HAL->disable();
    SyscallContext ctx;
    ctx.id = id;
    ctx.arg0 = CPU_HAL->readReg(10); // a0
    ctx.arg1 = CPU_HAL->readReg(11); // a1
    ctx.arg2 = CPU_HAL->readReg(12); // a2
    ctx.arg3 = CPU_HAL->readReg(13); // a3
    ctx.arg4 = CPU_HAL->readReg(14); // a4
    ctx.arg5 = CPU_HAL->readReg(15); // a5

    SyscallResult result = K_SYSCALLS->dispatch(ctx);

    if (result.hasReturnValue)
        CPU_HAL->writeReg(10, result.returnValue);

    if (result.advancePC)
        CPU_HAL->advancePC();

    if (result.error)
    {
        bool killed = K_PROC_MANAGER->killProcess(K_PROC_MANAGER->getCurrentThread()->getProcess()->getPid());
        if (!killed) PANIC("Failed to kill process after Syscall Error!");
        K_SCHEDULER->preempt();
        // never comeback here
    }
    INTERRUPT_HAL->restore(prev);
    if (result.needReschedule) K_SCHEDULER->preempt();
}

void Kernel::handlePageFault(Addr faultAddr)
{
    bool prev = INTERRUPT_HAL->disable();
    bool handled = K_VMM->handlePageFault(faultAddr);
    if (!handled)
    {
        bool killed = K_PROC_MANAGER->killProcess(K_PROC_MANAGER->getCurrentThread()->getProcess()->getPid());
        if (!killed) PANIC("KERNEL PANIC: Failed to kill process after Segfault!");
        // never comeback here
    }
    INTERRUPT_HAL->restore(prev);
    if (K_PROC_MANAGER->getCurrentThread()->getState() == ThreadState::BLOCKED) K_SCHEDULER->preempt();
}
