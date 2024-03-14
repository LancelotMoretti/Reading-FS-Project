#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_

#include "entry.hpp"

// Read sector from volume
bool readMultiSector(HANDLE devive, uint64_t readPoint, BYTE* sector, uint64_t bytesToRead);

// Print sector table
void printMultiSector(BYTE* sector, int numByte);

// Print FAT32 tree
void printFolderTreeFat32(std::vector<FATEntry> list);

// Get start point of RDET and SDET
uint64_t rdetStartPoint(BYTE* bootSector);
uint64_t sdetStartPoint(BYTE* bootSector, uint64_t cluster);

// Read RDET or SDET
std::vector<FATEntry> readRDETSDET(HANDLE device, uint64_t readPoint, bool isRDET);

// Convert byte array to integer
uint64_t nBytesToNum(BYTE* buffer, uint64_t start, int numBytes);

// Get MFT start point
uint64_t MFTStartPoint(BYTE* bootSector);

// Read MFT
// Read attribute $STANDARD_INFORMATION
std::pair<std::wstring, std::pair<std::wstring, std::wstring>> readSTD_INFO(BYTE* sector, uint64_t offSTD);
// Read attribute $FILE_NAME
std::pair<bool, std::wstring> readFILE_NAME(BYTE* sector, uint64_t offFileName);
// Delete duplicate entries
void formatListEntries(std::vector<uint64_t>& list);
// Get file size
uint64_t getFileSize(HANDLE device, uint64_t start, uint64_t bytePersect, uint64_t mftEntry);

// Read folder entry
// Get list of entry indexes
std::vector<uint64_t> readFolderEntry(HANDLE device, uint64_t readPoint);
// Get list of MFT entries
std::vector<MFTEntry> readNTFSTree(HANDLE device, uint64_t start, std::vector<uint64_t> listEntries);

// Print NTFS tree
void printFolderTreeNTFS(std::vector<MFTEntry> list);
void printNonHiddenFolderTreeNTFS(std::vector<MFTEntry> list);

#endif /* UTILITIES_HPP_ */