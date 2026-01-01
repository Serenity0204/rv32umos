#pragma once
#include "Syscall.hpp"
#include <exception>
#include <string>

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
