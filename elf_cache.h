#pragma once
#include <unordered_map>
#include <memory>
#include "executable.h"


class ElfCache
{
  private:
    std::unordered_map<std::string, std::unique_ptr<Executable>> elf_cache_;
    std::string WhichSoInProcess(std::string, int pid);
  public:
    std::pair<uint64_t, std::string> GetPathAndAddress(int pid, std::string lib_name, std::string symbol);
};
