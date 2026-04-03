#pragma once
#include <iostream>
#include <string>
#include <vector>

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
    CPU,
    INTERRUPT
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
        std::string sourceStr = "";

        // Map Source to String
        switch (source)
        {
        case LogSource::KERNEL:
            sourceStr = "[KERNEL   ]";
            break;
        case LogSource::SCHEDULER:
            sourceStr = "[SCHEDULER]";
            break;
        case LogSource::SYSCALL:
            sourceStr = "[SYSCALL  ]";
            break;
        case LogSource::MMU:
            sourceStr = "[MMU      ]";
            break;
        case LogSource::LOADER:
            sourceStr = "[LOADER   ]";
            break;
        case LogSource::CPU:
            sourceStr = "[CPU      ]";
            break;
        case LogSource::INTERRUPT:
            sourceStr = "[INTERRUPT]";
            break;
        }

        if (level == LogLevel::ERROR)
        {
            std::string msg = sourceStr + " ERROR: " + message;
            this->messages.push_back(msg);
            return;
        }
        if (level == LogLevel::WARNING)
        {
            std::string msg = sourceStr + " WARN: " + message;
            this->messages.push_back(msg);
            return;
        }
        std::string msg = sourceStr + " " + message;
        this->messages.push_back(msg);
    }
    void printAll()
    {
        std::cout << "\nLogs:=======================================\n";
        for (std::size_t i = 0; i < this->messages.size(); ++i)
            std::cout << this->messages[i] << std::endl;
    }

    void clear()
    {
        this->messages.clear();
    }

private:
    Logger() = default;
    ~Logger() = default;
    std::vector<std::string> messages;
};

#define LOG(source, level, msg) Logger::getInstance().log(LogSource::source, LogLevel::level, msg)
#define SHOW_LOGS() Logger::getInstance().printAll()
#define CLEAR_LOGS() Logger::getInstance().clear()