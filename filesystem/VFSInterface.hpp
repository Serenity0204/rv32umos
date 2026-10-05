#pragma once
#include "FileHandleInterface.hpp"
#include <string>

class VFSInterface
{
public:
    virtual ~VFSInterface() = default;

    FileHandleInterface* open(const std::string& filename);
    bool createFile(const std::string& filename, std::size_t sizeBytes);
    bool removeFile(const std::string& filename);

    virtual FileHandleInterface* openImpl(const std::string& filename) = 0;
    virtual bool createFileImpl(const std::string& filename, std::size_t sizeBytes) = 0;
    virtual bool removeFileImpl(const std::string& filename) = 0;
};