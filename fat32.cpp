#include "fat32.hpp"

void Fat32::ReadAndDisplayFileData(int startCluster, int fileSize) {
    std::vector<BYTE> buffer;
    buffer.resize(BytesPerSector * SectorsPerCluster);
    int currentCluster = startCluster;
    int remainingBytes = fileSize;
    while (remainingBytes > 0) {
        this->ReadDataCluster(currentCluster, buffer);
        int bytesToRead = remainingBytes < (BytesPerSector * SectorsPerCluster) ? remainingBytes : (BytesPerSector * SectorsPerCluster);
        for (int i = 0; i < bytesToRead; i++) {
            std::cout << buffer[i];
        }
        remainingBytes -= bytesToRead;
        currentCluster = this->GetNextDataCluster(currentCluster);
    }
}

int Fat32::GetNextDataCluster(int currentCluster) {
    int FATOffset = currentCluster * 4;
    int FATSector = FATOffset / BytesPerSector;
    int FATEntry = FATOffset % BytesPerSector;
    std::vector<BYTE> buffer;
    buffer.resize(BytesPerSector);
    this->ReadDataSector(SectorsPerBootSector + FATSector, buffer);
    return buffer[FATEntry] + (buffer[FATEntry + 1] << 8) + (buffer[FATEntry + 2] << 16) + (buffer[FATEntry + 3] << 24);
}

void Fat32::ReadDataCluster(int cluster, std::vector<BYTE> buffer) {
    for (int i = 0; i < SectorsPerCluster; i++) {
        this->ReadDataSector(cluster * SectorsPerCluster + i, buffer);
    }
}

void Fat32::ReadDataSector(int sector, std::vector<BYTE> buffer) {
    ReadSector(L"\\\\.\\D:", sector * BytesPerSector, buffer.data(), BytesPerSector);
}