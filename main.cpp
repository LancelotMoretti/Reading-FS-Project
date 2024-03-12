#include "fat32.hpp"
#include "ntfs.hpp"

int main(int argc, char ** argv) {
    // Volume* Volume = nullptr;
    // std::vector<BYTE> buffer(512);
    // LPCWSTR volume;

    // if (argc != 2) {
    //     std::wcout << "Usage: " << argv[0] << " <volume letter>" << std::endl;
    //     std::wcout << "Example: " << argv[0] << " C" << std::endl;
    //     std::wcout << "Set to default volume C:" << std::endl;
    //     volume = L"\\\\.\\C:";
    // }
    // else {
    //     wchar_t tmp[100];
    //     wcscat(tmp, L"\\\\.\\");
    //     wcscpy(tmp, (wchar_t*)argv[1]);
    //     wcscat(tmp, L":");
    //     volume = tmp;
    // }

    // readSector(volume, 0, buffer.data(), 512);
    // if (buffer[3] == 'N' && buffer[4] == 'T' && buffer[5] == 'F' && buffer[6] == 'S') {
    //     Volume = new NTFS(buffer, volume);
    // }
    // else if (buffer[82] == 'F' && buffer[83] == 'A' && buffer[84] == 'T' && buffer[85] == '3' && buffer[86] == '2') {
    //     Volume = new Fat32(buffer, volume);
    // }
    // else {
    //     std::wcout << "Unknown file system" << std::endl;
    //     return 1;
    // }
    // while (true) {
    //     std::string command;
    //     std::wcout << "-help - display all command" << std::endl;
    //     std::wcout << "Enter command: ";
    //     std::cin >> command;
    //     if (command == "-exit") {
    //         break;
    //     }
    //     else if (command == "-help") {
    //         std::wcout << "-exit - exit the program" << std::endl;
    //         std::wcout << "-type - display file system type" << std::endl;
    //         std::wcout << "-read - read file at position" << std::endl;
    //         std::wcout << "-start - return to start" << std::endl;
    //         std::wcout << "-parent - return to parent" << std::endl;
    //     }
    //     else if (command == "-start") {
    //         Volume->ReturnToRoot();
    //     }
    //     else if (command == "-parent") {
    //         Volume->ReturnToParent();
    //     }
    //     else if (command == "-read") {
    //         uint64_t position;
    //         std::wcout << "Enter position: ";
    //         std::cin >> position;
    //         Volume->ReadFileAtPosition(position);
    //     }
    //     else if (command == "-type") {
    //         std::wcout << Volume->GetFileSystemType() << std::endl;
    //     }
    //     else {
    //         std::wcout << "Unknown command" << std::endl;
    //     }
    // }

    // delete Volume;
    // return 0;

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

        // uint64_t start = MFTStartPoint(sector.data()) * 512;

        int choice;

        do {
            std::wcout << "-- Success" << std::endl;
            std::wcout << "----------------------------------------" << std::endl;
            std::wcout << "----     WELCOME TO " << name << " VOLUME     ----" << std::endl;
            std::wcout << "-- What do you want to do?" << std::endl;
            std::wcout << "--  1. View volume's information" << std::endl;
            std::wcout << "--  2. View volume's data" << std::endl;
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

    // BYTE sector[512];
    // readSector(L"\\\\.\\F:", 0, sector, 512);

    // uint64_t start = rdetStartPoint(sector) * 512;

    // // std::vector<Entry> res;
    // // res = readRDETSDET(L"\\\\.\\B:", start, true);
    // // for (int i = 0; i < res.size(); i++) std::wcout << res[i] << std::endl;

    // // uint64_t start2 = sdetStartPoint(sector, 10) * 512;
    // uint64_t start2 = sdetStartPoint(sector, 22) * 512;
    // // uint64_t start2 = sdetStartPoint(sector, 157735) * 512;
    // // std::wcout << sdetStartPoint(sector, 26665) << std::endl;
    // // uint64_t start2 = sdetStartPoint(sector, 28871) * 512;
    // // uint64_t start2 = sdetStartPoint(sector, 159941) * 512;
    // std::vector<Entry> res2;
    // res2 = readRDETSDET(L"\\\\.\\F:", start, false);
    // // for (int i = 0; i < res2.size(); i++) std::wcout << res2[i] << std::endl;
    // printFileAndFolder(res2);

    // return 0;
}