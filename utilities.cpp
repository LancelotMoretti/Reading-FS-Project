#include "utilities.hpp"

bool readSector(HANDLE device, uint64_t readPoint, BYTE* sector, uint64_t bytesPerSector) {
    uint64_t retCode = 0;
    DWORD bytesRead;
    // HANDLE device = NULL;

    // device = CreateFileW(volume,    // Volume to open
    //     GENERIC_READ,           // Access mode
    //     FILE_SHARE_READ | FILE_SHARE_WRITE,        // Share Mode
    //     NULL,                   // Security Descriptor
    //     OPEN_EXISTING,          // How to create
    //     FILE_FLAG_BACKUP_SEMANTICS,                      // File attributes
    //     NULL);                  // Handle to template

    if (device == INVALID_HANDLE_VALUE) {   // Open Error
        std::wcout << "CreateFile: " << GetLastError() << std::endl;
        return false;
    }

    SetFilePointer(device, readPoint, NULL, FILE_BEGIN);    //Set a Point to Read

    if (!ReadFile(device, sector, bytesPerSector, &bytesRead, NULL)) {
        std::wcout << "ReadFile: " << GetLastError() << std::endl;
        return false;
    }
    return true;
}

void printSectorTable(BYTE sector[]) {
    printf("FAT32 PARTITION Boot Sector:\n");
    printf(" Offset    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    for (uint64_t i = 0; i < 512; i += 16) {
        std::wcout << std::hex << std::setfill(L'0') << std::setw(7) << i << "   ";
        for (uint64_t j = 0; j < 16; j++) {
            if (i + j < 512) {
                std::wcout << std::hex << std::setfill(L'0') << std::setw(2) << static_cast<uint64_t>(sector[i + j]) << " ";
            }
            else {
                std::wcout << "   ";
            }
        }
        std::wcout << "  ";
        for (uint64_t j = 0; j < 16; j++) {
            if (i + j < 512) {
                BYTE c = sector[i + j];
                if (c >= 32 && c <= 126) {
                    std::wcout << wchar_t(c);
                }
                else {
                    std::wcout << ".";
                }
            }
        }
        std::wcout << "\n";
    }

}

void printSectorNum(BYTE sector[], int numByte) {

    std::wcout << L"  Offset    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F" << std::endl;
    for (int i = 0; i < numByte; i += 16) {
        std::wcout << std::hex << std::uppercase << std::setfill(L'0') << std::setw(8) << i << L"   ";
        for (int j = 0; j < 16; j++) {
            if (i + j < numByte) {
                std::wcout << std::hex << std::uppercase << std::setfill(L'0') << std::setw(2) << static_cast<int>(sector[i + j]) << L" ";
            }
            else {
                std::wcout << L"   ";
            }
        }

        std::wcout << L"  ";
        for (int j = 0; j < 16; j++) {
            if (i + j < numByte) {
                unsigned char c = sector[i + j];
                if (c >= 32 && c <= 126) std::cout << std::dec << c; // Ascii letters ??????????????
                else std::wcout << L"."; // Not ascii letters
            }
        }
        std::wcout << std::endl;
    }
}

std::wstring byteToWString(std::vector<BYTE> input, int wSize) {
    std::wstring wide(reinterpret_cast<wchar_t*>(input.data()), wSize);
    return wide;
}

std::string fourBytesToString(BYTE entry[], int start) { // Little endian
    std::string result = "";
    for (int i = 0; i < 4; i++) {
        result += entry[start + i];
    }
    return result;
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
    int size = static_cast<int>(vect.size());
    for (int i = 0; i < size; i++) {
        if (vect[i].getAttr() == L"Archive" || vect[i].getAttr() == L"Subdirectory") {
            std::wcout << "Position: " << std::dec << i << std::endl;
            std::wcout << vect[i] << std::endl;
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

std::map<int, std::wstring> flag32ToMeaningMapping = {
    {0x01, L"Read Only"},
    {0x02, L"Hidden"},
    {0x04, L"System"},
    {0x08, L"Volume Label"},
    {0x10, L"Subdirectory"},
    {0x20, L"Archive"},
    {0x40, L"Device"},
    {0x80, L"Unused"},
};

std::vector<Entry> readRDETSDET(HANDLE device, uint64_t readPoint, bool isRDET) {
    int start = isRDET ? 0 : 64; // True: RDET, False: SDET

    DWORD bytesRead;
    BYTE sector[512];

    std::vector<Entry> result;
    result.clear();

    LONG high = readPoint >> 32;
    LONG low = readPoint;

    SetFilePointer(device, low, &high, FILE_BEGIN); // Start reading from readPoint
    bool isStopped = false, hasSubEntry = false;
    std::wstring name = L"";

    while (!isStopped && ReadFile(device, sector, 512, &bytesRead, NULL)) {
        for (int i = start; i < 512; i += 32) {
            if (sector[i] == 0xE5) continue; // Deleted entry
            if (sector[i] == 0x00) { // End of RDET table
                isStopped = true;
                break;
            }

            if (sector[i + 11] == 0x0F) { // Sub entry
                std::wstring tempName = L"";
                for (int j = 1; j < 11; j += 2) if (sector[i + j] != 0x00 && sector[i + j] != 0xFF) {
                    tempName += byteToWString(std::vector<BYTE>(sector + i + j, sector + i + j + 1), 1);
                }
                for (int j = 14; j < 26; j += 2) if (sector[i + j] != 0x00 && sector[i + j] != 0xFF) {
                    tempName += byteToWString(std::vector<BYTE>(sector + i + j, sector + i + j + 1), 1);
                }
                for (int j = 28; j < 32; j += 2) if (sector[i + j] != 0x00 && sector[i + j] != 0xFF) {
                    tempName += byteToWString(std::vector<BYTE>(sector + i + j, sector + i + j + 1), 1);
                }
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
                cur.FormatName();

                // Set extension
                std::wstring ext = L"";
                for (int j = 0; j < 3; j++) ext += (sector[i + 8 + j]);
                if (sector[i + 11] == 0x10) cur.setExt(L""); // Folder
                else cur.setExt(ext); // Others

                // Attribute
                std::wstring attr = L"";
                for (int j = 0; j < 8; j++) {
                    if (sector[i + 11] & (1 << j)) {
                        attr += flag32ToMeaningMapping[1 << j] + L" | ";
                    }
                }
                attr.erase(attr.length() - 3, 3);
                cur.setAttr(attr);

                // Time
                std::wstring curTime = std::to_wstring(sector[i + 23] >> 3) + L":"; // Hour
                curTime += std::to_wstring((sector[i + 23] % (1 << 3)) * (1 << 3) + ((sector[i + 22] >> 5))) + L":"; // Minute
                curTime += std::to_wstring(sector[i + 22] % (1 << 5) * 2); // Second
                cur.setTime(curTime);

                // Date
                std::wstring curDate = std::to_wstring(sector[i + 24] % (1 << 5)) + L"/"; // Day
                curDate += std::to_wstring((sector[i + 25] % 2) * (1 << 3) + ((sector[i + 24] >> 5))) + L"/"; // Month
                curDate += std::to_wstring((sector[i + 25] >> 1) + 1980); // Year
                cur.setDate(curDate);

                // Cluster
                cur.setCluster(sector[i + 21] * (1 << 7) * (1 << 7) * (1 << 7) * 8 + sector[i + 20] * (1 << 7) * (1 << 7) * 4 + sector[i + 27] * (1 << 7) * 2 + sector[i + 26]);

                // Size
                cur.setSize(nBytesToNum(sector, i + 28, 4));

                result.push_back(cur);
                name = L"";
            }
        }
        if (start != 0) start = 0;
    }

    return result;
}

uint64_t nBytesToNum(BYTE entry[], uint64_t start, int numBytes) {
    uint64_t result = 0;
    for (uint64_t i = 0; i < numBytes; i++) result += entry[start + i] * (uint64_t)pow(2, 8 * i);
    return result;
}

uint64_t VBRStartPoint(BYTE mbr[]) {
    return nBytesToNum(mbr, 38, 4);
}

uint64_t MFTStartPoint(BYTE vbr[]) {
    uint64_t sc = vbr[13]; // sector / cluster
    uint64_t k = nBytesToNum(vbr, 48, 8); // Starting cluster
    return k * sc;
}

std::vector<uint64_t> readFolder(HANDLE device, uint64_t readPoint) { 
    // This funtion returns list of entries of all files and folders in a directory

    DWORD bytesRead;
    BYTE sector[1024];

    std::vector<uint64_t> result;
    result.clear();

    LONG high = readPoint >> 32;
    LONG low = readPoint;
    SetFilePointer(device, low, &high, FILE_BEGIN); // Start reading from readPoint  
    ReadFile(device, sector, 1024, &bytesRead, NULL);
    printSectorNum(sector, 1024);

    // Get first attribute's position
    uint64_t offAttr = nBytesToNum(sector, 0x14, 2);
    // Find $INDEX_ROOT entry
    while (offAttr < 1024 && nBytesToNum(sector, offAttr, 4) != uint64_t(144)) { // Check 4 first bytes to get attr's ID
        uint64_t size = nBytesToNum(sector, offAttr + uint64_t(4), 4); // Get next 4 bytes to get attr's size
        if (size == 0) {
            return result;
        }
        offAttr += size;
    }
    if (offAttr >= 1024) { // No $INDEX_ROOT
        return result;
    }
    uint64_t offRoot = offAttr;

    // Find $INDEX_ALLOCATION entry
    uint64_t offAllo;
    while (offAttr < 1024 && nBytesToNum(sector, offAttr, 4) != uint64_t(160)) { // Check 4 first bytes to get attr's ID
        uint64_t size = nBytesToNum(sector, offAttr + uint64_t(4), 4); // Get next 4 bytes to get attr's size
        if (size == 0) {
            offAllo = 0xFFFFFFFFFFFFFFFF;
            break;
        }
        offAttr += size;
    }
    offAllo = offAttr >= 1024 ? 0xFFFFFFFFFFFFFFFF : offAttr; // Check if $INDEX_ALLOCATION exists or not

    // Find $BITMAP entry
    uint64_t offBitm;
    while (offAttr < 1024 && nBytesToNum(sector, offAttr, 4) != uint64_t(176)) { // Check 4 first bytes to get attr's ID
        uint64_t size = nBytesToNum(sector, offAttr + uint64_t(4), 4); // Get next 4 bytes to get attr's size
        if (size == 0) {
            offBitm = 0xFFFFFFFFFFFFFFFF;
            break;
        }
        offAttr += size;
    }
    offBitm = offAttr >= 1024 ? 0xFFFFFFFFFFFFFFFF : offAttr;

    // Read $BITMAP
    std::vector<uint64_t> isUsedVCN;
    if (offBitm != 0xFFFFFFFFFFFFFFFF) {
        uint64_t bitmSize = nBytesToNum(sector, offBitm + uint64_t(4), 4);
        for (uint64_t i = 0; i < bitmSize; i++) {
            BYTE cur = sector[offBitm + uint64_t(32) + i];
            for (int j = 0; j < 8; j++) {
                bool isUp = cur & (1 << j);
                uint64_t num = static_cast<uint64_t>(i) * 8 + static_cast<uint64_t>(j);
                if (isUp) isUsedVCN.push_back(num); // Push current VCN if it is occupied
            }
        }
    }

    // Read $INDEX_ROOT
    uint64_t offContent = nBytesToNum(sector, offRoot + 20, 2); // Offset to the content of this attribute
    uint64_t offEntryRoot = nBytesToNum(sector, offRoot + offContent + uint64_t(16), 4) + offRoot + offContent + uint64_t(16);
    uint64_t endEntryRoot = nBytesToNum(sector, offRoot + offContent + uint64_t(20), 4) + offRoot + offContent + uint64_t(16);
    std::vector<uint64_t> listVCN;
    while (offEntryRoot <= endEntryRoot) {
        uint64_t curFile = nBytesToNum(sector, offEntryRoot, 6);

        uint64_t fileLen = nBytesToNum(sector, offEntryRoot + uint64_t(8), 2);
        if (fileLen == 0) break;

        uint64_t flag = nBytesToNum(sector, offEntryRoot + uint64_t(12), 4);
        if (flag != uint64_t(2) && flag != uint64_t(3)) result.push_back(curFile);

        uint64_t num = nBytesToNum(sector, offEntryRoot + fileLen - uint64_t(8), 8);
        if (flag == uint64_t(1) || flag == uint64_t(3)) {
            if (offBitm != 0xFFFFFFFFFFFFFFFF) {
                for (int i = 0; i < isUsedVCN.size(); i++) {
                    if (num == isUsedVCN[i]) {
                        listVCN.push_back(num); // Last 8 bytes 
                        isUsedVCN[i] = isUsedVCN[isUsedVCN.size() - 1];
                        isUsedVCN.pop_back();
                        break;
                    }
                }
            } else listVCN.push_back(num);
        } else if (flag == uint64_t(2) || flag == uint64_t(3)) break;
        offEntryRoot += fileLen; // Next index entry
    }

    // Read $INDEX_ALLOCATE
    if (offAllo != 0xFFFFFFFFFFFFFFFF) {
        uint64_t offRunlist = nBytesToNum(sector, offAllo + uint64_t(32), 2) + offAllo;
        uint64_t endAllo = nBytesToNum(sector, offAllo + uint64_t(4), 4) + offAllo;
        std::vector<std::pair<uint64_t, int>> listClusters;

        // Get list of offsets of all runlists
        while (offRunlist <= endAllo) {
            if (sector[offRunlist] == 0) break;

            int curByte = static_cast<int>(sector[offRunlist]);
            int runLen = curByte % 16;
            int offField = curByte / 16;
            uint64_t curCls = nBytesToNum(sector, offRunlist + static_cast<uint64_t>(runLen) + uint64_t(1), static_cast<uint64_t>(offField));
            int numCls = static_cast<uint64_t>(nBytesToNum(sector, offRunlist + uint64_t(1), static_cast<uint64_t>(runLen)));
            listClusters.push_back(std::make_pair(uint64_t(curCls), numCls));
            offRunlist += static_cast<uint64_t>(runLen) + static_cast<uint64_t>(offField) + uint64_t(1);
        }

        // Read VCN to get all files
        for(int i = 0; i < listClusters.size(); i++) {
            uint64_t curRun = listClusters[i].first * 8 * 512;
            high = curRun >> 32;
            low = curRun;
            SetFilePointer(device, low, &high, FILE_BEGIN);

            // Go through each cluster
            for (int j = 0; j < listClusters[i].second; j++) {
                BYTE clsCurrent[4096];
                ReadFile(device, clsCurrent, 4096, &bytesRead, NULL);
                printSectorNum(clsCurrent, 4096);
                    
                // Each cluster is referenced to a VCN, which means an Index record
                uint64_t curVCN = nBytesToNum(clsCurrent, uint64_t(16), 8);
                for (int k = 0; k < listVCN.size(); k++) {
                    if (curVCN == listVCN[k]) { // Check VCN
                        listVCN[k] = listVCN[listVCN.size() - 1];
                        listVCN.pop_back();

                        // Read Index record
                        uint64_t offEntryAllo = nBytesToNum(clsCurrent, uint64_t(24), 4) + uint64_t(24);
                        uint64_t endEntryAllo = nBytesToNum(clsCurrent, uint64_t(28), 4) + uint64_t(24);

                        // Read all entries of an Index record
                        // std::vector<uint64_t> listVCN;
                        while (offEntryAllo <= endEntryAllo) {
                            uint64_t curFile = nBytesToNum(clsCurrent, offEntryAllo, 6);

                            // Get file's length
                            uint64_t fileLen = nBytesToNum(clsCurrent, offEntryAllo + uint64_t(8), 2);
                            if (fileLen == 0) break;
                            std::wcout << std::hex << offEntryAllo << " " << fileLen << std::endl;

                            // Get flag state
                            uint64_t flag = nBytesToNum(clsCurrent, offEntryAllo + uint64_t(12), 4);
                            if (flag != uint64_t(2) && flag != uint64_t(3)) result.push_back(curFile);


                            if (flag == uint64_t(1) || flag == uint64_t(3)) {
                                uint64_t num = nBytesToNum(clsCurrent, offEntryAllo + fileLen - uint64_t(8), 8);
                                if (offBitm != 0xFFFFFFFFFFFFFFFF) {
                                    for (int l = 0; l < isUsedVCN.size(); l++) {
                                        if (num == isUsedVCN[l]) {
                                            listVCN.push_back(num); // Last 8 bytes 
                                            isUsedVCN[l] = isUsedVCN[isUsedVCN.size() - 1];
                                            isUsedVCN.pop_back();
                                            break;
                                        }
                                    }
                                } else listVCN.push_back(num);
                            } else if (flag == uint64_t(2) || flag == uint64_t(3)) break;

                            offEntryAllo += fileLen; // Next index entry
                        }
                        break;
                    }
                }
            }
        }
    }

    return result;
}

std::map<int, std::wstring> flagToMeaningMapping = {
    {0x0001, L"Read Only"},
    {0x0002, L"Hidden"},
    {0x0004, L"System"},
    {0x0020, L"Archive"},
    {0x0040, L"Device"},
    {0x0080, L"Normal"},
    {0x0100, L"Temporary"},
    {0x0200, L"Sparse File"},
    {0x0400, L"Reparse Point"},
    {0x0800, L"Compressed"},
    {0x1000, L"Offline"},
    {0x2000, L"Not Content Indexed"},
    {0x4000, L"Encrypted"},
    {0x10000000, L"Directory"},
    {0x20000000, L"Index View"},
    {0x40000000, L"Virtual"}
};

std::wstring readSTD_INFO(BYTE sector[], uint64_t stdInfoStart) {
    uint64_t STD_INFO_ContentStart = nBytesToNum(sector, stdInfoStart + 0x14, 2);
    uint64_t STD_INFO_Flag = nBytesToNum(sector, STD_INFO_ContentStart + 0x20, 4);
    return flagToMeaningMapping[STD_INFO_Flag];
}

bool readATTRIBUTE_LIST(BYTE sector[], uint64_t attributeListStart) {
    return true;
}

bool readFILE_NAME(BYTE sector[], uint64_t fileNameStart) {
    if (sector[fileNameStart + 8] == 0) {
        //resident
        int nameLength = sector[fileNameStart + 0x58];
        int nameNamespace = sector[fileNameStart + 0x59];
        std::vector<BYTE> name(nameLength * 2);
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

std::vector<MFTEntry> readNTFSTree(HANDLE device, std::vector<uint64_t> listEntries) {
    DWORD bytesRead;
    BYTE sector[1024];

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
        std::wcout << readSTD_INFO(sector, stdInfoStart) << std::endl;
        //check if there is $ATTRIBUTE_LIST
        uint64_t stdInfoSkipOffset = nBytesToNum(sector, stdInfoStart + 4, 4);
        uint64_t nextAttribute = stdInfoStart + stdInfoSkipOffset;
        if (nBytesToNum(sector, nextAttribute, 4) == 48) {
            //$FILE_NAME exists

        }
        else {
            //parse $ATTRIBUTE_LIST to find $FILE_NAME
            
        }
    }
    
    return result;
}