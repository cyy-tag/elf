#include <iostream>
#include "elf_cache.h"


int main(int argc, char**argv)
{
  int pid = atoi(argv[1]);
  const char* lib_name = argv[2];
  const char* symbol = argv[3];
  ElfCache elf;
  std::cout << "find target pid " << pid << " lib_name " \
      << lib_name << " symbol " << symbol << std::endl;
  auto [offset, lib_path] = elf.GetPathAndAddress(pid, lib_name, symbol);
  std::cout << "symbol " << std::hex << symbol << " : " << offset << std::endl;
  std::cout << "lib_name path : " << lib_path << std::endl;
  return 0;
}
