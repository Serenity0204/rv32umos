#pragma once
#include <atomic>
#include <thread>

struct HostBus
{
public:
    // interrupt lines for future peripheral Actors
    // std::atomic<bool> graphicsInterrupt{false};
    // std::atomic<bool> networkInterrupt{false};
    // std::atomic<bool> keyboardInterrupt{false};

    HostBus() = default;
    ~HostBus() = default;
};

class HostObject
{
protected:
    std::thread workerThread;
    std::atomic<bool> isRunning{false};

public:
    virtual ~HostObject();
    virtual void start();
    virtual void stop();
    void waitForShutdown();

protected:
    virtual void runLoop() = 0;
};