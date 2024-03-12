#ifndef VOLUME_HPP_
#define VOLUME_HPP_
#include "utilities.hpp"

class Volume {
public:
    Volume(LPCWSTR volume) : VolumeName(volume) {}
    
    virtual std::string GetFileSystemType() = 0;
    virtual void ReadFileAtPosition(uint64_t position) = 0;
    virtual void ReturnToRoot() = 0;
    virtual void ReturnToParent() = 0;
    virtual void ViewVolumeInformation() = 0;
    virtual void ViewFolderTree() = 0;

    virtual ~Volume() = default;

protected:
    LPCWSTR VolumeName;
    int BytesPerSector;
    int SectorsPerCluster;
    int SectorsPerTrack;
    int NumOfHeads;

    int NextReadingOffset;

};

#endif /* VOLUME_HPP_ */