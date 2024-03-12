#include "ntfs.hpp"

NTFS::NTFS(std::vector<BYTE>& bootSector, LPCWSTR volume) : Volume(volume) {
    ReadBootSector(bootSector);
}

std::string NTFS::GetFileSystemType() {
    return "NTFS";
}

void NTFS::ReadFileAtPosition(uint64_t position) {
    if (position >= this->MFTEntries.size()) {
        std::wcout << "Invalid position!" << std::endl;
        return;
    }

    if (this->MFTEntries[position].getAttr() == L"Archive") {
        if (this->MFTEntries[position].getExt() == L"TXT") {
            std::wcout << "File content: " << std::endl;
            this->ReadAndDisplayFileData(this->MFTEntries[position].getEntry());
        }
        else {
            std::wcout << "File type not supported!" << std::endl;
        }
    }
    else {
    }
}

void NTFS::ReturnToRoot() {
    std::wcout << "Returning to start" << std::endl;
}

void NTFS::ReturnToParent() {
    std::wcout << "Returning to parent" << std::endl;
}

void NTFS::ViewVolumeInformation() {
    std::wcout << "BytesPerSector: " << std::dec << this->BytesPerSector << std::endl;
    std::wcout << "SectorsPerCluster: " << std::dec << this->SectorsPerCluster << std::endl;
    std::wcout << "SectorsPerTrack: " << std::dec << this->SectorsPerTrack << std::endl;
    std::wcout << "NumOfHeads: " << std::dec << this->NumOfHeads << std::endl;
    std::wcout << "ReservedSectors: " << std::dec << this->ReservedSectors << std::endl;
    std::wcout << "HiddenSectors: " << std::dec << this->HiddenSectors << std::endl;
    std::wcout << "TotalSectors: " << std::dec << this->TotalSectors << std::endl;
    std::wcout << "StartOfMFT: " << std::dec << this->StartOfMFT << std::endl;
    std::wcout << "StartOfMFTMirr: " << std::dec << this->StartOfMFTMirr << std::endl;
}

void NTFS::ViewFolderTree() {
    std::wcout << "Viewing folder tree" << std::endl;
}

void NTFS::ReadBootSector(std::vector<BYTE>& bootSector) {
    if (bootSector.size() < 512) {
        return;
    }

    this->BytesPerSector = nBytesToNum(bootSector.data(), 0x0B, 2);
    this->SectorsPerCluster = bootSector[0x0D];
    this->SectorsPerTrack = nBytesToNum(bootSector.data(), 0x18, 2);
    this->NumOfHeads = nBytesToNum(bootSector.data(), 0x1A, 2);

    this->ReservedSectors = nBytesToNum(bootSector.data(), 0x0E, 2);
    this->HiddenSectors = nBytesToNum(bootSector.data(), 0x1C, 4);
    this->TotalSectors = nBytesToNum(bootSector.data(), 0x28, 8);
    this->StartOfMFT = MFTStartPoint(bootSector.data());
    this->StartOfMFTMirr = nBytesToNum(bootSector.data(), 0x38, 8) * this->SectorsPerCluster;
}

void NTFS::ReadAndDisplayFileData(uint64_t mftEntry) {
    uint64_t attributeCode = 0;
    uint64_t attributeSize = 0;
    uint64_t attributeOffset = 0;
    bool isResident = true;
    uint64_t nameLength = 0;
    uint64_t dataSize = 0; //Size of data in bytes if resident and number of clusters if non-resident
    uint64_t dataStart = 0; //Offset to the start of the data if resident and start cluster if non-resident
    uint64_t dataRunStart = 0; //Start data run of the data
    uint64_t dataRunLength = 0; //Length of the data run
    uint64_t dataRunEnd = 0; //End data run of the data
    uint64_t dataRunOffset = 0; //Offset to the start of the data run

    std::vector<BYTE> buffer(1024);

    readSector(this->VolumeName, this->StartOfMFT * this->BytesPerSector + mftEntry * 1024, buffer.data(), 1024); // Read MFT entry

    do {
        attributeCode = nBytesToNum(buffer.data(), attributeOffset + 0, 4);
        attributeSize = nBytesToNum(buffer.data(), attributeOffset + 4, 4);
        nameLength = nBytesToNum(buffer.data(), attributeOffset + 9, 1);

        if (attributeCode == 0x80 && nameLength == 0) {
            isResident = buffer[attributeOffset + 8] == 0;

            if (!isResident) {
                std::vector<BYTE> content(this->SectorsPerCluster * this->BytesPerSector);
                dataRunStart = nBytesToNum(buffer.data(), attributeOffset + 16, 8);
                dataRunEnd = nBytesToNum(buffer.data(), attributeOffset + 24, 8);
                dataRunOffset = nBytesToNum(buffer.data(), attributeOffset + 32, 2);

                attributeOffset += dataRunOffset;
                
                do {
                    dataRunLength = nBytesToNum(buffer.data(), attributeOffset, 1);
                    attributeOffset++;
                    dataSize = dataRunLength & 0x0F;
                    dataStart = dataRunLength >> 4;
                    
                    dataSize = nBytesToNum(buffer.data(), attributeOffset, dataSize);
                    attributeOffset += dataSize;
                    dataStart = nBytesToNum(buffer.data(), attributeOffset, dataStart);
                    attributeOffset += dataStart;

                    for (int i = 0; i < dataSize; i++) {
                        readSector(this->VolumeName,
                            (dataStart + i) * this->SectorsPerCluster * this->BytesPerSector,
                            content.data(),
                            this->BytesPerSector * this->SectorsPerCluster
                        );

                        for (int j = 0; j < content.size(); j++) {
                            std::wcout << wchar_t(content[j]);
                        }
                    }
                    
                } while (dataRunLength != 0);

                attributeOffset = 1024;
            }
            else {
                dataSize = nBytesToNum(buffer.data(), attributeOffset + 16, 4);
                dataStart = nBytesToNum(buffer.data(), attributeOffset + 20, 2);

                for (int i = 0; i < dataSize; i++) {
                    std::wcout << buffer[attributeOffset + i];
                }
                std::wcout << std::endl;
            }
            
            break;
        } 
        else attributeOffset += attributeSize;

    } while (attributeCode != 0 && attributeOffset < 1024);
}