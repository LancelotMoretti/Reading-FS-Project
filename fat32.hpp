#ifndef FAT32_HPP_
#define FAT32_HPP_
#include "volume.hpp"

class Fat32 : public Volume {
public:
    Fat32(std::vector<BYTE>& bootSector, HANDLE volumeHandle);

    // Inherited methods
    std::string GetFileSystemType();
    void ReadFileAtPosition(uint64_t position);
    void ReturnToRoot();
    void ReturnToParent();
    void ViewVolumeInformation();
    void ViewFolderTree();

    ~Fat32() = default;

private:
    // Fat32 specific
    uint64_t SectorsPerBootSector;
    uint64_t NumOfFAT;
    uint64_t SizeOfVolume;
    uint64_t SectorsPerFAT;
    uint64_t StartOfRDET;
    
    // List of entries
    // Last vector is the entries of current directory
    std::vector<std::vector<Entry>> Entries;

    // Methods for processing FAT32
    void ReadBootSector(std::vector<BYTE>& bootSector);
    void ReadAndDisplayFileData(uint64_t startCluster, uint64_t fileSize);
    uint64_t GetNextFATCluster(uint64_t currentCluster);
    uint64_t GetDataCluster(uint64_t cluster);
    void ReadDataCluster(uint64_t cluster, std::vector<BYTE>& buffer);
};

#endif /* FAT32_HPP_ */