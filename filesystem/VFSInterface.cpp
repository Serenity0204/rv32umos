#include "VFSInterface.hpp"

FileHandleInterface* VFSInterface::open(const std::string& filename)
{
    return this->openImpl(filename);
}
bool VFSInterface::createFile(const std::string& filename, std::size_t sizeBytes)
{
    return this->createFileImpl(filename, sizeBytes);
}
bool VFSInterface::removeFile(const std::string& filename)
{
    return this->removeFileImpl(filename);
}
