# rv32kernel
> A RISC-V OS kernel running on a custom RISC-V architectural simulator.

## Overview
rv32kernel is an OS kernel that runs on a custom 32-bit RISC-V architectural simulator in C++, featuring virtual memory with page faults, preemptive scheduling, and custom syscall ABI. It serves an educational purpose that it tries to bridge the gap between hardware emulation and OS theory.

## Architecture
The system operates on a Sequential execution model with Interrupt Polling. This ensures deterministic behavior and allows for precise debugging of race conditions and scheduling logic.
- **User Space**: Runs unmodified RISC-V ELF binaries. Instructions are fetched, decoded, and executed by the emulated CPU.
- **Kernel Space (Host)**: The OS logic (Scheduler, VMM, Syscalls) runs as native C++ code.
- **Hardware Interface**: Interrupts are simulated by checking a Hardware Timer flag at the instruction boundary, providing implicit atomicity for kernel operations.


## Features

- **The Core(CPU)**
  - **RV32IM Architecture**: Implements the base RISC-V 32-bit Integer instruction set with Multiplication and Division extension.
  - **Cost Modeling**: Every instruction and system event (trap, context switch, memory alloc) is charged a specific "Time Cost" to simulate hardware latency.
  
- **Multitasking**
  - **Process Control**: Manages Process Control Blocks (PCBs) with states (`NEW`, `READY`, `RUNNING`, `TERMINATED`).
  - **Scheduler**: Implements Round-Robin scheduling with a configurable time quantum.
  - **Context Switching**: Saves and restores full hardware context (registers, PC, page tables) between processes.

- **Memory Management**
  - **Virtual Memory**: Per-process isolation using page tables.
  - **Lazy Loading**: Stack memory grows dynamically as the process requires it.
  - **Protection**: Enforces Read/Write/Execute permissions defined in the ELF headers.

- **System Calls**
  - `SYS_WRITE`: Handles output to stdout/stderr.
  - `SYS_READ`: Basic input handling from stdin.
  - `SYS_EXIT`: Manages process termination and exit codes.
  - `SYS_THREAD_CREATE`: Creates a thread object.
  - `SYS_THREAD_EXIT`: Exit the current thread.
  - `SYS_THREAD_JOIN`: Join one thread with another thread
  - `SYS_MUTEX_CREATE`: Create a mutex lock for this process
  - `SYS_MUTEX_LOCK`: Try to acquire the lock
  - `SYS_MUTEX_UNLOCK`: Try to release the lock
## [Detailed Wiki](https://deepwiki.com/Serenity0204/rv32sysemu)

## Toolchain Requirements
1. Install the riscv toolchain
  ```bash
  gcc-riscv64-unknown-elf binutils-riscv64-unknown-elf
  ```
2. CMake
3. g++

## Usage

**Compile project and C code**
```bash
bash build.sh
```
**Clean**
```bash
bash clean.sh
```
**Run**
```bash
./rv32kernel programs/<your_file.bin>
```

## Example Output(Log)
```
[LOADER   ] Segment: 0x0 Size: 952 Flags: 5
[LOADER   ] Segment: 0x0 Size: 0 Flags: 6
[LOADER   ] Created Process 1: programs/test1.elf
[LOADER   ] Segment: 0x0 Size: 952 Flags: 5
[LOADER   ] Segment: 0x0 Size: 0 Flags: 6
[LOADER   ] Created Process 2: programs/test1.elf
[SCHEDULER] Switching to PID 1
[KERNEL   ] Simulation started...
[MMU      ] Segment Page Loaded: 0x0
[KERNEL   ] Timer Interrupt.
[SCHEDULER] Switching to PID 2
[MMU      ] Segment Page Loaded: 0x0
[KERNEL   ] Timer Interrupt.
[SCHEDULER] Switching to PID 1
[MMU      ] Stack Page Allocated: 0x7fffffc
[KERNEL   ] Process 1 exited with code 120
[SCHEDULER] Switching to PID 2
[MMU      ] Stack Page Allocated: 0x7fffffc
[KERNEL   ] Process 2 exited with code 120
[SCHEDULER] All processes terminated.

============================================
            MACHINE STATISTICS
============================================
Total Instructions       :        476
Total Syscalls           :          2
Total Page Faults        :          4
Context Switches         :          4
Disk Reads (4KB)         :          2
Physical Frames Used     :          4
--------------------------------------------
 Paging Rate: 0.8403% (Faults/Instr)
============================================
```

## Todo
- [ ] Multi-Threading
- [ ] Synchronization
- [ ] Advanced Paging(Copy-on-Write and Shared Memory)
- [ ] Swap and Page Eviction
- [ ] Virtual Disk
- [ ] More Syscalls
- [ ] Filesystem
- [ ] Configurable Scheduling/Swap Policies
- [ ] Block Caching the Instructions for Simulator

