#include "ntfs.hpp"

NTFS::NTFS(std::vector<BYTE>& bootSector, LPCWSTR volume) : Volume(volume) {
    ReadBootSector(bootSector);
}

std::string NTFS::GetFileSystemType() {
    return "NTFS";
}

void NTFS::ReadFileAtPosition(uint64_t position) {
    std::wcout << "Reading file at position: " << position << std::endl;
}

void NTFS::ReturnToStart() {
    std::wcout << "Returning to start" << std::endl;
}

void NTFS::ReturnToParent() {
    std::wcout << "Returning to parent" << std::endl;
}

void NTFS::ViewVolumeInformation() {
    std::wcout << "BytesPerSector: " << this->BytesPerSector << std::endl;
    std::wcout << "SectorsPerCluster: " << this->SectorsPerCluster << std::endl;
    std::wcout << "SectorsPerTrack: " << this->SectorsPerTrack << std::endl;
    std::wcout << "NumOfHeads: " << this->NumOfHeads << std::endl;
    std::wcout << "ReservedSectors: " << this->ReservedSectors << std::endl;
    std::wcout << "HiddenSectors: " << this->HiddenSectors << std::endl;
    std::wcout << "TotalSectors: " << this->TotalSectors << std::endl;
    std::wcout << "StartOfMFT: " << this->StartOfMFT << std::endl;
    std::wcout << "StartOfMFTMirr: " << this->StartOfMFTMirr << std::endl;
}

void NTFS::ViewFolderTree() {
    std::wcout << "Viewing folder tree" << std::endl;
}

void NTFS::ReadBootSector(std::vector<BYTE>& bootSector) {
    if (bootSector.size() < 512) {
        return;
    }

    this->ReservedSectors = nBytesToNum(bootSector.data(), 14, 2);
    //Debugging: 
    // std::wcout << "ReservedSectors: " << this->ReservedSectors << std::endl;
    this->HiddenSectors = nBytesToNum(bootSector.data(), 28, 4);
    // std::wcout << "HiddenSectors: " << this->HiddenSectors << std::endl;
    this->TotalSectors = nBytesToNum(bootSector.data(), 40, 4);
    // std::wcout << "TotalSectors: " << this->TotalSectors << std::endl;
    this->StartOfMFT = nBytesToNum(bootSector.data(), 48, 4);
    // std::wcout << "StartOfMFT: " << this->StartOfMFT << std::endl;
    this->StartOfMFTMirr = nBytesToNum(bootSector.data(), 56, 4);
    // std::wcout << "StartOfMFTMirr: " << this->StartOfMFTMirr << std::endl;

    this->BytesPerSector = nBytesToNum(bootSector.data(), 0x0B, 2);
    this->SectorsPerCluster = bootSector[0x0D];
    this->SectorsPerTrack = nBytesToNum(bootSector.data(), 0x18, 2);
    this->NumOfHeads = nBytesToNum(bootSector.data(), 0x1A, 2);

}