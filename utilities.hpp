#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_
#include <iostream>
#include <Windows.h>
#include <vector>

int ReadSector(LPCWSTR drive, int readPoint, BYTE sector[], int bytesPerSector);

#endif /* UTILITIES_HPP_ */