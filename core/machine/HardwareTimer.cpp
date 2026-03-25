#include "HardwareTimer.hpp"
#include "Interrupt.hpp"
#include "Logger.hpp"
#include <chrono>

void HardwareTimer::start(int interruptIntervalMS)
{
    struct sigaction sa;
    sa.sa_handler = &Interrupt::timerInterruptHandler;
    sa.sa_flags = SA_RESTART;
    sigfillset(&sa.sa_mask);

    if (sigaction(SIGALRM, &sa, nullptr) == -1)
    {
        LOG(KERNEL, ERROR, "Failed to register SIGALRM handler");
        return;
    }

    struct itimerval timer;
    int sec = interruptIntervalMS / 1000;
    int usec = (interruptIntervalMS % 1000) * 1000;

    timer.it_value.tv_sec = sec;
    timer.it_value.tv_usec = usec;
    timer.it_interval.tv_sec = sec;
    timer.it_interval.tv_usec = usec;

    if (setitimer(ITIMER_REAL, &timer, nullptr) == -1)
        LOG(KERNEL, ERROR, "Failed to start hardware timer");
}

void HardwareTimer::stop()
{
    struct itimerval timer{};
    setitimer(ITIMER_REAL, &timer, nullptr);
}

HardwareTimer::~HardwareTimer()
{
    this->stop();
}
