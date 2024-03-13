#ifndef NTFS_HPP_
#define NTFS_HPP_
#include "volume.hpp"

class NTFS : public Volume {
public:
    NTFS(std::vector<BYTE>& bootSector, HANDLE volumeHandle);

    std::string GetFileSystemType();
    void ReadAtPosition(uint64_t position);
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
    void ReadAndDisplayFileData(uint64_t mftEntry);
    uint64_t GetFileSize(uint64_t mftEntry);

};

#endif /* NTFS_HPP_ */