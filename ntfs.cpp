#include "ntfs.hpp"

NTFS::NTFS(std::vector<BYTE>& bootSector, LPCWSTR drive) : Drive(drive) {
    ReadBootSector(bootSector);
}

std::string NTFS::GetFileSystemType() {
    return "NTFS";
}

void NTFS::ReadFileAtPosition(uint64_t position) {
    std::cout << "Reading file at position: " << position << std::endl;
}

void NTFS::ReturnToStart() {
    std::cout << "Returning to start" << std::endl;
}

void NTFS::ReturnToParent() {
    std::cout << "Returning to parent" << std::endl;
}

void NTFS::ReadBootSector(std::vector<BYTE>& bootSector) {
    if (bootSector.size() < 512) {
        return;
    }

    this->ReservedSectors = bootSector[14] + (bootSector[15] << 8);
    //Debugging: 
    // std::cout << "ReservedSectors: " << this->ReservedSectors << std::endl;
    this->HiddenSectors = bootSector[28] + (bootSector[29] << 8) + (bootSector[30] << 16) + (bootSector[31] << 24);
    // std::cout << "HiddenSectors: " << this->HiddenSectors << std::endl;
    this->TotalSectors = bootSector[40] + (bootSector[41] << 8) + (bootSector[42] << 16) + (bootSector[43] << 24);
    // std::cout << "TotalSectors: " << this->TotalSectors << std::endl;
    this->StartOfMFT = bootSector[48] + (bootSector[49] << 8) + (bootSector[50] << 16) + (bootSector[51] << 24);
    // std::cout << "StartOfMFT: " << this->StartOfMFT << std::endl;
    this->StartOfMFTMirr = bootSector[56] + (bootSector[57] << 8) + (bootSector[58] << 16) + (bootSector[59] << 24);
    // std::cout << "StartOfMFTMirr: " << this->StartOfMFTMirr << std::endl;
}