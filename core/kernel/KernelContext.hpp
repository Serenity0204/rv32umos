#pragma once
#include "CPU.hpp"
#include "DiskInterface.hpp"
#include "InMemoryDisk.hpp"
#include "PageReplacementPolicy.hpp"
#include "PhysicalMemoryManager.hpp"
#include "Process.hpp"
#include "Stats.hpp"
#include "StubFileSystem.hpp"
#include "SwapManager.hpp"
#include "VFSInterface.hpp"
#include <ucontext.h>
#include <vector>

class Kernel;

struct SystemContext
{
public:
    CPU cpu;
    PhysicalMemoryManager pmm;
    std::vector<Process*> processList;

    std::vector<Thread*> activeThreads;
    int currentThreadIndex = -1;
    ucontext_t mainContext;

    SystemContext()
    {
        this->pmm.init();
    }

    ~SystemContext()
    {
        for (auto* p : this->processList) delete p;
        this->processList.clear();
        this->activeThreads.clear();
    }

    inline Thread* getCurrentThread()
    {
        if (currentThreadIndex == -1) return nullptr;
        if (static_cast<std::size_t>(currentThreadIndex) >= activeThreads.size()) return nullptr;

        return this->activeThreads[this->currentThreadIndex];
    }
};

struct StorageContext
{
    VFSInterface* vfs = nullptr;
    DiskInterface* disk = nullptr;
    PageReplacementPolicy* pageReplacementPolicy = nullptr;
    SwapManager* swap = nullptr;

    template <typename VFSImpl, typename DiskImpl, typename PageReplacementPolicyImpl>
    void init(std::size_t numDiskBlocks, std::size_t numSwapBlocks, std::size_t totalFrames)
    {
        static_assert(std::is_base_of<VFSInterface, VFSImpl>::value,
                      "VFSImpl must derive from VFSInterface");
        static_assert(std::is_base_of<DiskInterface, DiskImpl>::value,
                      "DiskImpl must derive from DiskInterface");
        static_assert(std::is_base_of<PageReplacementPolicy, PageReplacementPolicyImpl>::value,
                      "PageReplacementPolicyImpl must derive from PageReplacementPolicy");

        this->disk = new DiskImpl(numDiskBlocks);
        this->vfs = new VFSImpl(this->disk);
        this->pageReplacementPolicy = new PageReplacementPolicyImpl();
        this->pageReplacementPolicy->init(totalFrames);
        this->swap = new SwapManager(this->disk, numDiskBlocks, numSwapBlocks);
    }

    ~StorageContext()
    {
        delete this->swap;
        delete this->pageReplacementPolicy;
        delete this->vfs;
        delete this->disk;
    }
};