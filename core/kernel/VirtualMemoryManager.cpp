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

    // if in swap
    if (pte.swapped && !pte.valid)
    {
        LOG(MMU, INFO, "Swap-In VPN " + Utils::toHex(vpn));
        this->ctx->timer.tick(DISK_IO_TIME);
        Addr newPAddr = this->allocateFrame(process->getPid(), vpn);
        std::vector<Byte> buffer(KERNEL_PAGE_SIZE);
        int slot = static_cast<int>(pte.ppn);
        this->ctx->swap->swapIn(buffer, slot);

        // store the swap data back to physical memory
        for (Addr i = 0; i < KERNEL_PAGE_SIZE; ++i)
            this->ctx->cpu.storePhysicalMemory(newPAddr + i, 1, buffer[i]);

        STATS.incSwapIns();

        pte.ppn = newPAddr >> 12;
        pte.valid = true;
        pte.referenced = true;
        pte.dirty = false;
        return true;
    }

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

    // Check if it's a valid heap access
    if (this->handleHeapGrowth(process, faultAddr, vpn)) return true;
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
            Addr paddr = this->allocateFrame(proc->getPid(), vpn);

            PTE& pte = (*proc->getPageTable())[vpn];
            pte.ppn = paddr >> 12;
            pte.valid = true;
            pte.canRead = true;
            pte.canWrite = true;
            pte.referenced = true;
            LOG(MMU, DEBUG, "Stack Page Allocated: " + Utils::toHex(faultAddr));
            return true;
        }
    }

    LOG(MMU, WARNING, "Stack Overflow / Guard Page Hit at " + Utils::toHex(faultAddr));
    return false;
}

bool VirtualMemoryManager::handleHeapGrowth(Process* proc, Addr faultAddr, Addr vpn)
{
    if (faultAddr >= HEAP_START && faultAddr < proc->getProgramBreak())
    {
        this->ctx->timer.tick(MEMORY_ALLOCATION_TIME);
        Addr paddr = this->allocateFrame(proc->getPid(), vpn);

        // fill the new heap frame with pure zeros to prevent security leaks
        for (Addr i = 0; i < KERNEL_PAGE_SIZE; i++)
            this->ctx->cpu.storePhysicalMemory(paddr + i, 1, 0);
        PTE& pte = (*proc->getPageTable())[vpn];
        pte.ppn = paddr >> 12;
        pte.valid = true;
        pte.canRead = true;
        pte.canWrite = true;
        pte.referenced = true;
        LOG(MMU, DEBUG, "Heap Page Allocated: " + Utils::toHex(faultAddr));
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
            this->ctx->timer.tick(MEMORY_ALLOCATION_TIME);
            Addr paddr = this->allocateFrame(proc->getPid(), vpn);
            // Calculate offsets
            Addr pageStartVAddr = vpn * KERNEL_PAGE_SIZE;
            size_t offsetInSegment = pageStartVAddr - seg.vaddr;
            size_t filePos = seg.fileOffset + offsetInSegment;

            std::vector<char> buffer(KERNEL_PAGE_SIZE, 0);
            // Logic: Read from file if within fileSize; otherwise 0 (BSS)
            if (offsetInSegment < seg.fileSize)
            {
                this->ctx->timer.tick(DISK_IO_TIME);

                size_t bytesToRead = std::min((size_t)KERNEL_PAGE_SIZE, seg.fileSize - offsetInSegment);
                std::ifstream file(proc->getName(), std::ios::binary);

                file.seekg(filePos);
                file.read(buffer.data(), bytesToRead);
                STATS.incDiskReads();
            }

            // Write directly to Physical RAM using CPU's debug interface
            for (size_t i = 0; i < KERNEL_PAGE_SIZE; i++)
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

Addr VirtualMemoryManager::allocateFrame(int pid, Addr vpn)
{
    FrameAllocInfo info = this->ctx->pmm.allocateFrame();

    // full, must evict and try again
    if (!info.status)
    {
        LOG(MMU, WARNING, "RAM Full. Evicting...");
        this->evictPage();
        info = this->ctx->pmm.allocateFrame();
    }

    Addr ppn = info.paddr >> 12;
    this->ctx->pmm.registerFrameOwner(ppn, vpn, pid);
    this->ctx->pageReplacementPolicy->onAllocate(ppn);
    STATS.incAllocatedFrames();
    return info.paddr;
}

void VirtualMemoryManager::evictPage()
{
    int found = this->ctx->pageReplacementPolicy->findVictim(this->ctx->pmm.getFrameTable(), this->ctx->processList);

    // cannot find victim frame, something wrong
    if (found == -1) throw std::runtime_error("PANIC: OOM and no victim frame found!");

    Addr victimPPN = static_cast<Addr>(found);
    const FrameInfo& info = this->ctx->pmm.getFrameTable()[victimPPN];
    Process* process = this->ctx->processList[info.ownerPid];
    PTE& pte = (*process->getPageTable())[info.vpn];
    Addr paddr = victimPPN * KERNEL_PAGE_SIZE;

    // read only section or clean page, load from file
    if (!pte.canWrite || (!pte.dirty && !pte.swapped))
    {
        LOG(MMU, INFO, "Evicting CLEAN(CODE) page (Dropping) VPN " + Utils::toHex(info.vpn));
        pte.valid = false;
        pte.swapped = false;
        pte.ppn = 0;
        this->ctx->pmm.freeFrame(paddr);
        this->ctx->pageReplacementPolicy->onFree(victimPPN);
        return;
    }

    // data, stack
    LOG(MMU, INFO, "Evicting DATA page (Swapping) VPN " + Utils::toHex(info.vpn));
    std::vector<Byte> buffer(KERNEL_PAGE_SIZE);

    // load the memory to buffer, and store it in swap
    for (size_t i = 0; i < KERNEL_PAGE_SIZE; ++i)
        buffer[i] = static_cast<Byte>(this->ctx->cpu.loadPhysicalMemory(paddr + i, 1));

    int slot = this->ctx->swap->swapOut(buffer);
    STATS.incSwapOuts();
    if (slot == -1) throw std::runtime_error("Swap Full!");

    pte.valid = false;
    pte.swapped = true;
    pte.ppn = slot;
    pte.dirty = false;
    this->ctx->pmm.freeFrame(paddr);
    this->ctx->pageReplacementPolicy->onFree(victimPPN);
}