#ifndef NTFS_HPP_
#define NTFS_HPP_

#include "volume.hpp"

class NTFS : public Volume {
private:
    // NTFS specific
    uint64_t ReservedSectors;
    uint64_t HiddenSectors;
    uint64_t TotalSectors;
    uint64_t StartOfMFT;
    uint64_t StartOfMFTMirr;

    // List of entries in current directory
    std::vector<MFTEntry> MFTEntries;

    // Methods for processing NTFS
    void ReadBootSector(std::vector<BYTE>& buffer);
    void ReadAndDisplayFileData(uint64_t mftEntry);
    uint64_t GetFileSize(uint64_t mftEntry);

public:
    // Constructor
    NTFS(std::vector<BYTE>& bootSector, HANDLE volumeHandle);

    // Inherited methods
    std::string GetFileSystemType();
    void ReadAtPosition(uint64_t position);
    void ReturnToRoot();
    void ReturnToParent();
    void ViewVolumeInformation();
    void ViewFolderTree();

    // Destructor
    ~NTFS() = default;
};

#endif /* NTFS_HPP_ */