#pragma once
#include <vector>
#include <string>
#include <gelf.h>

class Symbol {
  private:
    GElf_Sym sym_;
    std::string name_;
  public:
    Symbol(GElf_Sym sym, const char* name);
    std::string Name() const;
    uint64_t Value() const;
    int Type() const;
    bool IsSame(const char* name);
};

class ElfFile {
  private:
    std::string elf_path_;
    int fd_;
    Elf * e;
    GElf_Ehdr file_header_;
    std::vector<Elf_Scn *> sections_;
    Elf* OpenElf(const char* path);
    void CloseElf();
    bool SectionAndHeaderByType(int type, Elf_Scn** scn, GElf_Shdr* shdr);
    std::vector<Symbol> GetSymbols(int type);
  public:
    std::vector<GElf_Phdr> prohdrs_;
    ElfFile(std::string file_path);
    ~ElfFile();
    bool Load();
    std::vector<Symbol> Symbols();
    std::vector<Symbol> DynamicSymbols();
};

