#include "RV32UMOS.hpp"
#include "HAL.hpp"
#include "Interrupt.hpp"
#include "KernelAlias.hpp"
#include "KernelPanic.hpp"
#include "KernelService.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "SystemConfig.hpp"

Kernel* RV32UMOS::kernel = nullptr;
static HAL* hal = nullptr;

void RV32UMOS::initMachine()
{
    // init HAL and devices
    hal = new HAL();
    Memory* memory = new Memory();
    Machine* cpu = new Machine();
    HardwareTimer* timer = new HardwareTimer();
    DiskInterface* disk = new DiskImpl(NUM_DISK_BLOCKS);
    Interrupt* interrupt = new Interrupt();

    cpu->setMemory(memory);

    // register devices to HAL device map
    hal->registerDevice(memory);
    hal->registerDevice(cpu);
    hal->registerDevice(timer);
    hal->registerDevice(disk);
    hal->registerDevice(interrupt);
}

void RV32UMOS::init()
{
    RV32UMOS::initMachine();
    RV32UMOS::kernel = new Kernel();
    Kernel::initKernelSubsystem(RV32UMOS::kernel, hal);
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

    CPU_HAL->enableVM(true);

    INTERRUPT_HAL->enable();
    Interrupt::init(K_SCHEDULER);
    INTERRUPT_HAL->disable();

    TIMER_HAL->start(TIMER_INTERRUPT_FREQUENCY);
    LOG(KERNEL, INFO, "rv32umos Booting...");
    K_SCHEDULER->preempt();
    TIMER_HAL->stop();
}

void RV32UMOS::runThread()
{
    INTERRUPT_HAL->enable();

    while (true)
    {
        // atomic check
        bool prev = INTERRUPT_HAL->disable();
        Thread* self = K_PROC_MANAGER->getCurrentThread();
        self->getProcess()->incrementInstruction();
        if (self->getState() == ThreadState::TERMINATED)
        {
            INTERRUPT_HAL->restore(prev);
            K_SCHEDULER->preempt();
        }

        STATS.incInstructions();
        CPU_HAL->step();

        // traps
        if (CPU_HAL->hasTrap())
        {
            Trap trap = CPU_HAL->getTrap();
            CPU_HAL->clearTrap();

            if (trap.type == TrapType::Syscall)
            {
                RV32UMOS::kernel->handleSyscall(static_cast<SyscallID>(trap.value));
                continue;
            }
            if (trap.type == TrapType::PageFault)
            {
                RV32UMOS::kernel->handlePageFault(trap.value);
                continue;
            }
            continue;
        }

        // instruction succeeded, advance PC
        CPU_HAL->advancePC();
    }
    PANIC("Unexpected error");
}