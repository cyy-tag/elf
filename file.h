#pragma once
#include <gelf.h>
#include <vector>
#include <string>

class Symbol {
  private:
    GElf_Sym sym_;
    std::string name_;
  public:
    Symbol(GElf_Sym sym, const char* name);
    std::string Name();
    bool IsSame(const char* name);
};

class File {
  private:
    std::string elf_path_;
    int fd_;
    Elf * e;
    GElf_Ehdr file_header_;
    std::vector<Elf_Scn *> sections_;
    Elf* OpenElf(const char* path);
    void CloseElf();
    bool SectionAndHeaderByType(int type, Elf_Scn** scn, GElf_Shdr* shdr);
  public:
    File(std::string file_path);
    std::vector<Symbol> GetSymbols(int type);
    ~File();
    bool Load();
};

