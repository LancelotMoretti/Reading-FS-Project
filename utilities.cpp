#include "utilities.hpp"

int ReadSector(LPCWSTR drive, int readPoint, BYTE sector[], int bytesPerSector) {
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
        return false;
    }
    else {
        std::cout << "Success!" << std::endl;
    }
    return true;
}