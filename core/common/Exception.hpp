#pragma once
#include <exception>
#include <string>

class SyscallException : public std::exception
{
public:
    SyscallException(std::string msg) : message(std::move(msg)) {}
    const char* what() const noexcept override { return this->message.c_str(); }

private:
    std::string message;
};
