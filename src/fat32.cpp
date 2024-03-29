#include "fat32.hpp"

////////////////////////////////////////////////////// FATEntry utilities ///////////////////////////////////////////////////////

///////////////////////////////////////////////////////// CLass Fat32 //////////////////////////////////////////////////////////

Fat32::Fat32(std::vector<BYTE>& bootSector, HANDLE volumeHandle) : Volume(volumeHandle){
    this->ReadBootSector(bootSector);
    this->Entries.push_back(readRDETSDET(this->VolumeHandle, this->StartOfRDET, true));
    if (this->Entries.back().size() == 0) {
        std::wcout << "No entries found!" << std::endl;
    }
}

void Fat32::ReadAtPosition(uint64_t position) {
    // Check if position is valid
    if (position >= this->Entries.back().size()) {
        std::wcout << "Invalid position!" << std::endl;
        return;
    }

    // Read file content if it is a text file else print error message
    if (this->Entries.back()[position].getAttr().find(L"Archive")!= std::wstring::npos && this->Entries.back()[position].getAttr().find(L"Subdirectory") == std::wstring::npos) {
        // Check if the file is a text file
        if (this->Entries.back()[position].getExt() == L"TXT") {
            std::wcout << "File content: " << std::endl;
            this->ReadAndDisplayFileData(this->Entries.back()[position].getStartCluster(),
                this->Entries.back()[position].getSize()
            );
            system("pause");
        }
        else {
            std::wcout << "File type not supported!" << std::endl;
            std::wcout << "Please use the reader for the file type: " << this->Entries.back()[position].getExt() << std::endl;
        }
    }
    // Open folder if it not a file
    else {
        this->Entries.push_back(
            readRDETSDET(this->VolumeHandle,
                this->ToDataCluster(this->Entries.back()[position].getStartCluster()) * this->BytesPerSector,
                false
            )
        );
    }
}

void Fat32::ReturnToRoot() {
    while (this->Entries.size() > 1) {
        this->Entries.pop_back();
    }
}

void Fat32::ReturnToParent() {
    if (this->Entries.size() == 1) {
        return;
    }
    this->Entries.pop_back();
}

void Fat32::ViewVolumeInformation() {
    // Print the volume information
    std::wcout << "BytesPerSector: " << std::dec << this->BytesPerSector << std::endl;
    std::wcout << "SectorsPerCluster: " << std::dec << this->SectorsPerCluster << std::endl;
    std::wcout << "SectorsPerTrack: " << std::dec << this->SectorsPerTrack << std::endl;
    std::wcout << "NumOfHeads: " << std::dec << this->NumOfHeads << std::endl;
    std::wcout << "SectorsPerBootSector: " << std::dec << this->SectorsPerBootSector << std::endl;
    std::wcout << "NumOfFAT: " << std::dec << this->NumOfFAT << std::endl;
    std::wcout << "SizeOfVolume: " << std::dec << this->SizeOfVolume << std::endl;
    std::wcout << "SectorsPerFAT: " << std::dec << this->SectorsPerFAT << std::endl;
    std::wcout << "StartOfRDET: " << std::dec << this->StartOfRDET << std::endl;
}

void Fat32::ViewFolderTree() {
    printFolderTreeFat32(this->Entries.back());
}

void Fat32::ReadBootSector(std::vector<BYTE>& bootSector) {
    // Read the boot sector
    // Volume information
    this->BytesPerSector = nBytesToNum(bootSector.data(), 0x0B, 2);
    this->SectorsPerCluster = bootSector[0x0D];
    this->SectorsPerTrack = nBytesToNum(bootSector.data(), 0x18, 2);
    this->NumOfHeads = nBytesToNum(bootSector.data(), 0x1A, 2);

    // FAT specific information
    this->SectorsPerBootSector = nBytesToNum(bootSector.data(), 0x0E, 2);
    this->NumOfFAT = bootSector[0x10];
    this->SizeOfVolume = nBytesToNum(bootSector.data(), 0x20, 4);
    this->SectorsPerFAT = nBytesToNum(bootSector.data(), 0x24, 4);
    
    this->StartOfRDET = rdetStartPoint(bootSector.data()) * this->BytesPerSector;
}

void Fat32::ReadAndDisplayFileData(uint64_t startCLusterFATPos, uint64_t fileSize) {
    // Vector to store the data of a cluster
    std::vector<BYTE> buffer;
    buffer.resize(this->BytesPerSector * this->SectorsPerCluster);

    // Current cluster (in position) and remaining bytes to read
    uint64_t curClusterFATPos = startCLusterFATPos;
    uint64_t remainingBytes = fileSize;
    
    while (remainingBytes > 0 && curClusterFATPos <= 0x0FFFFFEF) {
        // Read the data of the current cluster
        this->ReadDataCluster(this->ToDataCluster(curClusterFATPos), buffer);

        // Number of bytes to read in the current cluster
        uint64_t bytesReaded = remainingBytes < (this->BytesPerSector * this->SectorsPerCluster)
                             ? remainingBytes : (this->BytesPerSector * this->SectorsPerCluster);
        
        // Print the data
        for (uint64_t i = 0; i < bytesReaded; i++) {
            std::wcout << wchar_t(buffer[i]);
        }

        // Update the remaining bytes and the current cluster
        remainingBytes -= bytesReaded;
        curClusterFATPos = this->GetNextClusterPos(curClusterFATPos);
    }
    std::wcout << std::endl << std::endl;
}

uint64_t Fat32::GetNextClusterPos(uint64_t curClusterFATPos) {
    // Calculate the begin of the FAT
    uint64_t BeginOfFat = this->SectorsPerBootSector;

    // If the current cluster is not in the first sector of the FAT
    while (curClusterFATPos > this->BytesPerSector / 4) {
        curClusterFATPos -= this->BytesPerSector / 4;
        BeginOfFat += this->BytesPerSector;
    }

    // Read the current FAT sector
    BYTE FAT[512];
    readMultiSector(this->VolumeHandle, BeginOfFat * this->BytesPerSector, FAT, BytesPerSector);

    // Get the next cluster
    uint64_t nextCluster = nBytesToNum(FAT, curClusterFATPos * 4, 4);
    return nextCluster;
}

uint64_t Fat32::ToDataCluster(uint64_t cluster) {
    return this->SectorsPerBootSector
        + (this->NumOfFAT * this->SectorsPerFAT)
        + (cluster - 2) * this->SectorsPerCluster;
}

void Fat32::ReadDataCluster(uint64_t cluster, std::vector<BYTE>& buffer) {
    readMultiSector(this->VolumeHandle,
        cluster * this->BytesPerSector,
        buffer.data(),
        this->BytesPerSector * this->SectorsPerCluster
    );
}

std::string Fat32::GetFileSystemType() {
    return "FAT32";
}