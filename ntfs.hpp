#ifndef NTFS_HPP_
#define NTFS_HPP_
#include "drive.hpp"

class NTFS : public Drive {
public:
    NTFS() = default;

private:
    int ReservedSectors;
    int HiddenSectors;
    int TotalSectors;
    int StartOfMFT;
    int StartOfMFTMirr;

};

#endif /* NTFS_HPP_ */