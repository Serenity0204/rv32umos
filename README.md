# rv32sysemu
> A RISC-V 32-bit system emulator integrated with a multitasking kernel.

This project implements a custom 32-bit RISC-V system emulator in C++, featuring a multitasking kernel with virtual memory management, demand paging, and a preemptive scheduler.

## Overview

The emulator simulates a full computer architecture stack—from the CPU and MMU to the System Bus and RAM. It runs a custom Kernel capable of loading multiple binary programs, managing them as processes, and executing them concurrently using a Round-Robin scheduler. It is designed to demonstrate low-level OS concepts like context switching, page fault handling, and system calls.

## Features

- **RV32I Core Emulation**
  - Implements the base RISC-V 32-bit Integer instruction set with Multiplication and Division extension.

- **Multitasking Kernel**
  - **Process Control**: Manages Process Control Blocks (PCBs) with states (`NEW`, `READY`, `RUNNING`, `TERMINATED`).
  - **Scheduler**: Implements Round-Robin scheduling with a configurable time quantum.
  - **Context Switching**: Saves and restores full hardware context (registers, PC, page tables) between processes.

- **Memory Management (MMU)**
  - **Virtual Memory**: Per-process isolation using page tables.
  - **Lazy Loading**: Stack memory grows dynamically as the process requires it.

- **System Calls**
  - `SYS_WRITE`: Handles output to stdout/stderr.
  - `SYS_READ`: Basic input handling from stdin.
  - `SYS_EXIT`: Manages process termination and exit codes.

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
./rv32sysemu programs/<your_file.bin>
```

## Example Output
```
rv32sysemu entry:
[LOADER   ] Created Process 1: programs/test2.bin
[LOADER   ] Created Process 2: programs/test2.bin
[KERNEL   ] Simulation started...
[MMU      ] Stack expanded for PID 1
[SCHEDULER] Switching to PID 2
[MMU      ] Stack expanded for PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[SCHEDULER] Switching to PID 2
[SCHEDULER] Switching to PID 1
[KERNEL   ] Process 1 exited with code 59.
[SCHEDULER] Switching to PID 2
[KERNEL   ] Process 2 exited with code 59.
[SCHEDULER] All processes terminated.

============================================
            MACHINE STATISTICS
============================================
Total Instructions       :       2236
Total Syscalls           :          2
Total Page Faults        :          2
Context Switches         :         23
Disk Reads (4KB)         :          2
Physical Frames Used     :          2
--------------------------------------------
 Paging Rate: 0.0894% (Faults/Instr)
============================================
```