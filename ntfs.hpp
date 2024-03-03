#ifndef NTFS_HPP_
#define NTFS_HPP_
#include "drive.hpp"

class NTFS : public Drive {
public:
    NTFS() = default;
    void ReadBootSector(std::vector<BYTE>& buffer);

private:
    LPCWSTR drive;
    int ReservedSectors;
    int HiddenSectors;
    int TotalSectors;
    int StartOfMFT;
    int StartOfMFTMirr;

};

#endif /* NTFS_HPP_ */