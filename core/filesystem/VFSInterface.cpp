#include "VFSInterface.hpp"
#include "KernelInstance.hpp"

FileHandleInterface* VFSInterface::open(const std::string& filename)
{
    kernel.scheduler->sleepCurrentThread(FILE_IO_TIME, "VFS Open");
    return this->openImpl(filename);
}
bool VFSInterface::createFile(const std::string& filename, std::size_t sizeBytes)
{
    kernel.scheduler->sleepCurrentThread(FILE_IO_TIME, "VFS Create");
    return this->createFileImpl(filename, sizeBytes);
}
bool VFSInterface::removeFile(const std::string& filename)
{
    kernel.scheduler->sleepCurrentThread(FILE_IO_TIME, "VFS Remove");
    return this->removeFileImpl(filename);
}