#pragma once
#include "Common.hpp"
#include "Process.hpp"
#include <cstddef>
#include <queue>
#include <vector>

struct FrameInfo
{
public:
    int ownerPid;
    Addr vpn;
    bool allocated;

    void clear()
    {
        this->ownerPid = -1;
        this->vpn = 0;
        this->allocated = false;
    }
};

class PageReplacementPolicy
{
public:
    virtual ~PageReplacementPolicy() = default;
    virtual void init(std::size_t) {}
    virtual void onAllocate(std::size_t) {}
    virtual void onFree(std::size_t) {}
    virtual int findVictim(const std::vector<FrameInfo>&, const std::vector<Process*>&) = 0;
};

class FIFOPolicy : public PageReplacementPolicy
{
public:
    void onAllocate(size_t ppn) override;
    int findVictim(const std::vector<FrameInfo>&, const std::vector<Process*>&) override;

private:
    std::queue<std::size_t> queue;
};
