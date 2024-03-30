#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include "elf_file.h"

Symbol::Symbol(GElf_Sym sym, const char* name)
  : sym_(sym), name_(name)
{
}

std::string Symbol::Name() const
{
  return name_;
}

uint64_t Symbol::Value() const
{
  return sym_.st_value;
}

int Symbol::Type() const
{
  return GELF_ST_TYPE(sym_.st_info);
}

bool Symbol::IsSame(const char* name)
{
  return !strcmp(name, name_.c_str());
}

ElfFile::ElfFile(std::string file_path)
 : elf_path_(file_path)
{
  e = OpenElf(elf_path_.c_str());
}

ElfFile::~ElfFile()
{
  CloseElf();
}

Elf* ElfFile::OpenElf(const char* path)
{
  if (elf_version(EV_CURRENT) == EV_NONE) {
    return nullptr;
  }

  fd_ = open(path, O_RDONLY);
  if (fd_ < 0) {
    return nullptr;
  }

  return elf_begin(fd_, ELF_C_READ, nullptr);
}

void ElfFile::CloseElf()
{
  if (!e) {
    elf_end(e);
  }
  if (fd_ > 0) {
    ::close(fd_);
  }
}

bool ElfFile::SectionAndHeaderByType(int type, Elf_Scn** scn, GElf_Shdr* shdr)
{
  for(auto sec : sections_)
  {
    if (!gelf_getshdr(sec, shdr))
      continue;
    if (shdr->sh_type == type) {
      *scn = sec;
      return true;
    }
  }
  return false;
}

bool ElfFile::Load()
{
  if (!e) {
    return false;
  }
  if (!gelf_getehdr(e, &file_header_)) {
    return false;
  }

  // Load section
  Elf_Scn *scn = nullptr;
  while((scn = elf_nextscn(e, scn))) {
    sections_.push_back(scn);
  }
  // Load program header
  size_t nhdrs = 0;
  GElf_Phdr phdr;
  if (file_header_.e_type == ET_EXEC || file_header_.e_type == ET_DYN) {
    if (elf_getphdrnum(e, &nhdrs) != 0) {
      return false;
    }
    for (size_t i = 0; i < nhdrs; i++) {
      if (!gelf_getphdr(e, i, &phdr))
        continue;
      prohdrs_.emplace_back(phdr);
    }
  }
  return true;
}

std::vector<Symbol> ElfFile::GetSymbols(int type)
{
  GElf_Sym sym;
  GElf_Shdr shdr;
  Elf_Scn*  scn = nullptr;
  if (!SectionAndHeaderByType(type, &scn, &shdr)) {
    return {};
  }
  std::vector<Symbol> ret;
  Elf_Data* data = nullptr;
  while ((data = elf_getdata(scn, data))) {
    for(int i = 0; gelf_getsym(data, i, &sym); i++) {
      char * str = elf_strptr(e, shdr.sh_link, sym.st_name);
      if (!str) continue;
      ret.emplace_back(sym, str);
    }
  }
  return ret;
}

std::vector<Symbol> ElfFile::Symbols()
{
  return GetSymbols(SHT_SYMTAB);
}

std::vector<Symbol> ElfFile::DynamicSymbols()
{
  return GetSymbols(SHT_DYNSYM);
}
