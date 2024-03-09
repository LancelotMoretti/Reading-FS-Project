#include "fat32.hpp"

////////////////////////////////////////////////////// Entry utilities ///////////////////////////////////////////////////////

///////////////////////////////////////////////////////// CLass Fat32 //////////////////////////////////////////////////////////

Fat32::Fat32(std::vector<BYTE>& bootSector, LPCWSTR drive) : Drive(drive){
    this->ReadBootSector(bootSector);
    this->Entries.push_back(readRDETSDET(this->DriveName, this->StartOfRDET, true));
}

void Fat32::ReadFileAtPosition(uint64_t position) {
    if (position >= this->Entries.back().size()) {
        std::wcout << "Invalid position!" << std::endl;
        return;
    }
    if (this->Entries.back()[position].getAttr() == "Archive") {
        this->ReadAndDisplayFileData(this->Entries.back()[position].getStartCluster(), this->Entries.back()[position].getSize());
    }
    else {
        this->Entries.push_back(readRDETSDET(this->DriveName, this->Entries.back()[position].getStartCluster(), false));
    }
}

void Fat32::ReturnToStart() {
    while (this->Entries.size() > 1) {
        this->Entries.pop_back();
    }
    printFileAndFolder(this->Entries.back());
}

void Fat32::ReturnToParent() {
    this->Entries.pop_back();
    printFileAndFolder(this->Entries.back());
}

void Fat32::ViewDriveInformation() {
    std::wcout << "BytesPerSector: " << this->BytesPerSector << std::endl;
    std::wcout << "SectorsPerCluster: " << this->SectorsPerCluster << std::endl;
    std::wcout << "SectorsPerTrack: " << this->SectorsPerTrack << std::endl;
    std::wcout << "NumOfHeads: " << this->NumOfHeads << std::endl;
    std::wcout << "SectorsPerBootSector: " << this->SectorsPerBootSector << std::endl;
    std::wcout << "NumOfFAT: " << this->NumOfFAT << std::endl;
    std::wcout << "SizeOfVolume: " << this->SizeOfVolume << std::endl;
    std::wcout << "SectorsPerFAT: " << this->SectorsPerFAT << std::endl;
    std::wcout << "StartOfRDET: " << this->StartOfRDET << std::endl;
    std::wcout << "SizeOfRDET: " << this->SizeOfRDET << std::endl;
}

void Fat32::ViewFolderTree() {
    printFileAndFolder(this->Entries.back());
}

void Fat32::ReadBootSector(std::vector<BYTE>& bootSector) {
    this->SectorsPerBootSector = bootSector[11] + (bootSector[12] << 8);
    this->NumOfFAT = bootSector[16];
    this->SizeOfVolume = bootSector[32] + (bootSector[33] << 8) + (bootSector[34] << 16) + (bootSector[35] << 24);
    this->SectorsPerFAT = bootSector[22] + (bootSector[23] << 8);
    this->StartOfRDET = this->SectorsPerBootSector + (this->NumOfFAT * this->SectorsPerFAT);
    this->SizeOfRDET = (this->SizeOfVolume * 32) / 512;

    this->BytesPerSector = bootSector[0x0B] + (bootSector[0x0C] << 8);
    this->SectorsPerCluster = bootSector[0x0D];
    this->SectorsPerTrack = bootSector[0x18] + (bootSector[0x19] << 8);
    this->NumOfHeads = bootSector[0x1A] + (bootSector[0x1B] << 8);
}

void Fat32::ReadAndDisplayFileData(uint64_t startCluster, uint64_t fileSize) {
    std::vector<BYTE> buffer;
    buffer.resize(BytesPerSector * SectorsPerCluster);
    uint64_t currentCluster = startCluster;
    uint64_t remainingBytes = fileSize;
    while (remainingBytes > 0 && currentCluster < 0x0FFFFFF8) {
        this->ReadDataCluster(this->GetDataCluster(currentCluster), buffer); // SomeFunction(currentCluster)
        uint64_t bytesToRead = remainingBytes < (BytesPerSector * SectorsPerCluster) ? remainingBytes : (BytesPerSector * SectorsPerCluster);
        for (uint64_t i = 0; i < bytesToRead; i++) {
            std::wcout << buffer[i];
        }
        remainingBytes -= bytesToRead;
        currentCluster = this->GetNextFATCluster(currentCluster);
    }
}

uint64_t Fat32::GetNextFATCluster(uint64_t currentCluster) {
    uint64_t BeginOfFat = SectorsPerBootSector * BytesPerSector;
    BeginOfFat += currentCluster * 4;
    BYTE FAT[512];
    readSector(this->DriveName, BeginOfFat, FAT, BytesPerSector);
    uint64_t nextCluster = FAT[0] + (FAT[1] << 8) + (FAT[2] << 16) + (FAT[3] << 24);
    return nextCluster;
}

uint64_t Fat32::GetDataCluster(uint64_t cluster) {
    return SectorsPerBootSector + (NumOfFAT * SectorsPerFAT) + (cluster - 2) * SectorsPerCluster;
}

void Fat32::ReadDataCluster(uint64_t cluster, std::vector<BYTE> buffer) {
    readSector(this->DriveName, cluster, buffer.data(), BytesPerSector * SectorsPerCluster);
}

std::string Fat32::GetFileSystemType() {
    return "FAT32";
}