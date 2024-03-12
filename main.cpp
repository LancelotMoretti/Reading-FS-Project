#include "fat32.hpp"
#include "ntfs.hpp"

int main() {
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    DWORD bytesRead;
    HANDLE device = NULL;
    std::wstring rootDir = L"\\\\.\\";
    std::wstring name;
    Volume *volume = nullptr;
    std::vector<BYTE> sector(512);

    std::wcout << L"----------------------------------------" << std::endl;
    std::wcout << L"-------       WELCOME USER       -------" << std::endl;
    std::wcout << std::endl;

    while (true) {
        std::wcout << L"----------------------------------------" << std::endl;
        std::wcout << L"-- Enter volume's name (enter 0 to exit): ";
        std::wcin >> name;
        std::wstring curDir = rootDir + name + L":";
        
        // Stop while loop 
        if (name == L"0") break;

        // Open partition
        device = CreateFileW(
            curDir.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
        );

        // Partition not exist
        if (device == INVALID_HANDLE_VALUE || !ReadFile(device, sector.data(), 512, &bytesRead, NULL)) {
            std::wcout << "-- This volume is not exist!" << std::endl;
            std::wcout << "-- Please enter a valid volume's name!" << std::endl;
            continue;
        }

        readSector(device, 0, sector.data(), 512);
        if (sector[3] == 'N' && sector[4] == 'T' && sector[5] == 'F' && sector[6] == 'S') {
            std::wcout << "-- Detected NTFS file system" << std::endl;
            volume = new NTFS(sector, device);
        }
        else if (sector[82] == 'F' && sector[83] == 'A' && sector[84] == 'T' && sector[85] == '3' && sector[86] == '2') {
            std::wcout << "-- Detected Fat32 file system" << std::endl;
            volume = new Fat32(sector, device);
        }
        else {
            std::wcout << "-- Unknown file system" << std::endl;
            continue;
        }

        int choice;

        do {
            std::wcout << "-- Success" << std::endl;
            std::wcout << "----------------------------------------" << std::endl;
            std::wcout << "----     WELCOME TO " << name << " VOLUME     ----" << std::endl;
            std::wcout << "-- What do you want to do?" << std::endl;
            std::wcout << "--  1. View volume's information" << std::endl;
            std::wcout << "--  2. View directory list" << std::endl;
            std::wcout << "--  3. Read file or folder at position" << std::endl;
            std::wcout << "--  4. Return previous directory" << std::endl;
            std::wcout << "--  5. Return root directory" << std::endl;
            std::wcout << "--  6. Exit" << std::endl;
            std::wcout << "-- >> Enter your choice: ";
            std::wcin >> choice;

            switch (choice) {
                case 1:
                    volume->ViewVolumeInformation();
                    break;
                case 2:
                    volume->ViewFolderTree();
                    break;
                case 3: {
                    uint64_t position;
                    std::wcout << "-- Enter position: ";
                    std::wcin >> position;
                    volume->ReadFileAtPosition(position);
                    break;
                }
                case 4:
                    volume->ReturnToParent();
                    break;
                case 5:
                    volume->ReturnToRoot();
                    break;
                case 6:
                    break;
                default:
                    std::wcout << "-- Invalid choice!" << std::endl;
                    break;
            }
        } while (choice != 6);

        delete volume;
        volume = nullptr;
        CloseHandle(device);
    }
    return 0;
}