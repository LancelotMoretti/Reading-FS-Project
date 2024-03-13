#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_

#include "entry.hpp"

// Read sector from volume
bool readSector(HANDLE devive, uint64_t readPoint, BYTE* sector, uint64_t bytesPerSector);

// Print sector table
void printSectorNum(BYTE sector[], int numByte);

// Print file and folder
void printFileAndFolder(std::vector<Entry> vect);

// Get start point of RDET and SDET
uint64_t rdetStartPoint(BYTE sector[]);
uint64_t sdetStartPoint(BYTE bootSector[], uint64_t cluster);

// Read RDET or SDET
std::vector<Entry> readRDETSDET(HANDLE device, uint64_t readPoint, bool isRDET);

// Convert byte array to integer
uint64_t nBytesToNum(BYTE entry[], uint64_t start, int numBytes);

// Get VBR start point
uint64_t VBRStartPoint(BYTE mbr[]);

// Get MFT start point
uint64_t MFTStartPoint(BYTE vbr[]);

// Read MFT
std::pair<std::wstring, std::pair<std::wstring, std::wstring>> readSTD_INFO(BYTE sector[], uint64_t offSTD);
std::pair<bool, std::wstring> readFILE_NAME(BYTE sector[], uint64_t offFileName);
void formatListEntries(std::vector<uint64_t> list);
std::vector<uint64_t> readFolder(HANDLE device, uint64_t readPoint);
std::vector<MFTEntry> readNTFSTree(HANDLE device, uint64_t start, std::vector<uint64_t> listEntries);

// Print NTFS tree
void printFileAndFolderNTFS(std::vector<MFTEntry> list);
void printFileAndFolderNoHidden(std::vector<MFTEntry> list);

#endif /* UTILITIES_HPP_ */