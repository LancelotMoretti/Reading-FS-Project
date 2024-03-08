#include "utilities.hpp"

bool readSector(LPCWSTR drive, uint64_t readPoint, BYTE* sector, uint64_t bytesPerSector) {
    uint64_t retCode = 0;
    DWORD bytesRead;
    HANDLE device = NULL;

    device = CreateFileW(drive,    // Drive to open
        GENERIC_READ,           // Access mode
        FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
        NULL,                   // Security Descriptor
        OPEN_EXISTING,          // How to create
        FILE_FLAG_BACKUP_SEMANTICS,                      // File attributes
        NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) {   // Open Error
        std::cout << "CreateFile: " << GetLastError() << std::endl;
        return false;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);    //Set a Point to Read

    if (!ReadFile(device, sector, bytesPerSector, &bytesRead, NULL)) {
        std::cout << "ReadFile: " << GetLastError() << std::endl;
        CloseHandle(device);
        return false;
    }
    else {
        std::cout << "Success!" << std::endl;
    }
    CloseHandle(device);
    return true;
}

void printSectorTable(BYTE sector[]) {
    printf("FAT32 PARTITION Boot Sector:\n");
    printf(" Offset    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    for (uint64_t i = 0; i < 512; i += 16) {
        std::cout << std::hex << std::setfill('0') << std::setw(7) << i << "   ";
        for (uint64_t j = 0; j < 16; j++) {
            if (i + j < 512) {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint64_t>(sector[i + j]) << " ";
            }
            else {
                std::cout << "   ";
            }
        }
        std::cout << "  ";
        for (uint64_t j = 0; j < 16; j++) {
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

void printSectorNum(BYTE sector[], int numByte) {

    std::cout << "  Offset    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F" << std::endl;
    for (int i = 0; i < numByte; i += 16) {
        std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << i << "   ";
        for (int j = 0; j < 16; j++) {
            if (i + j < numByte) {
                std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<int>(sector[i + j]) << " ";
            }
            else {
                std::cout << "   ";
            }
        }

        std::cout << "  ";
        for (int j = 0; j < 16; j++) {
            if (i + j < numByte) {
                BYTE c = sector[i + j];
                if (c >= 32 && c <= 126) std::cout << c; // Ascii letters
                else std::cout << "."; // Not ascii letters
            }
        }
        std::cout << std::endl;
    }
}

std::wstring byteToWString(std::vector<BYTE> input) {
    std::wstring wide(reinterpret_cast<wchar_t*>(input.data()), input.size() / 2);
    return wide;
}

std::string wcharToUtf8(const std::wstring& unicode) {
    uint64_t uni_len = unicode.length();
    std::string utf8;
    for (uint64_t i = 0; i < uni_len; i++) {
        if (unicode[i] < 0x80) utf8 += unicode[i];
        else if (unicode[i] < 0x800) {
            utf8 += (0xc0 | (unicode[i] >> 6));
            utf8 += (0x80 | (unicode[i] & 0x3f));
        } 
        else if (unicode[i] < 0x10000) {
            utf8 += (0xe0 | (unicode[i] >> 12));
            utf8 += (0x80 | ((unicode[i] >> 6) & 0x3f));
            utf8 += (0x80 | (unicode[i] & 0x3f));
        }
        else if (unicode[i] < 0x110000) {
            utf8 += (0xf0 | (unicode[i] >> 18));
            utf8 += (0x80 | ((unicode[i] >> 12) & 0x3f));
            utf8 += (0x80 | ((unicode[i] >> 6) & 0x3f));
            utf8 += (0x80 | (unicode[i] & 0x3f));
        }
    }
    return utf8;
}

void printFileAndFolder(std::vector<Entry> vect) {
    bool isPrinted = false;
    for (uint64_t i = 0; i < vect.size(); i++) {
        if (vect[i].getAttr() == "Archive" || vect[i].getAttr() == "Subdirectory") {
            std::cout << vect[i] << std::endl;
            isPrinted = true;
        }
    }
    if (!isPrinted) std::cout << "No file or folder here!" << std::endl;
}

uint64_t rdetStartPoint(BYTE bootSector[]) {
    uint64_t sb = bootSector[15] * (1 << 7) * 2 + bootSector[14];
    uint64_t nf = bootSector[16];
    uint64_t sf = bootSector[37] * (1 << 7) * 2 + bootSector[36];
    uint64_t sc = bootSector[13];
    uint64_t k = bootSector[45] * (1 << 7) * 2 + bootSector[44];
    return sb + sf * nf + (k - 2) * sc;
}

uint64_t sdetStartPoint(BYTE bootSector[], uint64_t cluster) {
    uint64_t sb = bootSector[15] * (1 << 7) * 2 + bootSector[14];
    uint64_t nf = bootSector[16];
    uint64_t sf = bootSector[37] * (1 << 7) * 2 + bootSector[36];
    uint64_t sc = bootSector[13];
    return sb + sf * nf + (cluster - 2) * sc;
}

std::vector<Entry> readRDETSDET(LPCWSTR drive, uint64_t readPoint, bool isRDET) {
    uint64_t start = isRDET ? 0 : 64; // True: RDET, False: SDET

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
        printSectorTable(sector);
        // Iterate through all entries of current sector
        for (uint64_t i = start; i < 512; i += 32) {
            if (sector[i] == 0xE5) continue; // Deleted entry
            if (sector[i] == 0x00) { // End of RDET table
                isStopped = true;
                break;
            }

            if (sector[i + 11] == 0x0F) { // Sub entry
                std::string tempName = "";
                for (uint64_t j = 1; j < 11; j++) tempName += sector[i + j];
                for (uint64_t j = 14; j < 26; j++) tempName += sector[i + j];
                for (uint64_t j = 28; j < 32; j++) tempName += sector[i + j];
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
                    for (uint64_t j = 0; j < 8; j++) name += sector[i + j];
                    cur.setName(name); // name
                }
                cur.FormatName();

                // Set extension
                std::string ext = "";
                for (uint64_t j = 0; j < 3; j++) ext += (sector[i + 8 + j]);
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

int32_t fourBytesToInt(BYTE entry[], uint64_t start) { // Little endian
    return (entry[start + 3] * (1 << 8) * (1 << 8) * (1 << 8)) + (entry[start + 2] * (1 << 8) * (1 << 8)) + (entry[start + 1] * (1 << 8)) + entry[start];
}

uint64_t eightBytesToInt(BYTE entry[], uint64_t start) { // Little endian
    uint64_t temp = (entry[start + 7] * (1 << 8) * (1 << 8) * (1 << 8)) + (entry[start + 6] * (1 << 8) * (1 << 8)) + (entry[start + 5] * (1 << 8)) + entry[start + 4];
    temp *= pow(2, 32);
    return temp + (entry[start + 3] * (1 << 8) * (1 << 8) * (1 << 8)) + (entry[start + 2] * (1 << 8) * (1 << 8)) + (entry[start + 1] * (1 << 8)) + entry[start];
}

uint64_t nBytesToNum(BYTE entry[], uint64_t start, int numBytes) {
    uint64_t result = 0;
    for (uint64_t i = 0; i < numBytes; i++) {
        result += entry[start + i] * (uint64_t)pow(2, 8 * i);
    }
    return result;
}

int32_t VBRStartPoint(BYTE mbr[]) {
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
    printSectorTable(sector);
    
    CloseHandle(device);
    return result;
}

std::map<int, std::string> flagToMeaningMapping = {
    {0x0001, "Read Only"},
    {0x0002, "Hidden"},
    {0x0004, "System"},
    {0x0020, "Archive"},
    {0x0040, "Device"},
    {0x0080, "Normal"},
    {0x0100, "Temporary"},
    {0x0200, "Sparse File"},
    {0x0400, "Reparse Point"},
    {0x0800, "Compressed"},
    {0x1000, "Offline"},
    {0x2000, "Not Content Indexed"},
    {0x4000, "Encrypted"},
    {0x10000000, "Directory"},
    {0x20000000, "Index View"},
    {0x40000000, "Virtual"}
};

std::string readSTD_INFO(BYTE sector[], uint64_t stdInfoStart) {
    uint64_t STD_INFO_ContentStart = nBytesToNum(sector, stdInfoStart + 0x14, 2);
    uint64_t STD_INFO_Flag = fourBytesToInt(sector, STD_INFO_ContentStart + 0x20);
    return flagToMeaningMapping[STD_INFO_Flag];
}

bool readATTRIBUTE_LIST(BYTE sector[], uint64_t attributeListStart) {

}

bool readFILE_NAME(BYTE sector[], uint64_t fileNameStart) {
    if (sector[fileNameStart + 8] == 0) {
        //resident
        int nameLength = sector[fileNameStart + 0x58];
        int nameNamespace = sector[fileNameStart + 0x59];
        BYTE name[nameLength * 2];
        for (int i = 0; i < nameLength * 2; i++) {
            name[i] = sector[fileNameStart + 0x5A + i];
        }
        //read the name based on nameSpace, like
        //std::string readFileName(BYTE name[]);
        return true;
    }
    else {
        //non-resident
        //I'll find a way to solve this later, for it's rare to encounter this
        return false;
    }
}

std::vector<MFTEntry> readNTFSTree(LPCWSTR drive, std::vector<uint64_t> listEntries) {
    DWORD bytesRead;
    HANDLE device = NULL;
    BYTE sector[1024];
    device = CreateFileW(drive, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

    std::vector<MFTEntry> result;
    result.clear();

    for (int i = 0; i < listEntries.size(); i++) {
        uint64_t readPoint = listEntries[i];
        LONG high = readPoint >> 32;
        LONG low = readPoint;
        SetFilePointer(device, low, &high, FILE_BEGIN); // Start reading from readPoint
        ReadFile(device, sector, 1024, &bytesRead, NULL);
        //read $STANDARD_INFORMATION
        uint64_t stdInfoStart = nBytesToNum(sector, 0x14, 2);
        std::cout << readSTD_INFO(sector, stdInfoStart) << std::endl;
        //check if there is $ATTRIBUTE_LIST
        uint64_t stdInfoSkipOffset = fourBytesToInt(sector, stdInfoStart + 4);
        uint64_t nextAttribute = stdInfoStart + stdInfoSkipOffset;
        if (fourBytesToInt(sector, nextAttribute) == 48) {
            //$FILE_NAME exists

        }
        else {
            //parse $ATTRIBUTE_LIST to find $FILE_NAME
            
        }
    }
    
    CloseHandle(device);
    return result;
}