#include "Host.hpp"
HostObject::~HostObject() { this->stop(); }

void HostObject::start()
{
    if (!this->isRunning)
    {
        this->isRunning = true;
        this->workerThread = std::thread(&HostObject::runLoop, this);
    }
}

void HostObject::stop()
{
    if (this->isRunning)
    {
        this->isRunning = false;
        if (this->workerThread.joinable()) this->workerThread.join();
    }
}

void HostObject::waitForShutdown()
{
    if (this->workerThread.joinable()) this->workerThread.join();
}
