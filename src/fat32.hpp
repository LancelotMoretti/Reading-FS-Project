#ifndef FAT32_HPP_
#define FAT32_HPP_

#include "volume.hpp"

class Fat32 : public Volume {
private:
    // Fat32 specific
    uint64_t SectorsPerBootSector;
    uint64_t NumOfFAT;
    uint64_t SizeOfVolume;
    uint64_t SectorsPerFAT;
    uint64_t StartOfRDET;
    
    // List of entries
    // Last vector is the entries of current directory
    std::vector<std::vector<FATEntry>> Entries;

    // Methods for processing FAT32
    void ReadBootSector(std::vector<BYTE>& bootSector);
    void ReadAndDisplayFileData(uint64_t startCLusterFATPos, uint64_t fileSize);
    uint64_t GetNextClusterPos(uint64_t curClusterFATPos);
    uint64_t ToDataCluster(uint64_t cluster);
    void ReadDataCluster(uint64_t cluster, std::vector<BYTE>& buffer);

public:
    // Constructor
    Fat32(std::vector<BYTE>& bootSector, HANDLE volumeHandle);

    // Inherited methods
    std::string GetFileSystemType();
    void ReadAtPosition(uint64_t position);
    void ReturnToRoot();
    void ReturnToParent();
    void ViewVolumeInformation();
    void ViewFolderTree();

    // Destructor
    ~Fat32() = default;
};

#endif /* FAT32_HPP_ */