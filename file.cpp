#include "file.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

Symbol::Symbol(GElf_Sym sym, const char* name)
  : sym_(sym), name_(name)
{
}

std::string Symbol::Name()
{
  return name_;
}

bool Symbol::IsSame(const char* name)
{
  return !strcmp(name, name_.c_str());
}

File::File(std::string file_path)
 : elf_path_(file_path)
{
  e = OpenElf(elf_path_.c_str());
}

File::~File()
{
  CloseElf();
}

Elf* File::OpenElf(const char* path)
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

void File::CloseElf()
{
  if (!e) {
    elf_end(e);
  }
  if (fd_ > 0) {
    ::close(fd_);
  }
}

bool File::SectionAndHeaderByType(int type, Elf_Scn** scn, GElf_Shdr* shdr)
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

bool File::Load()
{
  if (!e) {
    return false;
  }
  if (!gelf_getehdr(e, &file_header_)) {
    return false;
  }

  // Load Section
  Elf_Scn *scn = nullptr;
  while((scn = elf_nextscn(e, scn))) {
    sections_.push_back(scn);
  }
  return true;
}

std::vector<Symbol> File::GetSymbols(int type)
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