#include "VirtualMemoryManager.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Utils.hpp"
#include <fstream>

VirtualMemoryManager::VirtualMemoryManager(KernelContext* context) : ctx(context) {}

bool VirtualMemoryManager::handlePageFault(Addr faultAddr)
{
    STATS.incPageFaults();
    Thread* currentThread = this->ctx->getCurrentThread();

    if (currentThread == nullptr)
    {
        this->ctx->cpu.halt();
        return false;
    }

    Process* process = currentThread->getProcess();
    Addr vpn = faultAddr >> 12;
    PTE& pte = (*process->getPageTable())[vpn];

    // faulting when the pte clearly still exists, it's a permission error
    if (pte.valid)
    {
        LOG(KERNEL, ERROR, "Protection Fault: Thread " + std::to_string(currentThread->getTid()) + " tried to access protected " + Utils::toHex(faultAddr));
        return false;
    }

    // if it's under stack limit, allocate one physical page and set the page table entry
    if (this->handleStackGrowth(process, faultAddr, vpn)) return true;

    // check for segments for lazy loading
    if (this->handleLazyLoading(process, faultAddr, vpn)) return true;

    // If it's not stack, it's a real crash (SegFault), return false to let the handler handle it
    LOG(KERNEL, ERROR, "Thread " + std::to_string(currentThread->getTid()) + " (PID " + std::to_string(process->getPid()) + ")" + " causes Segmentation Fault: Invalid access at " + Utils::toHex(faultAddr));
    return false;
}

bool VirtualMemoryManager::handleStackGrowth(Process* proc, Addr faultAddr, Addr vpn)
{
    // not even a stack region
    if (faultAddr < STACK_REGION_BOTTOM || faultAddr >= STACK_TOP) return false;

    for (Thread* t : proc->getThreads())
    {
        Addr stackTop = t->getStackTop();
        size_t stackSize = (t->getTid() == 0) ? MAIN_STACK_SIZE : THREAD_STACK_SIZE;
        Addr stackBottom = stackTop - stackSize;

        if (faultAddr >= stackBottom && faultAddr < stackTop)
        {
            this->ctx->timer.tick(MEMORY_ALLOCATION_TIME);

            Addr paddr = this->ctx->pmm.allocateFrame();

            PTE& pte = (*proc->getPageTable())[vpn];
            pte.ppn = paddr >> 12;
            pte.valid = true;
            pte.canRead = true;
            pte.canWrite = true;
            pte.referenced = true;

            STATS.incAllocatedFrames();
            LOG(MMU, DEBUG, "Stack Page Allocated: " + Utils::toHex(faultAddr));
            return true;
        }
    }

    LOG(MMU, WARNING, "Stack Overflow / Guard Page Hit at " + Utils::toHex(faultAddr));
    return false;
}

bool VirtualMemoryManager::handleLazyLoading(Process* proc, Addr faultAddr, Addr vpn)
{
    for (const Segment& seg : proc->getSegments())
    {
        // if fault within this segment
        if (faultAddr >= seg.vaddr && faultAddr < seg.vaddr + seg.memSize)
        {
            this->ctx->timer.tick(MEMORY_ALLOCATION_TIME);

            Addr paddr = this->ctx->pmm.allocateFrame();
            STATS.incAllocatedFrames();
            // Calculate offsets
            Addr pageStartVAddr = vpn * PAGE_SIZE;
            size_t offsetInSegment = pageStartVAddr - seg.vaddr;
            size_t filePos = seg.fileOffset + offsetInSegment;

            std::vector<char> buffer(PAGE_SIZE, 0);
            // Logic: Read from file if within fileSize; otherwise 0 (BSS)
            if (offsetInSegment < seg.fileSize)
            {
                this->ctx->timer.tick(DISK_IO_TIME);

                size_t bytesToRead = std::min((size_t)PAGE_SIZE, seg.fileSize - offsetInSegment);
                std::ifstream file(proc->getName(), std::ios::binary);

                file.seekg(filePos);
                file.read(buffer.data(), bytesToRead);
                STATS.incDiskReads();
            }

            // Write directly to Physical RAM using CPU's debug interface
            for (size_t i = 0; i < PAGE_SIZE; i++)
                this->ctx->cpu.storePhysicalMemory(paddr + i, 1, static_cast<Word>(buffer[i]));

            // Update PTE Struct
            PTE& pte = (*proc->getPageTable())[vpn];
            pte.ppn = paddr >> 12;
            pte.valid = true;
            pte.referenced = true;
            // Set R/W/X permissions
            PTE::setPTEFlagsFromElf(pte, seg.flags);

            LOG(MMU, DEBUG, "Segment Page Loaded: " + Utils::toHex(pageStartVAddr));
            return true;
        }
    }
    return false;
}