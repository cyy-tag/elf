#include <functional>
#include "executable.h"
#include "elf_file.h"

Executable::Executable(std::string path)
  : path_(path)
{

}

const char* Executable::ErrMsg()
{
  return elf_errmsg(errno);
}

bool Executable::LoadSymbol()
{
  ElfFile f(path_);
  if (!f.Load()) return false;

  // cal address of symbol
  std::vector<Symbol> syms = f.Symbols();
  std::vector<Symbol> dynsyms = f.DynamicSymbols();
  auto cal = [&](const std::vector<Symbol>& sym) {
    for(const auto& s : sym) {
      if (s.Type() != STT_FUNC)
        continue;
      uint64_t address = s.Value();

      // calculate symbol offset
      // fn symbol offset = fn symbol VA - .text VA + .text offset
      // stackoverflow.com/a/40249502
      for(const auto& p : f.prohdrs_) {
        if (p.p_type != PT_LOAD || !(p.p_flags & PF_X))
          continue;
        if (p.p_vaddr <= address && address < (p.p_vaddr + p.p_memsz)) {
          address = address - p.p_vaddr + p.p_offset;
          break;
        }
      }
      cache_addresses_[s.Name()] = address;
    }
  };
  cal(syms);
  cal(dynsyms);
  return true;
}

uint64_t Executable::Address(const std::string symbol)
{
  if (cache_addresses_.count(symbol))
    return cache_addresses_[symbol];
  else
    return -1;
}

std::string Executable::Path()
{
  return path_;
}