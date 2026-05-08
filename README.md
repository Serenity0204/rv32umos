# rv32umos
> **RISCV-32 User Mode Operating System (rv32umos)** is an educational operating system simulator for studying operating system concepts and implementation. It is inspired by [Nachos](https://en.wikipedia.org/wiki/Not_Another_Completely_Heuristic_Operating_System).


## Overview
**rv32umos** 
is an educational operating system simulator that bridges the gap between operating systems theory and hardware emulation. Implemented in C++, it runs as a user-space process on a host operating system while providing a realistic environment for exploring core OS concepts.

rv32umos includes various implementations of fundamental operating system components, such as:
* A 32-bit RISC-V virtual machine capable of executing unmodified RISC-V ELF binaries
* A virtual memory subsystem with demand paging and swapping
* A preemptive multithreading scheduler
* Synchronization primitives for concurrent programming
* A virtual file system interface and FAT like filesystem implementation
* A custom system call ABI for user-program interaction

  

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
**Adding custom C code**
add your file under `programs/`, name it `test<test number>.c`

**Run**
- Without Logs
```bash
./rv32umos programs/<your_file1.elf> programs/<your_file2.elf> ...
```
- With Logs
```bash
./rv32umos programs/<your_file1.elf> programs/<your_file2.elf> ... -logs
```

## Example Output(Log)
```
Hello From Meow!
───────────────────────────────────────
───▐▀▄───────▄▀▌───▄▄▄▄▄▄▄─────────────
───▌▒▒▀▄▄▄▄▄▀▒▒▐▄▀▀▒██▒██▒▀▀▄──────────
──▐▒▒▒▒▀▒▀▒▀▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▀▄────────
──▌▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▄▒▒▒▒▒▒▒▒▒▒▒▒▀▄──────
▀█▒▒▒█▌▒▒█▒▒▐█▒▒▒▀▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▌─────
▀▌▒▒▒▒▒▒▀▒▀▒▒▒▒▒▒▀▀▒▒▒▒▒▒▒▒▒▒▒▒▒▒▐───▄▄
▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▌▄█▒█
▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█▒█▀─
▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒█▀───
▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▌────
─▌▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▐─────
─▐▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▌─────
──▌▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▐──────
──▐▄▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▄▌──────
────▀▄▄▀▀▀▀▀▄▄▀▀▀▀▀▀▀▄▄▀▀▀▀▀▄▄▀────────

Logs:=======================================
[LOADER   ] Segment: 0x0 Size: 3917 Flags: 5
[LOADER   ] Segment: 0x0 Size: 0 Flags: 6
[LOADER   ] Created Process 0: programs/test3.elf
[KERNEL   ] Simulation started...
[SCHEDULER] Switching to Thread 0 (PID 0)
[MMU      ] Segment Page Loaded: 0x0
[MMU      ] Thread 0 BLOCKED for Lazy Load (10ms)
[SCHEDULER] System IDLE: All threads blocked. Waiting for interrupts...
[MMU      ] Lazy Load Complete: Waking up Thread 0
[MMU      ] Stack Page Allocated: 0x3fffffc
[SYSCALL  ] Write called by PID 0
[SYSCALL  ] Write called by PID 0
[SYSCALL  ] Write called by PID 0
[INTERRUPT] Timer interrupt fired.
[INTERRUPT] Timer interrupt fired.
[INTERRUPT] Timer interrupt fired.
[INTERRUPT] Timer interrupt fired.
[INTERRUPT] Timer interrupt fired.
[SYSCALL  ] Write called by PID 0
[KERNEL   ] Process 0 (Group Exit) terminated with code 67
[SCHEDULER] All threads terminated.

============================================
            MACHINE STATISTICS              
============================================
Total Instructions       :      15593
Total Syscalls           :          5
Total Page Faults        :          2
Total Page Swap Ins      :          0
Total Page Swap Outs     :          0
Context Switches         :          1
Disk Reads (4KB)         :          1
Disk Writes (4KB)        :          0
Physical Frames Used     :          2
--------------------------------------------
 Paging Rate: 0.0128% (Faults/Instr)
============================================

```


## Todo
- [X] Multi-Threading
- [ ] Synchronization
- [ ] IPC
- [X] Swap and Page Eviction
- [X] Virtual Disk
- [X] More Syscalls
- [X] Filesystem
- [X] Configurable Scheduling/Swap Policies
- [ ] Add SDL2
- [ ] Add Host networking