#ifndef NTFS_HPP_
#define NTFS_HPP_
#include "drive.hpp"

class NTFS : public Drive {
public:
    NTFS(std::vector<BYTE>& bootSector, LPCWSTR drive);

    std::string GetFileSystemType();
    void ReadFileAtPosition(uint64_t position);
    void ReturnToStart();
    void ReturnToParent();
    void ViewDriveInformation();
    void ViewFolderTree();

private:
    uint64_t ReservedSectors;
    uint64_t HiddenSectors;
    uint64_t TotalSectors;
    uint64_t StartOfMFT;
    uint64_t StartOfMFTMirr;

    void ReadBootSector(std::vector<BYTE>& buffer);

};

#endif /* NTFS_HPP_ */