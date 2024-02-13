#ifndef DRIVE_HPP_
#define DRIVE_HPP_
#include "utilities.hpp"

class Drive {
public:
    Drive() = default;

protected:
    int BytesPerSector;
    int SectorsPerCluster;
    int SectorsPerTrack;
    int NumOfHeads;

    int NextReadingOffset;

};

#endif /* DRIVE_HPP_ */