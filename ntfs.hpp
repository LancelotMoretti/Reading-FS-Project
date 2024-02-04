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
    int LogicalClusterNumberForMFT;
    int LogicalClusterNumberForMFTMirr;
    int ClustersPerMFTRecord;
    int ClustersPerIndexBlock;

};

#endif /* NTFS_HPP_ */