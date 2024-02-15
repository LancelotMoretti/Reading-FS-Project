#include "utilities.hpp"

int main(int argc, char ** argv) {
    BYTE sector[512];
    readSector(L"\\\\.\\D:", 0, sector, 512);
    system("pause");
    return 0;
}