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
                wchar_t c = L'a'; // Temp
                c = c >> 8; // Set to 0
                c = c << 8; // Set to 0
                c += sector[i + j]; // Add real data
                if (c >= 32 && c <= 126) std::wcout << c;
                else std::wcout << L"."; // Not ascii letters
            }
        }
        std::wcout << std::endl;
    }
}

void printFileAndFolder(std::vector<Entry> vect) {
    bool isPrinted = false;
    int size = static_cast<int>(vect.size());
    for (int i = 0; i < size; i++) {
        if (vect[i].getAttr().find(L"Archive") != std::wstring::npos || vect[i].getAttr().find(L"Subdirectory") != std::wstring::npos) {
            std::wcout << L"Position: " << std::dec << i << std::endl;
            std::wcout << vect[i] << std::endl;
            isPrinted = true;
        }
    }
    if (!isPrinted) std::wcout << L"No file or folder here!" << std::endl;
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
                wchar_t tempChar = L'\000';
                for (int j = 1; j < 32; j += 2) {
                    if (j == 11) j = 14;
                    if (j == 26) j = 28;
                    tempChar = sector[i + j + 1];
                    tempChar <<= 8;
                    tempChar += sector[i + j];
                    tempName += tempChar;
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
        if (!(flag & (1 << 1))) result.push_back(curFile);

        uint64_t num = nBytesToNum(sector, offEntryRoot + fileLen - uint64_t(8), 8);
        if (flag & 1) { // Bit 1 is turned on
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
        } else if (flag & (1 << 1)) break; // Bit second is turned on
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
                        while (offEntryAllo <= endEntryAllo) {
                            uint64_t curFile = nBytesToNum(clsCurrent, offEntryAllo, 6);

                            // Get file's length
                            uint64_t fileLen = nBytesToNum(clsCurrent, offEntryAllo + uint64_t(8), 2);
                            if (fileLen == 0) break;

                            // Get flag state
                            uint64_t flag = nBytesToNum(clsCurrent, offEntryAllo + uint64_t(12), 4);
                            if (!(flag & (1 << 1))) result.push_back(curFile);


                            if (flag & 1) { // Bit 1 is turned on
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
                            } else if (flag & (1 << 1)) break;

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

std::pair<std::wstring, std::pair<std::wstring, std::wstring>> readSTD_INFO(BYTE sector[], uint64_t offSTD) { // Flag, time, date
    uint64_t offContent = nBytesToNum(sector, offSTD + 0x14, 2);
    uint64_t flagByte = nBytesToNum(sector, offSTD + offContent + 0x20, 4);
    
    std::wstring flagWstring = L"";
    for (int i = 0; i < 32; i++) {
        if (flagByte & (1 << i)) flagWstring += L" | " + flagToMeaningMapping[1 << i];
    }

    uint64_t timeStamp = nBytesToNum(sector, offSTD + offContent + 0x10, 8);
    FILETIME fileTime;
    fileTime.dwHighDateTime = nBytesToNum(sector, offSTD + offContent + 0x14, 4);
    fileTime.dwLowDateTime = nBytesToNum(sector, offSTD + offContent + 0x10, 4);
    SYSTEMTIME systemTime;
    FileTimeToSystemTime(&fileTime, &systemTime);

    std::wstring date = std::to_wstring(systemTime.wDay) + L"/" + std::to_wstring(systemTime.wMonth) + L"/" + std::to_wstring(systemTime.wYear);
    std::wstring time = std::to_wstring(systemTime.wHour) + L":" + std::to_wstring(systemTime.wMinute) + L":" + std::to_wstring(systemTime.wSecond);

    return std::make_pair(flagWstring, std::make_pair(time, date));
}

std::pair<bool, std::wstring> readFILE_NAME(BYTE sector[], uint64_t offFileName) {
    std::wstring name; // $FILE_NAME is always a resident attribute

    int contentStart = sector[offFileName + 0x14];
    int nameLength = sector[offFileName + contentStart + 0x40];
    if (sector[offFileName + contentStart + 0x41] == 0x02) return std::make_pair(false, name); // Not this one

    for (int i = 0; i < nameLength * 2; i += 2) {
        wchar_t temp = L'a';
        temp = temp >> 8;
        temp += sector[offFileName + contentStart + 0x42 + i + 1];
        temp = temp << 8;
        temp += sector[offFileName + contentStart + 0x42 + i];

        name += temp;
    }
    
    bool isTrue = true;
    return std::make_pair(isTrue, name);
}

void formatListEntries(std::vector<uint64_t>& list) {
    for (int i = 0; i < list.size(); i++) {
        for (int j = i + 1; j < list.size(); j++) {
            if (list[i] == list[j]) {
                list[j] = list[list.size() - 1];
                list.pop_back();
                continue;
            }
        }
    }
}

uint64_t getFileSize(HANDLE device, uint64_t start, uint64_t bytePersect, uint64_t mftEntry) {
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
    uint64_t dataRunLength = 0; //Length of the data run
    uint64_t dataRunOffset = 0; //Offset to the start of the data run

    // Size of content in byte
    uint64_t fileSize = 0;

    std::vector<BYTE> buffer(1024);

    // Set pointer and read sector
    DWORD bytesRead;
    uint64_t readPoint = start + mftEntry * 1024;
    LONG high = readPoint >> 32;
    LONG low = readPoint;
    SetFilePointer(device, low, &high, FILE_BEGIN); // Start reading from readPoint
    ReadFile(device, buffer.data(), 1024, &bytesRead, NULL);

    do {
        // Read attribute type and size to jump
        attributeCode = nBytesToNum(buffer.data(), attributeOffset + 0, 4);
        attributeSize = nBytesToNum(buffer.data(), attributeOffset + 4, 4);
        nameLength = nBytesToNum(buffer.data(), attributeOffset + 9, 1);

        if (attributeCode == 0x80 && nameLength == 0) {
            isResident = buffer[attributeOffset + 8] == 0;

            if (!isResident) {
                std::vector<BYTE> content(4096);
                dataRunOffset = nBytesToNum(buffer.data(), attributeOffset + 32, 2);

                attributeOffset += dataRunOffset;
                
                do {
                    // Read length of datarun
                    dataRunLength = nBytesToNum(buffer.data(), attributeOffset, 1);
                    attributeOffset++;
                    dataSize = dataRunLength & 0x0F;
                    dataStart = dataRunLength >> 4;
                    
                    // Read run length and run offset of datarun
                    dataSize = nBytesToNum(buffer.data(), attributeOffset, dataSize);
                    attributeOffset += dataSize;
                    dataStart = nBytesToNum(buffer.data(), attributeOffset, dataStart);
                    attributeOffset += dataStart;

                    // Calculate max file size
                    fileSize += dataSize * 4096;

                    // Read last cluster
                    readPoint = (dataStart + dataSize - 1) * 4096;
                    high = readPoint >> 32;
                    low = readPoint;
                    SetFilePointer(device, low, &high, FILE_BEGIN); // Start reading from readPoint
                    ReadFile(device, content.data(), 4096, &bytesRead, NULL);

                    // Minus the unused space
                    for (int i = 0; i < content.size(); i++) {
                        if (content[i] == L'\000')
                            return fileSize - (content.size() - i);
                    }
                    
                } while (dataRunLength != 0);

                return fileSize;
            }
            else return nBytesToNum(buffer.data(), attributeOffset + 16, 4);
        } 
        else attributeOffset += attributeSize;

    } while (attributeCode != 0 && attributeOffset < 1024);
    
    return fileSize;
}

std::vector<MFTEntry> readNTFSTree(HANDLE device, uint64_t start, std::vector<uint64_t> listEntries) {
    DWORD bytesRead;
    BYTE sector[1024];

    std::vector<MFTEntry> result;
    result.clear();
    std::wstring mftType;

    for (int i = 0; i < listEntries.size(); i++) {
        MFTEntry cur;

        uint64_t readPoint = start + uint64_t(listEntries[i] * 1024);
        LONG high = readPoint >> 32;
        LONG low = readPoint;
        SetFilePointer(device, low, &high, FILE_BEGIN); // Start reading from readPoint
        ReadFile(device, sector, 1024, &bytesRead, NULL);

        // Read flag
        uint64_t curFlag = nBytesToNum(sector, 0x16, 2);
        if (curFlag == 0x01) mftType = L"File";
        else if (curFlag == 0x03) mftType = L"Folder";
        else continue; // Not file or folder

        uint64_t offAttr = nBytesToNum(sector, 0x14, 2);
        if (offAttr == 0) continue; // Not a mft entry
        
        uint64_t size;
        while (offAttr < 1024 && nBytesToNum(sector, offAttr, 4) != uint64_t(16)) { // Check 4 first bytes to get attr's ID
            size = nBytesToNum(sector, offAttr + uint64_t(4), 4); // Get next 4 bytes to get attr's size
            if (size == 0) break;
            offAttr += size;
        }
        if (offAttr >= 1024 || size == 0) continue; // No $STANDARD_INFORMATION
        uint64_t offSTD = offAttr;

        //// FILE_NAME
        std::pair<bool, std::wstring> curName;
        bool isNext = false;
        do {
            while (offAttr < 1024 && nBytesToNum(sector, offAttr, 4) != uint64_t(48)) { // Check 4 first bytes to get attr's ID
                size = nBytesToNum(sector, offAttr + uint64_t(4), 4); // Get next 4 bytes to get attr's size
                if (size == 0) break;
                offAttr += size;
            }
            if (offAttr >= 1024 || size == 0) isNext = true; // No $FILE_NAME
            uint64_t offFileName = offAttr;

            // Read $FILE_NAME
            curName = readFILE_NAME(sector, offFileName);
        } while (!curName.first && !isNext);
        if (isNext) continue;
        
        // Read $STANDARD_INFORMATION
        std::pair<std::wstring, std::pair<std::wstring, std::wstring>> timeStamp = readSTD_INFO(sector, offSTD);

        // Set data
        std::wstring tmp = L"";
        if (mftType == L"File") {
            for (int i = curName.second.size() - 1; i >= 0; i--) {
                if (curName.second[i] == L'.') {
                    for (int j = i + 1; j < curName.second.size(); j++) tmp += curName.second[j];
                    curName.second.resize(i); // Cut string
                    break;
                }
            }
        }

        // Set data for mft entry
        cur.setName(curName.second); // Set name
        if (timeStamp.first != L"") cur.setType(mftType + timeStamp.first);
        else cur.setType(mftType);
        cur.setDate(timeStamp.second.second);
        cur.setTime(timeStamp.second.first);
        cur.setEntry(listEntries[i]);
        cur.setExt(tmp);

        // Set size
        uint64_t curSize = getFileSize(device, start, 512, listEntries[i]);
        cur.setSize(curSize);

        // Push
        result.push_back(cur);
    }
    
    return result;
}

void printFileAndFolderNTFS(std::vector<MFTEntry> list) {
    for (int i = 0; i < list.size(); i++) {
        std::wcout << L"No. " << i << std::endl;
        std::wcout << list[i] << std::endl;
    }
    if (list.size() == 0) std::wcout << "There's no file or folder here!" << std::endl; 
}

void printFileAndFolderNoHidden(std::vector<MFTEntry> list) {
    int index = 1;
    for (int i = 0; i < list.size(); i++) {
        std::wstring temp = L"Hidden";
        if (list[i].getType().find(temp) != std::wstring::npos) continue;
        std::wcout << L"No. " << index++ << std::endl;
        std::wcout << list[i] << std::endl;
    }
    if (list.size() == 0) std::wcout << "There's no file or folder here!" << std::endl; 
}