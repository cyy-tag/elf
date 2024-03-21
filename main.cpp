#include "file.h"
#include <iostream>


int main()
{
  File elf("/lib/x86_64-linux-gnu/libc.so.6");
  if (!elf.Load()) {
    std::cout << "load failed" << std::endl;
  }
  auto symbols = elf.GetSymbols(SHT_DYNSYM);
  for(auto symbol : symbols) {
    std::cout << "name: " << symbol.Name() << std::endl;
  }
  return 0;
}