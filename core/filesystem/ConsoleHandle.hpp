#pragma once
#include "FileHandleInterface.hpp"
#include "Types.hpp"
#include <vector>

class ConsoleHandle : public FileHandleInterface
{
public:
    ConsoleHandle(int fd);
    ~ConsoleHandle() = default;

    int read(std::vector<Byte>& buffer, std::size_t size) override;
    int write(const std::vector<Byte>& buffer, std::size_t size) override;

private:
    int hostFD;
};