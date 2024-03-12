#ifndef NTFS_HPP_
#define NTFS_HPP_
#include "volume.hpp"

class NTFS : public Volume {
public:
    NTFS(std::vector<BYTE>& bootSector, LPCWSTR volume);

    std::string GetFileSystemType();
    void ReadFileAtPosition(uint64_t position);
    void ReturnToRoot();
    void ReturnToParent();
    void ViewVolumeInformation();
    void ViewFolderTree();

private:
    uint64_t ReservedSectors;
    uint64_t HiddenSectors;
    uint64_t TotalSectors;
    uint64_t StartOfMFT;
    uint64_t StartOfMFTMirr;

    std::vector<MFTEntry> MFTEntries;

    void ReadBootSector(std::vector<BYTE>& buffer);

};

#endif /* NTFS_HPP_ */