#pragma once
#include <exception>
#include <string>

enum class SyscallID : int
{
    SYS_EXIT = 0,
    SYS_WRITE = 1,
    SYS_UNKNOWN = -1,
};

enum class FileDescriptor : int
{
    STDIN = 0,
    STDOUT = 1,
    STDERR = 2
};

class SyscallException : public std::exception
{
public:
    SyscallException(std::string msg, SyscallID syscallID) : message(std::move(msg)), syscallID(syscallID) {}
    const char* what() const noexcept override { return this->message.c_str(); }
    SyscallID getSyscallID() { return this->syscallID; }

private:
    std::string message;
    SyscallID syscallID;
};
