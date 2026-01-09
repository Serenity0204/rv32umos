#pragma once
#include <iostream>
#include <mutex>
#include <string>

enum class LogLevel
{
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

enum class LogSource
{
    KERNEL,
    SCHEDULER,
    SYSCALL,
    MMU,
    LOADER,
    CPU
};

class Logger
{
public:
    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    // Main Log Function
    void log(LogSource source, LogLevel level, const std::string& message)
    {
        std::lock_guard<std::mutex> lock(logMutex);

        const std::string RESET = "\033[0m";
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string CYAN = "\033[36m";
        const std::string BLUE = "\033[34m";
        const std::string MAGENTA = "\033[35m";

        std::string sourceStr = "";
        std::string color = RESET;

        switch (source)
        {
        case LogSource::KERNEL:
            sourceStr = "[KERNEL   ]";
            color = GREEN;
            break;
        case LogSource::SCHEDULER:
            sourceStr = "[SCHEDULER]";
            color = CYAN;
            break;
        case LogSource::SYSCALL:
            sourceStr = "[SYSCALL  ]";
            color = BLUE;
            break;
        case LogSource::MMU:
            sourceStr = "[MMU      ]";
            color = MAGENTA;
            break;
        case LogSource::LOADER:
            sourceStr = "[LOADER   ]";
            color = YELLOW;
            break;
        case LogSource::CPU:
            sourceStr = "[CPU      ]";
            color = RESET;
            break;
        }

        if (level == LogLevel::ERROR)
        {
            std::cerr << RED << sourceStr << " ERROR: " << message << RESET << std::endl;
            return;
        }
        if (level == LogLevel::WARNING)
        {
            std::cout << YELLOW << sourceStr << " WARN: " << message << RESET << std::endl;
            return;
        }

        std::cout << color << sourceStr << RESET << " " << message << std::endl;
    }

private:
    Logger() = default;
    std::mutex logMutex;
};

#define LOG(source, level, msg) Logger::getInstance().log(LogSource::source, LogLevel::level, msg)