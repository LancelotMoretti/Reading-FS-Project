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

int rdetStartPoint(BYTE sector[]) {
    int sb = sector[15] * (1 << 7) * 2 + sector[14];
    int nf = sector[16];
    int sf = sector[37] * (1 << 7) * 2 + sector[36];
    int sc = sector[13];
    int k = sector[45] * (1 << 7) * 2 + sector[44];
    return sb + sf * nf + (k - 2) * sc;
}

int sdetStartPoint(BYTE bootSector[], int cluster) {
    int sb = bootSector[15] * (1 << 7) * 2 + bootSector[14];
    int nf = bootSector[16];
    int sf = bootSector[37] * (1 << 7) * 2 + bootSector[36];
    int sc = bootSector[13];
    return sb + sf * nf + (cluster - 2) * sc;
}