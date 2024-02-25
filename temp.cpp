#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#include <stdio.h>
#include <vector>
#include <iomanip>
#include <math.h>
#include <cstdint>

class Drive {
public:
    Drive() = default;

protected:
    int BytesPerSector;
    int SectorsPerCluster;
    int SectorsPerTrack;
    int NumOfHeads;

    int NextReadingOffset;
};

class Fat32 : public Drive {
public:
    Fat32() = default;

private:
    LPCWSTR drive;
    int SectorsPerBootSector;
    int NumOfFAT;
    int SizeOfVolume;
    int SectorsPerFAT;
    int StartOfRDET;
    int SizeOfRDET;
};

class NTFS : public Drive {
public:
    NTFS() = default;

private:
    int ReservedSectors;
    int HiddenSectors;
    int TotalSectors;
    int StartOfMFT;
    int StartOfMFTMirr;

};

class Entry {
private:
    std::string name; // Name
    std::string ext; // Extension
    std::string attr; // Attribute
    std::string time; // Last update time
    std::string date; // Last update date
    int cluster; // Starting cluster
    int size; // Size
    BYTE inByte[32];
public:
    void setByte(BYTE sector[], int pos) {
        for (int i = 0; i < 32; i++) {
            this->inByte[i] = sector[pos + i];
        }
    }
    void formatName() {
        int len = this->name.length(), index = 0;
        char* temp = new char [len + 1];
        for (char c : this->name) temp[index++] = c;
        index--;
        while (index > -1 && this->name[index] == ' ') index--;
        temp[index + 1] = '\0';
        this->name = temp;
        delete []temp;
    }

    void setName(std::string name) {
        this->name = name;
    }

    void setExt(std::string ext) {
        this->ext = ext;
    }

    void setAttr(std::string attr) {
        this->attr = attr;
    }

    void setTime(std::string time) {
        this->time = time;
    }

    void setDate(std::string date) {
        this->date = date;
    }

    void setCluster(int clus) {
        this->cluster = clus;
    }

    void setSize(int size) {
        this->size = size;
    }

    std::string getAttr() {
        return this->attr;
    }

    friend std::ostream& operator << (std::ostream&, const Entry&);
};

// Sua lai cac thuoc tinh cua MFT entry
class MFTEntry {
private:
    std::string name; // Name
    std::string ext; // Extension
    std::string attr; // Attribute
    std::string time; // Last update time
    std::string date; // Last update date
    int cluster; // Starting cluster
    int size; // Size
public:
    void formatName() {
        int len = this->name.length(), index = 0;
        char* temp = new char [len + 1];
        for (char c : this->name) temp[index++] = c;
        index--;
        while (index > -1 && this->name[index] == ' ') index--;
        temp[index + 1] = '\0';
        this->name = temp;
        delete []temp;
    }

    void setName(std::string name) {
        this->name = name;
    }

    void setExt(std::string ext) {
        this->ext = ext;
    }

    void setAttr(std::string attr) {
        this->attr = attr;
    }

    void setTime(std::string time) {
        this->time = time;
    }

    void setDate(std::string date) {
        this->date = date;
    }

    void setCluster(int clus) {
        this->cluster = clus;
    }

    void setSize(int size) {
        this->size = size;
    }

    std::string getAttr() {
        return this->attr;
    }

    friend std::ostream& operator << (std::ostream&, const Entry&);
};

std::ostream& operator << (std::ostream& out, const Entry& cur) {
    out << "Name: " << cur.name << std::endl;
    if (cur.ext != "") out << "Extension: " << cur.ext << std::endl;
    out << "Attribute: " << cur.attr << std::endl;
    out << "Last modified time: " << cur.date << ", " << cur.time << std::endl;
    out << "Starting cluster: " << cur.cluster << std::endl;
    out << "Size: " << cur.size << std::endl;
    return out;
}

void printFAT32BootSector(BYTE sector[]) {
    printf("FAT32 PARTITION Boot Sector:\n");
    printf(" Offset    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    for (int i = 0; i < 512; i += 16) {
        std::cout << std::hex << std::setfill('0') << std::setw(7) << i << "   ";
        for (int j = 0; j < 16; j++) {
            if (i + j < 512) {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(sector[i + j]) << " ";
            }
            else {
                std::cout << "   ";
            }
        }
        std::cout << "  ";
        for (int j = 0; j < 16; j++) {
            if (i + j < 512) {
                BYTE c = sector[i + j];
                if (c >= 32 && c <= 126) {
                    std::cout << c;
                }
                else {
                    std::cout << ".";
                }
            }
        }
        std::cout << "\n";
    }

}

int ReadSector(LPCWSTR  drive, int readPoint, BYTE sector[512])
{
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFileW(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        0,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) // Open Error
    {
        printf("CreateFile: %u\n", GetLastError());
        return 1;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);//Set a Point to Read

    if (!ReadFile(device, sector, 512, &bytesRead, NULL))
    {
        printf("ReadFile: %u\n", GetLastError());
    }
    else
    {
        printf("Success!\n");
    }
    CloseHandle(device);
    return 0;
}

long int fourBytesToInt(BYTE entry[], int start) { // Little endian
    return (entry[start + 3] * (1 << 8) * (1 << 8) * (1 << 8)) + (entry[start + 2] * (1 << 8) * (1 << 8)) + (entry[start + 1] * (1 << 8)) + entry[start];
}

long int eightBytesToInt(BYTE entry[], int start) { // Little endian
    long int temp = (entry[start + 7] * (1 << 8) * (1 << 8) * (1 << 8)) + (entry[start + 6] * (1 << 8) * (1 << 8)) + (entry[start + 5] * (1 << 8)) + entry[start + 4];
    temp *= pow(2, 32);
    return temp + (entry[start + 3] * (1 << 8) * (1 << 8) * (1 << 8)) + (entry[start + 2] * (1 << 8) * (1 << 8)) + (entry[start + 1] * (1 << 8)) + entry[start];
}

std::wstring utf8ToUtf16(std::string input) {
    std::wstring strOut;
    int i = 0, len = input.length();
    while(i < len) { 
        strOut += input[i++]; 
    }
    return strOut;
}

uint64_t rdetStartPoint(BYTE bootSector[]) {
    uint64_t sb = bootSector[15] * (1 << 7) * 2 + bootSector[14];
    uint64_t nf = bootSector[16];
    uint64_t sf = bootSector[37] * (1 << 7) * 2 + bootSector[36];
    uint64_t sc = bootSector[13];
    uint64_t k = bootSector[45] * (1 << 7) * 2 + bootSector[44];
    return sb + sf * nf + (k - 2) * sc;
}

uint64_t sdetStartPoint(BYTE bootSector[], int cluster) {
    uint64_t sb = bootSector[15] * (1 << 7) * 2 + bootSector[14];
    uint64_t nf = bootSector[16];
    uint64_t sf = bootSector[37] * (1 << 7) * 2 + bootSector[36];
    uint64_t sc = bootSector[13];
    return sb + sf * nf + (cluster - 2) * sc;
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

std::vector<Entry> readSDET(LPCWSTR drive, int readPoint) {
    DWORD bytesRead;
    HANDLE device = NULL;
    BYTE sector[512];

    std::vector<Entry> result;
    result.clear();

    device = CreateFileW(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN); // Start reading from readPoint
    bool isStopped = false, hasSubEntry = false;
    std::string name = "";

    while (!isStopped && ReadFile(device, sector, 512, &bytesRead, NULL)) {
        // Iterate through all entries of current sector
        for (int i = 64; i < 512; i += 32) {
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
                cur.setCluster(sector[i + 21] * (1 << 7) * 2 + sector[i + 20] + sector[i + 27] * (1 << 7) * 2 + sector[i + 26]);

                // Size
                cur.setSize((sector[i + 31] * (1 << 7) * (1 << 7) * (1 << 7) * 8) + (sector[i + 30] * (1 << 7) * (1 << 7) * 4) + (sector[i + 29] * (1 << 7) * 2) + sector[i + 28]);

                result.push_back(cur);
                name = "";
            }
        }
    }

    CloseHandle(device);
    return result;
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

long int VBRStartPoint(BYTE mbr[]) {
    return fourBytesToInt(mbr, 38);
}

uint64_t MFTStartPoint(BYTE vbr[]) {
    uint64_t sc = vbr[13]; // sector / cluster
    uint64_t k = eightBytesToInt(vbr, 48); // Starting cluster
    std::cout << k << " " << sc << std::endl;
    return k * sc;
}

std::vector<MFTEntry> readMFT(LPCWSTR drive, uint64_t readPoint) {
    DWORD bytesRead;
    HANDLE device = NULL;
    BYTE sector[1024];

    std::vector<MFTEntry> result;
    result.clear();

    device = CreateFileW(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    LARGE_INTEGER li;
    li.QuadPart = readPoint;
    SetFilePointerEx(device, li, 0, FILE_BEGIN);

    // SetFilePointer(device, readPoint, NULL, FILE_BEGIN); // Start reading from readPoint
    ReadFile(device, sector, 1024, &bytesRead, NULL);

    int MFTsize = fourBytesToInt(sector, 28);
    std::cout << "Size = " << MFTsize << std::endl; 
    printFAT32BootSector(sector);
    
    CloseHandle(device);
    return result;
}

int main(int argc, char ** argv) {
    BYTE sector[512];
    // ReadSector(L"\\\\.C:", 0, sector);

    // long int vstart = VBRStartPoint(sector) * 512;

    ReadSector(L"\\\\.\\C:", 0, sector);
    uint64_t start = MFTStartPoint(sector) * 512;
    printFAT32BootSector(sector);
    std::cout << start << std::endl;

    std::vector<MFTEntry> res;
    res = readMFT(L"\\\\.\\C:", start);

    return 0;
}

/*int main(int argc, char ** argv)
{
    BYTE sector[512];
    ReadSector(L"\\\\.\\B:", 0, sector);

    int start = rdetStartPoint(sector) * 512;

    // std::vector<Entry> res;
    // res = readRDETSDET(L"\\\\.\\B:", start, true);
    // for (int i = 0; i < res.size(); i++) std::cout << res[i] << std::endl;

    // uint64_t start2 = sdetStartPoint(sector, 10) * 512;
    uint64_t start2 = sdetStartPoint(sector, 22) * 512;
    // uint64_t start2 = sdetStartPoint(sector, 157735) * 512;
    // std::cout << sdetStartPoint(sector, 26665) << std::endl;
    // uint64_t start2 = sdetStartPoint(sector, 28871) * 512;
    // uint64_t start2 = sdetStartPoint(sector, 159941) * 512;
    std::vector<Entry> res2;
    res2 = readRDETSDET(L"\\\\.\\B:", start, false);
    // for (int i = 0; i < res2.size(); i++) std::cout << res2[i] << std::endl;
    printFileAndFolder(res2);

    return 0;
}*/