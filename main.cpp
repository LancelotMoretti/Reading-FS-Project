#include "fat32.hpp"
#include "ntfs.hpp"

int main(int argc, char ** argv) {
    // Drive* Drive = nullptr;
    // BYTE buffer[512];
    // LPCWSTR drive;

    // if (argc != 2) {
    //     std::cout << "Usage: " << argv[0] << " <drive letter>" << std::endl;
    //     std::cout << "Example: " << argv[0] << " C" << std::endl;
    //     std::cout << "Set to default drive C:" << std::endl;
    //     drive = L"\\\\.\\C:";
    // }
    // else {
    //     wchar_t tmp[100];
    //     wcscat(tmp, L"\\\\.\\");
    //     wcscpy(tmp, argv[1]);
    //     wcscat(tmp, L":");
    //     drive = tmp;
    // }

    // readSector(drive, 0, buffer, 512);
    // if (buffer[3] == 'N' && buffer[4] == 'T' && buffer[5] == 'F' && buffer[6] == 'S') {
    //     Drive = new NTFS();
    // }
    // else if (buffer[82] == 'F' && buffer[83] == 'A' && buffer[84] == 'T' && buffer[85] == '3' && buffer[86] == '2') {
    //     Drive = new Fat32();
    // }
    // else {
    //     std::cout << "Unknown file system" << std::endl;
    //     return 1;
    // }

    // while (true) {
    //     std::string command;
    //     std::cout << "help - display all command" << std::endl;
    //     std::cout << "Enter command: ";
    //     std::cin >> command;
    //     if (command == "exit") {
    //         break;
    //     }
    //     else if (command == "help") {
    //         std::cout << "exit - exit the program" << std::endl;
    //     }
    //     else {
    //         std::cout << "Unknown command" << std::endl;
    //     }
    // }

    BYTE sector[512];
    readSector(L"\\\\.\\F:", 0, sector, 512);

    uint64_t start = rdetStartPoint(sector) * 512;

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
    res2 = readRDETSDET(L"\\\\.\\F:", start, false);
    // for (int i = 0; i < res2.size(); i++) std::cout << res2[i] << std::endl;
    printFileAndFolder(res2);

    return 0;
}