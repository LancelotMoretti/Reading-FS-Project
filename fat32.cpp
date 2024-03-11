#include "fat32.hpp"

////////////////////////////////////////////////////// Entry utilities ///////////////////////////////////////////////////////

///////////////////////////////////////////////////////// CLass Fat32 //////////////////////////////////////////////////////////

Fat32::Fat32(std::vector<BYTE>& bootSector, LPCWSTR volume) : Volume(volume){
    this->ReadBootSector(bootSector);
    this->Entries.push_back(readRDETSDET(this->VolumeName, this->StartOfRDET, true));
    if (this->Entries.back().size() == 0) {
        std::wcout << "No entries found!" << std::endl;
    }
    else {
        printFileAndFolder(this->Entries.back());
    }
}

void Fat32::ReadFileAtPosition(uint64_t position) {
    if (position >= this->Entries.back().size()) {
        std::wcout << "Invalid position!" << std::endl;
        return;
    }
    if (this->Entries.back()[position].getAttr() == L"Archive") {
        if (this->Entries.back()[position].getExt() == L"TXT") {
            this->ReadAndDisplayFileData(this->Entries.back()[position].getStartCluster(), this->Entries.back()[position].getSize());
        }
        else {
            std::wcout << "File type not supported!" << std::endl;
        }
    }
    else {
        std::wcout << this->Entries.back()[position].getAttr() << std::endl;
        this->Entries.push_back(readRDETSDET(this->VolumeName, this->GetDataCluster(this->Entries.back()[position].getStartCluster()) * this->BytesPerSector, false));
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

void Fat32::ViewVolumeInformation() {
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
    printFileAndFolder(this->Entries.back());
}

void Fat32::ReadBootSector(std::vector<BYTE>& bootSector) {
    this->BytesPerSector = nBytesToNum(bootSector.data(), 0x0B, 2);
    this->SectorsPerCluster = bootSector[0x0D];
    this->SectorsPerTrack = nBytesToNum(bootSector.data(), 0x18, 2);
    this->NumOfHeads = nBytesToNum(bootSector.data(), 0x1A, 2);

    this->SectorsPerBootSector = nBytesToNum(bootSector.data(), 0x0E, 2);
    this->NumOfFAT = bootSector[0x10];
    this->SizeOfVolume = nBytesToNum(bootSector.data(), 0x20, 4);
    this->SectorsPerFAT = nBytesToNum(bootSector.data(), 0x24, 4);

    this->StartOfRDET = rdetStartPoint(bootSector.data()) * this->BytesPerSector;
}

void Fat32::ReadAndDisplayFileData(uint64_t startCluster, uint64_t fileSize) {
    std::vector<BYTE> buffer;
    buffer.resize(BytesPerSector * SectorsPerCluster);
    uint64_t currentCluster = startCluster;
    uint64_t remainingBytes = fileSize;
    int count = 0;
    while (remainingBytes > 0 && currentCluster <= 0x0FFFFFEF) {
        std::wcout << "Loop " << count++ << " " << std::dec << remainingBytes << " " << std::hex << currentCluster << std::endl;

        this->ReadDataCluster(this->GetDataCluster(currentCluster), buffer);
        uint64_t bytesToRead = remainingBytes < (BytesPerSector * SectorsPerCluster) ? remainingBytes : (BytesPerSector * SectorsPerCluster);
        for (uint64_t i = 0; i < bytesToRead; i++) {
            std::wcout << wchar_t(buffer[i]);
        }
        remainingBytes -= bytesToRead;
        currentCluster = this->GetNextFATCluster(currentCluster);
    }
}

uint64_t Fat32::GetNextFATCluster(uint64_t currentCluster) {
    uint64_t BeginOfFat = this->SectorsPerBootSector;
    while (currentCluster > this->BytesPerSector / 4) {
        currentCluster -= this->BytesPerSector / 4;
        BeginOfFat += this->BytesPerSector;
    }
    std::wcout << L"Sectors per BootSector: " << std::dec << SectorsPerBootSector << std::endl;
    std::wcout << L"Current cluster: " << std::dec << currentCluster << std::endl;
    std::wcout << L"Begin of Fat: " << std::dec << BeginOfFat << std::endl;
    BYTE FAT[512];
    readSector(this->VolumeName, BeginOfFat * this->BytesPerSector, FAT, BytesPerSector);
    uint64_t nextCluster = nBytesToNum(FAT, currentCluster * 4, 4);
    return nextCluster;
}

uint64_t Fat32::GetDataCluster(uint64_t cluster) {
    return SectorsPerBootSector + (NumOfFAT * SectorsPerFAT) + (cluster - 2) * SectorsPerCluster;
}

void Fat32::ReadDataCluster(uint64_t cluster, std::vector<BYTE>& buffer) {
    readSector(this->VolumeName, cluster * this->BytesPerSector, buffer.data(), this->BytesPerSector * this->SectorsPerCluster);
}

std::string Fat32::GetFileSystemType() {
    return "FAT32";
}