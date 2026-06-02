#pragma once
#include "Process.hpp"
#include <unordered_map>
#include <vector>

class ProcessManager
{
public:
    // Maps PID -> Exit Code
    std::unordered_map<int, int> exitCodes;
    // Maps PID -> List of Threads waiting for that PID to exit
    std::unordered_map<int, std::vector<Thread*>> processWaiters;

    std::vector<Process*> processList;
    std::vector<Thread*> activeThreads;
    int currentThreadIndex = -1;

public:
    ProcessManager();
    ~ProcessManager();
    Thread* getCurrentThread();
    bool killProcess(int pid);
    bool createProcess(const std::string& filename);
};
