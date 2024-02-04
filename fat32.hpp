#ifndef FAT32_HPP_
#define FAT32_HPP_
#include "drive.hpp"

class Fat32 : public Drive {
public:
    Fat32() = default;

private:
    int SectorsPerBootSector;
    int NumOfFAT;
    int SizeOfVolume;
    int SectorsPerFAT;
    int StartOfRDET;
    int SizeOfRDET;

};

#endif /* FAT32_HPP_ */