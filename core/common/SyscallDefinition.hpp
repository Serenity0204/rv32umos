#pragma once
#include <string>
#include <unordered_map>

enum class SyscallID : int
{
    SYS_EXIT = 0,
    SYS_WRITE = 1,
    SYS_READ = 2,
    SYS_THREAD_CREATE = 3,
    SYS_THREAD_EXIT = 4,
    SYS_UNKNOWN = -1,
};

inline const std::unordered_map<SyscallID, std::string> sysCallNameMap = {
    {SyscallID::SYS_EXIT, "exit syscall"},
    {SyscallID::SYS_WRITE, "write syscall"},
    {SyscallID::SYS_READ, "read syscall"},
    {SyscallID::SYS_THREAD_CREATE, "create thread syscall"},
    {SyscallID::SYS_THREAD_EXIT, "exit thread syscall"},
};
