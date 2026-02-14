#pragma once
#include "DiskInterface.hpp"
#include "VFSInterface.hpp"
#include <map>
#include <string>

class StubFileSystem : public VFSInterface
{
public:
    StubFileSystem(DiskInterface* disk);
    ~StubFileSystem() = default;

    FileHandleInterface* open(const std::string& filename) override;
    bool createFile(const std::string& filename, std::size_t sizeBytes) override;
    bool removeFile(const std::string& filename) override;

private:
    struct FileMetadata
    {
        std::string name;
        Addr startBlock;
        std::size_t sizeBytes;
    };

private:
    DiskInterface* disk;
    std::map<std::string, FileMetadata> fileTable;
    uint32_t freeBlockPointer;
};