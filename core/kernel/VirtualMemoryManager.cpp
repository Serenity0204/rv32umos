#include "VirtualMemoryManager.hpp"
#include "Logger.hpp"
#include "Stats.hpp"
#include "Utils.hpp"
#include <fstream>

VirtualMemoryManager::VirtualMemoryManager(KernelContext* context) : ctx(context) {}

bool VirtualMemoryManager::handlePageFault(Addr faultAddr)
{
    STATS.incPageFaults();

    if (this->ctx->currentProcessIndex == -1)
    {
        this->ctx->cpu.halt();
        return false;
    }

    Process* process = this->ctx->processList[this->ctx->currentProcessIndex];
    Addr vpn = faultAddr >> 12;

    // if it's under stack limit, allocate one physical page and set the page table entry
    if (this->handleStackGrowth(process, faultAddr, vpn)) return true;

    // check for segments for lazy loading
    if (this->handleLazyLoading(process, faultAddr, vpn)) return true;

    // If it's not stack, it's a real crash (SegFault)
    LOG(KERNEL, ERROR, "Segmentation Fault: Invalid access at " + Utils::toHex(faultAddr));
    process->setState(ProcessState::TERMINATED);
    return true;
}

bool VirtualMemoryManager::handleStackGrowth(Process* proc, Addr faultAddr, Addr vpn)
{
    if (faultAddr >= STACK_LIMIT && faultAddr < STACK_TOP)
    {
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
    return false;
}

bool VirtualMemoryManager::handleLazyLoading(Process* proc, Addr faultAddr, Addr vpn)
{
    for (const Segment& seg : proc->getSegments())
    {
        // if fault within this segment
        if (faultAddr >= seg.vaddr && faultAddr < seg.vaddr + seg.memSize)
        {
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