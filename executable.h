#pragma once
#include <string>
#include <unordered_map>

class Executable
{
  private:
    // Path of the executable on the filesystem.
    std::string path_;   
    // Parsed ELF and dynamic symbols'cacheAddress.
    std::unordered_map<std::string, uint64_t> cache_addresses_;
  public:
    Executable(std::string path);
    bool LoadSymbol();
    uint64_t Address(const std::string symbol);
    const char* ErrMsg();
    std::string Path();
};
