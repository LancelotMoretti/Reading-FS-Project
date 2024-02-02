#ifndef FAT32_HPP_
#define FAT32_HPP_
#include "drive.hpp"

class Fat32 : public Drive {
public:
    Fat32() = default;

private:
    int SectorPerCluster;
    //...
};

#endif /* FAT32_HPP_ */