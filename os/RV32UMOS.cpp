#include "RV32UMOS.hpp"
#include "HAL.hpp"
#include "KernelAlias.hpp"
#include "KernelService.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "SystemConfig.hpp"

Kernel* RV32UMOS::kernel = nullptr;
static HAL* hal = nullptr;

void RV32UMOS::initMachine()
{
    // init HAL and devices (no host timer / interrupt devices;
    // preemption is driven synchronously by instruction quantum)
    hal = new HAL();
    Memory* memory = new Memory();
    Machine* cpu = new Machine();
    DiskInterface* disk = new DiskImpl(NUM_DISK_BLOCKS);

    cpu->setMemory(memory);

    // register devices to HAL device map
    hal->registerDevice(memory);
    hal->registerDevice(cpu);
    hal->registerDevice(disk);
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

    LOG(KERNEL, INFO, "rv32umos Booting...");
    LOG(KERNEL, INFO, "Simulation started...");
    K_SCHEDULER->preempt();

    uint64_t ticks = 0;
    while (!K_SCHEDULER->checkAllTerminated())
    {
        if (K_PROC_MANAGER->currentThreadIndex == -1 ||
            K_PROC_MANAGER->getCurrentThread()->getState() != ThreadState::RUNNING)
        {
            K_SCHEDULER->preempt();
            if (K_SCHEDULER->checkAllTerminated()) break;
            if (K_PROC_MANAGER->currentThreadIndex == -1 ||
                K_PROC_MANAGER->getCurrentThread()->getState() != ThreadState::RUNNING)
            {
                LOG(SCHEDULER, ERROR, "Stall: current thread not runnable and no READY thread.");
                break;
            }
            continue;
        }

        Thread* self = K_PROC_MANAGER->getCurrentThread();
        self->getProcess()->incrementInstruction();
        STATS.incInstructions();
        CPU_HAL->step();
        ticks++;

        // traps
        if (CPU_HAL->hasTrap())
        {
            Trap trap = CPU_HAL->getTrap();
            CPU_HAL->clearTrap();

            if (trap.type == TrapType::Syscall)
            {
                RV32UMOS::kernel->handleSyscall(static_cast<SyscallID>(trap.value));
                // Syscall handler already advanced PC as needed.
            }
            else if (trap.type == TrapType::PageFault)
            {
                RV32UMOS::kernel->handlePageFault(trap.value);
                // On success retry same PC (no advance); on failure
                // the process was killed (TERMINATED).
            }

            if (self->getState() != ThreadState::RUNNING)
                K_SCHEDULER->preempt();
            else if (ticks % SCHED_QUANTUM_INSTRUCTIONS == 0)
                K_SCHEDULER->preempt();
            continue;
        }

        // instruction succeeded, advance PC
        CPU_HAL->advancePC();
        if (ticks % SCHED_QUANTUM_INSTRUCTIONS == 0) K_SCHEDULER->preempt();
    }
}
