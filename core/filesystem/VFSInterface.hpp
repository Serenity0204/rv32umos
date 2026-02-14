#pragma once
#include "FileHandleInterface.hpp"
#include <string>

class VFSInterface
{
public:
    virtual ~VFSInterface() = default;
    virtual FileHandleInterface* open(const std::string& filename) = 0;
    virtual bool createFile(const std::string& filename, std::size_t sizeBytes) = 0;
    virtual bool removeFile(const std::string& filename) = 0;
};