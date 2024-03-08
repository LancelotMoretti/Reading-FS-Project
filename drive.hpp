#ifndef DRIVE_HPP_
#define DRIVE_HPP_
#include "utilities.hpp"

class Drive {
public:
    Drive(LPCWSTR drive) : DriveName(drive) {}
    
    virtual std::string GetFileSystemType() = 0;
    virtual void ReadFileAtPosition(uint64_t position) = 0;
    virtual void ReturnToStart() = 0;
    virtual void ReturnToParent() = 0;
    virtual void ViewDriveInformation() = 0;
    virtual void ViewFolderTree() = 0;

    virtual ~Drive() = default;

protected:
    LPCWSTR DriveName;
    int BytesPerSector;
    int SectorsPerCluster;
    int SectorsPerTrack;
    int NumOfHeads;

    int NextReadingOffset;

};

#endif /* DRIVE_HPP_ */