#include "Loader.hpp"
#include "Logger.hpp"
#include "Utils.hpp"
#include <elf.h>
#include <fstream>

Loader::Loader(KernelContext* context) : ctx(context) {}

bool Loader::loadELF(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file) return false;

    // elf header
    Elf32_Ehdr ehdr;
    if (!file.read(reinterpret_cast<char*>(&ehdr), sizeof(ehdr))) return false;

    // Verify Magic: 0x7F 'E' 'L' 'F'
    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
        ehdr.e_ident[EI_MAG2] != ELFMAG2 || ehdr.e_ident[EI_MAG3] != ELFMAG3)
    {
        LOG(LOADER, ERROR, "Not a valid ELF file: " + filename);
        return false;
    }

    // if already max processes, fail the creation
    if (this->ctx->processList.size() == MAX_PROCESS) return false;

    int newPid = this->ctx->processList.size() + 1;
    Process* process = new Process(newPid, filename);

    // set PC entry for that process
    process->setPC(ehdr.e_entry);

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

    this->ctx->processList.push_back(process);
    LOG(LOADER, INFO, "Created Process " + std::to_string(newPid) + ": " + filename);
    return true;
}
