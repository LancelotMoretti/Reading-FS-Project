#include "entry.hpp"
#include <map>

#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_

// Read sector from drive
bool readSector(LPCWSTR drive, uint64_t readPoint, BYTE* sector, uint64_t bytesPerSector);

// Print sector table
void printSectorTable(BYTE sector[]);
void printSectorNum(BYTE sector[], int numByte);

// Convert utf16 byte array to utf8 string
std::wstring byteToWString(std::vector<BYTE> input);
std::string wcharToUtf8(const std::wstring& wstr);

// Print file and folder
void printFileAndFolder(std::vector<Entry> vect);

// Get start point of RDET and SDET
uint64_t rdetStartPoint(BYTE sector[]);
uint64_t sdetStartPoint(BYTE bootSector[], uint64_t cluster);
// Read RDET or SDET
std::vector<Entry> readRDETSDET(LPCWSTR drive, uint64_t readPoint, bool isRDET);

// Convert byte array to integer
int32_t fourBytesToInt(BYTE bytes[], uint64_t start);
uint64_t eightBytesToInt(BYTE bytes[], uint64_t start);
uint64_t nBytesToNum(BYTE entry[], uint64_t start, int numBytes);

// Get VBR start point
int32_t VBRStartPoint(BYTE mbr[]);
// Get MFT start point
uint64_t MFTStartPoint(BYTE vbr[]);
// Read MFT
std::string readSTD_INFO(BYTE sector[], uint64_t stdInfoStart);
bool readATTRIBUTE_LIST(BYTE sector[], uint64_t attributeListStart);
bool readFILE_NAME(BYTE sector[], uint64_t fileNameStart);
std::vector<MFTEntry> readMFT(LPCWSTR drive, uint64_t readPoint);
std::vector<MFTEntry> readNTFSTree(LPCWSTR drive, std::vector<uint64_t> listEntries);

#endif /* UTILITIES_HPP_ */