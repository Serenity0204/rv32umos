#pragma once
#include "DiskInterface.hpp"
#include "FileHandleInterface.hpp"
#include "Types.hpp"
#include <vector>

class DiskFileHandle : public FileHandleInterface
{
public:
    DiskFileHandle(DiskInterface* disk, std::size_t startBlock, std::size_t sizeBytes);
    ~DiskFileHandle() = default;

    int read(std::vector<Byte>& buffer, std::size_t size) override;
    int write(const std::vector<Byte>& buffer, std::size_t size) override;
    int seek(int offset);
    int reset();
    bool eof();

private:
    DiskInterface* disk;
    std::size_t startBlockIndex;
    std::size_t fileSize;
    Addr cursor;
};