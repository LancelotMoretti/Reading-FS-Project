#ifndef NTFS_HPP_
#define NTFS_HPP_
#include "drive.hpp"

class NTFS : public Drive {
public:
    NTFS() = default;

private:
    int BytesPerSector;
    //...
};

#endif /* NTFS_HPP_ */