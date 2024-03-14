#ifndef VOLUME_HPP_
#define VOLUME_HPP_

#include "utilities.hpp"

class Volume {
public:
    // Constructor
    Volume(HANDLE volumeHandle) : VolumeHandle(volumeHandle) {}
    
    // Pure virtual methods for interface
    virtual std::string GetFileSystemType() = 0;
    virtual void ReadAtPosition(uint64_t position) = 0;
    virtual void ReturnToRoot() = 0;
    virtual void ReturnToParent() = 0;
    virtual void ViewVolumeInformation() = 0;
    virtual void ViewFolderTree() = 0;

    // Destructor
    virtual ~Volume() = default;

protected:
    // Volume specific
    HANDLE VolumeHandle;
    int BytesPerSector;
    int SectorsPerCluster;
    int SectorsPerTrack;
    int NumOfHeads;
};

#endif /* VOLUME_HPP_ */