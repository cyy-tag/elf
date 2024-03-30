#include <fstream>
#include "elf_cache.h"

std::string ElfCache::WhichSoInProcess(const std::string libname, int pid) {
  std::string path = "/proc/" + std::to_string(pid) + "/maps";
  std::ifstream infile(path);
  if (!infile.is_open()) {
    return "";
  }

  std::string search1 = "/lib" + libname + ".";
  std::string search2 = "/lib" + libname + "-";
  std::string line;
  while(std::getline(infile, line))
  {
    if(line.find(".so") && (line.find(search1) != std::string::npos || \
      line.find(search2) != std::string::npos)) {
        int pos = line.find_first_of('/');
        return line.substr(pos);
    }
  }
  return "";
}

std::pair<uint64_t, std::string> ElfCache::GetPathAndAddress(int pid, std::string lib_name, std::string symbol) {
  std::string key = std::to_string(pid) + "-" + lib_name;
  if (elf_cache_.count(key)) {
    auto &elf_ptr = elf_cache_[key];
    return {elf_ptr->Address(symbol), elf_ptr->Path()};
  }

  // cache elf
  std::string libc_path = WhichSoInProcess("c", pid);
  if (libc_path.empty()) {
    return {-1, ""};
  }
  Executable ex(libc_path);
  if (!ex.LoadSymbol()) {
    return {-1, ""};
  }

  elf_cache_[key] = std::make_unique<Executable>(ex);
  return {ex.Address(symbol), ex.Path()};
}
