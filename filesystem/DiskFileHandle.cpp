#include "DiskFileHandle.hpp"
#include <algorithm>

DiskFileHandle::DiskFileHandle(DiskInterface* disk, std::size_t start, std::size_t size)
    : disk(disk), startBlockIndex(start), fileSize(size), cursor(0)
{
}

int DiskFileHandle::read(std::vector<Byte>& buffer, std::size_t size)
{
    if (this->eof()) return 0;
    std::size_t toRead = size;

    // truncate
    if (this->cursor + toRead > this->fileSize) toRead = this->fileSize - this->cursor;
    buffer.resize(toRead);

    std::size_t bytesRead = 0;
    std::vector<Byte> temp(BLOCK_SIZE);
    while (bytesRead < toRead)
    {
        Addr relativeBlock = this->cursor / BLOCK_SIZE;
        Addr absoluteBlock = this->startBlockIndex + relativeBlock;
        Addr offsetInBlock = this->cursor % BLOCK_SIZE;

        this->disk->readBlock(absoluteBlock, temp);

        std::size_t bytesLeftInBlock = BLOCK_SIZE - offsetInBlock;
        std::size_t remainingNeeded = toRead - bytesRead;
        std::size_t chunk = std::min(bytesLeftInBlock, remainingNeeded);

        std::copy(temp.begin() + offsetInBlock, temp.begin() + offsetInBlock + chunk, buffer.begin() + bytesRead);

        this->cursor += chunk;
        bytesRead += chunk;
    }
    return bytesRead;
}

int DiskFileHandle::write(const std::vector<Byte>& buffer, std::size_t size)
{
    if (this->eof()) return 0;
    std::size_t toWrite = size;

    // truncate
    if (this->cursor + toWrite > this->fileSize) toWrite = this->fileSize - this->cursor;
    std::size_t bytesWritten = 0;
    std::vector<Byte> temp(BLOCK_SIZE);
    while (bytesWritten < toWrite)
    {
        Addr relativeBlock = this->cursor / BLOCK_SIZE;
        Addr absoluteBlock = this->startBlockIndex + relativeBlock;
        Addr offsetInBlock = this->cursor % BLOCK_SIZE;

        // read the block, overwrite the portion, then write back
        this->disk->readBlock(absoluteBlock, temp);
        std::size_t bytesLeftInBlock = BLOCK_SIZE - offsetInBlock;
        std::size_t remainingNeeded = toWrite - bytesWritten;
        std::size_t chunk = std::min(bytesLeftInBlock, remainingNeeded);

        std::copy(buffer.begin() + bytesWritten, buffer.begin() + bytesWritten + chunk, temp.begin() + offsetInBlock);
        this->disk->writeBlock(absoluteBlock, temp);
        this->cursor += chunk;
        bytesWritten += chunk;
    }
    return bytesWritten;
}

int DiskFileHandle::seek(int offset)
{
    this->cursor += offset;
    return static_cast<int>(this->cursor);
}

int DiskFileHandle::reset()
{
    this->cursor = 0;
    return this->cursor;
}

bool DiskFileHandle::eof()
{
    if (this->cursor >= this->fileSize) return true;
    return false;
}