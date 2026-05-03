#pragma once
#include "Host.hpp"
#include "Kernel.hpp"

class RV32UMOS : public HostObject
{
private:
    static Kernel* kernel;

public:
    RV32UMOS() = default;
    ~RV32UMOS() = default;
    RV32UMOS(const RV32UMOS&) = delete;
    RV32UMOS& operator=(const RV32UMOS&) = delete;

    // the thread entry
    static void executionLoop();

    // the OS lifecycle
    static void init();
    static void destroy();
    static void reset();
    static bool loadApplication(const std::string& filename);

public:
    // overload base class
    void start() override;
    void runLoop() override;
};
