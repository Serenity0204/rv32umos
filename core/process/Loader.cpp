#include "Loader.hpp"
#include "Kernel.hpp"
#include "KernelAlias.hpp"
#include "Logger.hpp"
#include "RV32UMOS.hpp"
#include "Segment.hpp"
#include "Utils.hpp"
#include <elf.h>
#include <fstream>

int Loader::loadELF(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) return -1;

    // elf header
    Elf32_Ehdr ehdr;
    if (!file.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr))) return -1;

    // Verify Magic: 0x7F 'E' 'L' 'F'
    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3)
    {
        LOG(LOADER, ERROR, "Not a valid ELF file: " + filename);
        return -1;
    }

    int newPid = -1;
    for (size_t i = 0; i < K_PROC_MANAGER->processList.size(); ++i)
    {
        Process* p = K_PROC_MANAGER->processList[i];
        // slot holds a non active process
        if (!p->isActive())
        {
            p->recycle(filename);
            newPid = static_cast<int>(i);
            break;
        }
    }

    // full
    if (newPid == -1) return -1;

    Process* process = K_PROC_MANAGER->processList[newPid];

    // Parse Program Headers (Segments)
    file.seekg(ehdr.e_phoff);

    for (int i = 0; i < ehdr.e_phnum; ++i)
    {
        Elf32_Phdr phdr;
        file.read(reinterpret_cast<char*>(&phdr), sizeof(phdr));
        // only care about LOAD segments
        if (phdr.p_type != PT_LOAD) continue;

        Segment seg;
        seg.vaddr = phdr.p_vaddr;
        seg.memSize = phdr.p_memsz;
        seg.fileSize = phdr.p_filesz;
        seg.fileOffset = phdr.p_offset;
        seg.flags = phdr.p_flags;

        process->getSegments().push_back(seg);

        LOG(LOADER, INFO, "Segment: " + Utils::toHex(seg.vaddr) + " Size: " + std::to_string(seg.memSize) + " Flags: " + std::to_string(seg.flags));
    }

    Thread* mainThread = process->createThread(ehdr.e_entry, 0);
    if (mainThread == nullptr)
    {
        // create main thread fails, kill the process
        process->setActive(false);
        LOG(LOADER, ERROR, "Create process failed " + filename);
        return -1;
    }
    mainThread->setupHostContext(reinterpret_cast<void (*)()>(&RV32UMOS::executionLoop));
    mainThread->setState(ThreadState::READY);

    K_PROC_MANAGER->activeThreads.push_back(mainThread);

    LOG(LOADER, INFO, "Created Process " + std::to_string(newPid) + ": " + filename);
    return newPid;
}
