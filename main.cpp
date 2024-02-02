#include <windows.h>
#include <iostream>

int ReadSector(LPCWSTR drive, int readPoint, BYTE sector[512]) {
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

    if (!ReadFile(device, sector, 512, &bytesRead, NULL)) {
        std::cout << "ReadFile: " << GetLastError() << std::endl;
        return false;
    }
    else {
        std::cout << "Success!" << std::endl;
    }
    return true;
}

int main(int argc, char ** argv) {
    BYTE sector[512];
    ReadSector(L"\\\\.\\D:", 0, sector);
    system("pause");
    return 0;
}