#include "typedef.hpp"

std::map<int, std::wstring> fat32FlagToMeaning = {
    {0x01, L"Read Only"},
    {0x02, L"Hidden"},
    {0x04, L"System"},
    {0x08, L"Volume Label"},
    {0x10, L"Subdirectory"},
    {0x20, L"Archive"},
    {0x40, L"Device"},
    {0x80, L"Unused"},
};

std::map<int, std::wstring> ntfsFlagToMeaning = {
    {0x0001, L"Read Only"},
    {0x0002, L"Hidden"},
    {0x0004, L"System"},
    {0x0020, L"Archive"},
    {0x0040, L"Device"},
    {0x0080, L"Normal"},
    {0x0100, L"Temporary"},
    {0x0200, L"Sparse File"},
    {0x0400, L"Reparse Point"},
    {0x0800, L"Compressed"},
    {0x1000, L"Offline"},
    {0x2000, L"Not Content Indexed"},
    {0x4000, L"Encrypted"},
    {0x10000000, L"Directory"},
    {0x20000000, L"Index View"},
    {0x40000000, L"Virtual"}
};