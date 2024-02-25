#include "utilities.hpp"

int readSector(LPCWSTR drive, int readPoint, BYTE* sector, int bytesPerSector) {
    int retCode = 0;
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