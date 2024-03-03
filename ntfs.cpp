#include "ntfs.hpp"

void NTFS::ReadBootSector(std::vector<BYTE>& buffer) {
    if (buffer.size() < 512) {
        return;
    }

    if (!readSector(this->drive, 0, buffer.data(), 512)) {
        return;
    }

    readSector(this->drive, 0, buffer.data(), 512);

    this->ReservedSectors = buffer[14] + (buffer[15] << 8);
    //Debugging: 
    std::cout << "ReservedSectors: " << this->ReservedSectors << std::endl;
    this->HiddenSectors = buffer[28] + (buffer[29] << 8) + (buffer[30] << 16) + (buffer[31] << 24);
    std::cout << "HiddenSectors: " << this->HiddenSectors << std::endl;
    this->TotalSectors = buffer[40] + (buffer[41] << 8) + (buffer[42] << 16) + (buffer[43] << 24);
    std::cout << "TotalSectors: " << this->TotalSectors << std::endl;
    this->StartOfMFT = buffer[48] + (buffer[49] << 8) + (buffer[50] << 16) + (buffer[51] << 24);
    std::cout << "StartOfMFT: " << this->StartOfMFT << std::endl;
    this->StartOfMFTMirr = buffer[56] + (buffer[57] << 8) + (buffer[58] << 16) + (buffer[59] << 24);
    std::cout << "StartOfMFTMirr: " << this->StartOfMFTMirr << std::endl;
}