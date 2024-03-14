#ifndef TYPEDEF_HPP_
#define TYPEDEF_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <cstdio>
#include <vector>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <locale>
#include <sstream>
#include <codecvt>
#include <map>
#include <io.h>
#include <fcntl.h>

// Define the meaning of flag of FAT32
extern std::map<int, std::wstring> fat32FlagToMeaning;

// Define the meaning of flag of NTFS
extern std::map<int, std::wstring> ntfsFlagToMeaning;

#endif /* TYPEDEF_HPP_ */