#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_
#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <iomanip>

int readSector(LPCWSTR drive, int readPoint, BYTE* sector, int bytesPerSector);
uint64_t rdetStartPoint(BYTE sector[]);
uint64_t sdetStartPoint(BYTE bootSector[], int cluster);
void printFAT32BootSector(BYTE sector[]);

#endif /* UTILITIES_HPP_ */