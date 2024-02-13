#ifndef FAT32_HPP_
#define FAT32_HPP_
#include "drive.hpp"

class Fat32 : public Drive {
public:
    Fat32() = default;

private:
    LPCWSTR drive;
    int SectorsPerBootSector;
    int NumOfFAT;
    int SizeOfVolume;
    int SectorsPerFAT;
    int StartOfRDET;
    int SizeOfRDET;

    void ReadAndDisplayFileData(int startCluster, int fileSize);
    int GetNextDataCluster(int currentCluster);
    void ReadDataCluster(int cluster, std::vector<BYTE> buffer);
    void ReadDataSector(int sector, std::vector<BYTE> buffer);

};

#endif /* FAT32_HPP_ */