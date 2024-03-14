#include "ntfs.hpp"

NTFS::NTFS(std::vector<BYTE>& bootSector, HANDLE volumeHandle) : Volume(volumeHandle) {
    ReadBootSector(bootSector);

    // Read root directory
    this->curEntry = uint64_t(5);
    uint64_t start = this->curEntry * 1024 + this->StartOfMFT * this->BytesPerSector;
    std::vector<uint64_t> listEntries = readFolder(this->VolumeHandle, start);
    formatListEntries(listEntries);
    this->MFTEntries = readNTFSTree(this->VolumeHandle,
        this->StartOfMFT * this->BytesPerSector,
        listEntries
    );
}

void NTFS::SetCurEntry(uint64_t entry) {
    this->curEntry = entry;
}

uint64_t NTFS::GetCurEntry() {
    return this->curEntry;
}

std::string NTFS::GetFileSystemType() {
    return "NTFS";
}

void NTFS::ReadAtPosition(uint64_t position) {
    if (position >= this->MFTEntries.size()) {
        std::wcout << "Invalid position!" << std::endl;
        return;
    }

    if (this->MFTEntries[position].getType().find(L"File") != std::wstring::npos) {
        if (this->MFTEntries[position].getExt() == L"txt") {
            std::wcout << "File content: " << std::endl;
            this->ReadAndDisplayFileData(this->MFTEntries[position].getEntry());
            system("pause");
        }
        else {
            std::wcout << "File type not supported!" << std::endl;
        }
    }
    else {
        uint64_t start = this->MFTEntries[position].getEntry() * 1024 + this->StartOfMFT * this->BytesPerSector;
        this->curEntry = this->MFTEntries[position].getEntry();
        std::vector<uint64_t> listEntries = readFolder(this->VolumeHandle, start);
        formatListEntries(listEntries);
        this->MFTEntries = readNTFSTree(this->VolumeHandle,
            this->StartOfMFT * this->BytesPerSector,
            listEntries
        );
    }
}

void NTFS::ReturnToRoot() {
    // Reset current entry
    this->curEntry = uint64_t(5);
    // Read root directory
    uint64_t start = this->curEntry * 1024 + this->StartOfMFT * this->BytesPerSector;
    std::vector<uint64_t> listEntries = readFolder(this->VolumeHandle, start);
    formatListEntries(listEntries);
    this->MFTEntries = readNTFSTree(this->VolumeHandle,
        this->StartOfMFT * this->BytesPerSector,
        listEntries
    );
}

void NTFS::ReturnToParent() {
    if (this->curEntry == uint64_t(5)) return; // Exception/Quick exit

    // Prepare for reading data
    DWORD bytesRead;
    uint64_t readPoint = this->StartOfMFT * this->BytesPerSector + this->curEntry * 1024;
    LONG high = readPoint >> 32;
    LONG low = readPoint;
    SetFilePointer(this->VolumeHandle, low, &high, FILE_BEGIN);
    BYTE sector[1024];
    ReadFile(this->VolumeHandle, sector, 1024, &bytesRead, NULL);

    // Find $FILE_NAME position
    uint64_t startAttr = nBytesToNum(sector, 0x14, 2);
    while (startAttr < 1024 && nBytesToNum(sector, startAttr, 4) != uint64_t(48)) {
        uint64_t size = nBytesToNum(sector, startAttr + uint64_t(4), 4);
        if (size == 0) return;
        startAttr += size;
    }
    if (startAttr >= 1024) return;
    uint64_t offFileName = startAttr;

    // Seek to parent position
    uint64_t startContent = offFileName + 24; // Starting position of content section
    uint64_t parentEntry = nBytesToNum(sector, startContent, 6); // Six bytes not eight bytes
    this->curEntry = parentEntry;

    // Read parent directory
    uint64_t start = this->curEntry * 1024 + this->StartOfMFT * this->BytesPerSector;
    std::vector<uint64_t> listEntries = readFolder(this->VolumeHandle, start);
    formatListEntries(listEntries);
    this->MFTEntries = readNTFSTree(this->VolumeHandle,
        this->StartOfMFT * this->BytesPerSector,
        listEntries
    );
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
    printFileAndFolderNTFS(this->MFTEntries);
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
    this->StartOfMFTMirr = nBytesToNum(bootSector.data(), 0x38, 8);
}

void NTFS::ReadAndDisplayFileData(uint64_t mftEntry) {
    // Attribute information
    uint64_t attributeCode = 0;
    uint64_t attributeSize = 0;

    // Current offset in the buffer
    uint64_t attributeOffset = 0;

    // Flag to check if the attribute is resident or non-resident
    bool isResident = true;

    uint64_t nameLength = 0;
    uint64_t dataSize = 0; //Size of data in bytes if resident and number of clusters if non-resident
    uint64_t dataStart = 0; //Offset to the start of the data if resident and start cluster if non-resident
    uint64_t dataRunStart = 0; //Start VCN of the data runlist if needed
    uint64_t dataRunLength = 0; //Length of the data run
    uint64_t dataRunEnd = 0; //End VCN of the data runlist if needed
    uint64_t dataRunOffset = 0; //Offset to the start of the data run

    std::vector<BYTE> buffer(1024);

    readSector(this->VolumeHandle, this->StartOfMFT * this->BytesPerSector + mftEntry * 1024, buffer.data(), 1024); // Read MFT entry

    attributeOffset = nBytesToNum(buffer.data(), 0x14, 2); // Offset to the first attribute

    do {
        // Read attribute type and size to jump
        attributeCode = nBytesToNum(buffer.data(), attributeOffset + 0, 4);
        if (attributeCode == 0) break;
        attributeSize = nBytesToNum(buffer.data(), attributeOffset + 4, 4);
        nameLength = nBytesToNum(buffer.data(), attributeOffset + 9, 1);

        if (attributeCode == 0x80 && nameLength == 0) {
            isResident = (buffer[attributeOffset + 8] == 0);

            if (!isResident) {
                // Read data attribute's data if non-resident
                std::vector<BYTE> content(this->SectorsPerCluster * this->BytesPerSector);
                dataRunStart = nBytesToNum(buffer.data(), attributeOffset + 16, 8);
                dataRunEnd = nBytesToNum(buffer.data(), attributeOffset + 24, 8);
                dataRunOffset = nBytesToNum(buffer.data(), attributeOffset + 32, 2);

                attributeOffset += dataRunOffset;
                
                do {
                    // Read length of datarun
                    dataRunLength = nBytesToNum(buffer.data(), attributeOffset, 1);
                    if (dataRunLength == 0) break;
                    attributeOffset++;
                    dataSize = dataRunLength & 0x0F;
                    dataStart = dataRunLength >> 4;
                    
                    // Read run length and run offset of datarun
                    attributeOffset += dataSize;
                    dataSize = nBytesToNum(buffer.data(), attributeOffset - dataSize, dataSize);
                    attributeOffset += dataStart;
                    dataStart = nBytesToNum(buffer.data(), attributeOffset - dataStart, dataStart);

                    // Read data and display
                    for (int i = 0; i < dataSize; i++) {
                        readSector(this->VolumeHandle,
                            (dataStart + i) * this->SectorsPerCluster * this->BytesPerSector,
                            content.data(),
                            this->BytesPerSector * this->SectorsPerCluster
                        );

                        for (int j = 0; j < content.size(); j++) {
                            if (content[j] == L'\000') break;
                            std::wcout << wchar_t(content[j]);
                        }
                    }
                    
                } while (dataRunLength != 0);
            }
            else {
                // Read start offset and size of data
                dataSize = nBytesToNum(buffer.data(), attributeOffset + 16, 4);
                dataStart = nBytesToNum(buffer.data(), attributeOffset + 20, 2);

                // Read data end display
                for (int i = 0; i < dataSize; i++) {
                    uint64_t end = nBytesToNum(buffer.data(), attributeOffset + i, 4);
                    if (end == 0xffffffff) break;
                    std::wcout << buffer[attributeOffset + i];
                }
            }
            
            std::wcout << std::endl << std::endl;

            return;
        } 
        else attributeOffset += attributeSize;

    } while (attributeCode != 0 && attributeOffset < 1024);
}