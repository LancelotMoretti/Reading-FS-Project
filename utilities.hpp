#ifndef UTILITIES_HPP_
#define UTILITIES_HPP_
#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>

int ReadSector(LPCWSTR drive, int readPoint, BYTE sector[], int bytesPerSector);
int rdetStartPoint(BYTE sector[]);
int sdetStartPoint(BYTE bootSector[], int cluster);

#endif /* UTILITIES_HPP_ */