# Reading-FS-Project
OS reading FAT32 and NTFS file system project

## How to run
```bash
1. g++ main.cpp -o main
2. runas /user:Administrator main.exe
    (or main.exe -> Properties -> Compatibility -> Run this program as an administrator
    ./main.exe)
    (or open Visual Studio Code as an administrator and ./main.exe)
```

## How to run with a specific drive
```bash
1. g++ main.cpp -o main
2. runas /user:Administrator main.exe <drive_letter>
    (or main.exe -> Properties -> Compatibility -> Run this program as an administrator
    ./main.exe <drive_letter>)
    (or open Visual Studio Code as an administrator and ./main.exe <drive_letter>)
```

# Assignment
## Week 1 (02/02/2024 - 15/02/2024):
### Đào Việt Hoàng:
- [ ] Read boot sector partition
### Đặng Thanh Tú:
- [ ] Read FAT table
### Đào Ngọc Thiện:
- [ ] Read RDET + SDET
### Bùi Tá Phát:
- [ ] Read file content