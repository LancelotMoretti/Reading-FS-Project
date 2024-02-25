#include "fat32.hpp"

// CLass Fat32

void Fat32::ReadBootSector(std::vector<BYTE> buffer) {
    readSector(this->drive, 0, buffer.data(), 512);
    this->SectorsPerBootSector = buffer[11] + (buffer[12] << 8);
    this->NumOfFAT = buffer[16];
    this->SizeOfVolume = buffer[32] + (buffer[33] << 8) + (buffer[34] << 16) + (buffer[35] << 24);
    this->SectorsPerFAT = buffer[22] + (buffer[23] << 8);
    this->StartOfRDET = this->SectorsPerBootSector + (this->NumOfFAT * this->SectorsPerFAT);
    this->SizeOfRDET = (this->SizeOfVolume * 32) / 512;
}

void Fat32::ReadAndDisplayFileData(int startCluster, int fileSize) {
    std::vector<BYTE> buffer;
    buffer.resize(BytesPerSector * SectorsPerCluster);
    int currentCluster = startCluster;
    int remainingBytes = fileSize;
    while (remainingBytes > 0 && currentCluster < 0x0FFFFFF8) {
        this->ReadDataCluster(this->GetDataCluster(currentCluster), buffer); // SomeFunction(currentCluster)
        int bytesToRead = remainingBytes < (BytesPerSector * SectorsPerCluster) ? remainingBytes : (BytesPerSector * SectorsPerCluster);
        for (int i = 0; i < bytesToRead; i++) {
            std::cout << buffer[i];
        }
        remainingBytes -= bytesToRead;
        currentCluster = this->GetNextFATCluster(currentCluster);
    }
}

int Fat32::GetNextFATCluster(int currentCluster) {
    int BeginOfFat = SectorsPerBootSector * BytesPerSector;
    BeginOfFat += currentCluster * 4;
    BYTE FAT[512];
    readSector(this->drive, BeginOfFat, FAT, BytesPerSector);
    int nextCluster = FAT[0] + (FAT[1] << 8) + (FAT[2] << 16) + (FAT[3] << 24);
    return nextCluster;
}

int Fat32::GetDataCluster(int cluster) {
    return SectorsPerBootSector + (NumOfFAT * SectorsPerFAT) + (cluster - 2) * SectorsPerCluster;
}

void Fat32::ReadDataCluster(int cluster, std::vector<BYTE> buffer) {
    readSector(this->drive, cluster, buffer.data(), BytesPerSector * SectorsPerCluster);
}

/////////////////// RDET / SDET

std::ostream& operator << (std::ostream& out, const Entry& cur) {
    out << "Name: " << cur.name;
    if (cur.ext != "") out << "." << cur.ext;
    out << std::endl;
    out << "Attribute: " << cur.attr << std::endl;
    out << "Last modified time: " << cur.date << ", " << cur.time << std::endl;
    out << "Starting cluster: " << cur.cluster << std::endl;
    out << "Size: " << cur.size << std::endl;
    return out;
}

void printFileAndFolder(std::vector<Entry> vect) {
    bool isPrinted = false;
    for (int i = 0; i < vect.size(); i++) {
        if (vect[i].getAttr() == "Archive" || vect[i].getAttr() == "Subdirectory") {
            std::cout << vect[i] << std::endl;
            isPrinted = true;
        }
    }
    if (!isPrinted) std::cout << "No file or folder here!" << std::endl;
}

std::vector<Entry> readRDETSDET(LPCWSTR drive, uint64_t readPoint, bool isRDET) {
    int start = isRDET ? 0 : 64; // True: RDET, False: SDET

    DWORD bytesRead;
    HANDLE device = NULL;
    BYTE sector[512];

    std::vector<Entry> result;
    result.clear();

    device = CreateFileW(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    LONG high = readPoint >> 32;
    LONG low = readPoint;

    SetFilePointer(device, low, &high, FILE_BEGIN); // Start reading from readPoint
    bool isStopped = false, hasSubEntry = false;
    std::string name = "";

    while (!isStopped && ReadFile(device, sector, 512, &bytesRead, NULL)) {
        printFAT32BootSector(sector);
        // Iterate through all entries of current sector
        for (int i = start; i < 512; i += 32) {
            if (sector[i] == 0xE5) continue; // Deleted entry
            if (sector[i] == 0x00) { // End of RDET table
                isStopped = true;
                break;
            }

            if (sector[i + 11] == 0x0F) { // Sub entry
                std::string tempName = "";
                for (int j = 1; j < 11; j++) if (sector[i + j] != 0x00 && sector[i + j] != 0xFF) tempName += sector[i + j];
                for (int j = 14; j < 26; j++) if (sector[i + j] != 0x00 && sector[i + j] != 0xFF) tempName += sector[i + j];
                for (int j = 28; j < 32; j++) if (sector[i + j] != 0x00 && sector[i + j] != 0xFF) tempName += sector[i + j];
                name = tempName + name;
                hasSubEntry = true;
            } else { // Main entry
                Entry cur;
                cur.setByte(sector, i);

                // Set name
                if (hasSubEntry) {
                    cur.setName(name);
                    hasSubEntry = false;
                } else {
                    for (int j = 0; j < 8; j++) name += sector[i + j];
                    cur.setName(name); // name
                }
                cur.formatName();

                // Set extension
                std::string ext = "";
                for (int j = 0; j < 3; j++) ext += (sector[i + 8 + j]);
                if (sector[i + 11] == 0x10) cur.setExt(""); // Folder
                else cur.setExt(ext); // Others

                // Attribute
                switch (sector[i + 11]) {
                    case 0x01: {
                        cur.setAttr("Read Only");
                        break;
                    }
                    case 0x02: {
                        cur.setAttr("Hidden");
                        break;
                    }
                    case 0x04: {
                        cur.setAttr("System");
                        break;
                    }
                    case 0x08: {
                        cur.setAttr("Volume Label");
                        break;
                    }
                    case 0x10: {
                        cur.setAttr("Subdirectory");
                        break;
                    }
                    case 0x20: {
                        cur.setAttr("Archive");
                        break;
                    }
                    case 0x40: {
                        cur.setAttr("Device");
                        break;
                    }
                    case 0x80: {
                        cur.setAttr("Unused");
                        break;
                    }
                }

                // Time
                std::string curTime = std::to_string(sector[i + 23] >> 3) + ":"; // Hour
                curTime += std::to_string((sector[i + 23] % (1 << 3)) * (1 << 3) + ((sector[i + 22] >> 5))) + ":"; // Minute
                curTime += std::to_string(sector[i + 22] % (1 << 5) * 2); // Second
                cur.setTime(curTime);

                // Date
                std::string curDate = std::to_string(sector[i + 24] % (1 << 5)) + "/"; // Day
                curDate += std::to_string((sector[i + 25] % 2) * (1 << 3) + ((sector[i + 24] >> 5))) + "/"; // Month
                curDate += std::to_string((sector[i + 25] >> 1) + 1980); // Year
                cur.setDate(curDate);

                // Cluster
                cur.setCluster(sector[i + 21] * (1 << 7) * (1 << 7) * (1 << 7) * 8 + sector[i + 20] * (1 << 7) * (1 << 7) * 4 + sector[i + 27] * (1 << 7) * 2 + sector[i + 26]);

                // Size
                cur.setSize((sector[i + 31] * (1 << 7) * (1 << 7) * (1 << 7) * 8) + (sector[i + 30] * (1 << 7) * (1 << 7) * 4) + (sector[i + 29] * (1 << 7) * 2) + sector[i + 28]);

                result.push_back(cur);
                name = "";
            }
        }
        if (start != 0) start = 0;
    }

    CloseHandle(device);
    return result;
}