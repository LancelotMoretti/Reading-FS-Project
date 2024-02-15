#ifndef FAT32_HPP_
#define FAT32_HPP_
#include "drive.hpp"

class Fat32 : public Drive {
public:
    Fat32() = default;

private:
    LPCWSTR drive;
    int SectorsPerBootSector;
    int NumOfFAT;
    int SizeOfVolume;
    int SectorsPerFAT;
    int StartOfRDET;
    int SizeOfRDET;

    void ReadAndDisplayFileData(int startCluster, int fileSize);
    int GetNextDataCluster(int currentCluster);
    void ReadDataCluster(int cluster, std::vector<BYTE> buffer);
    void ReadDataSector(int sector, std::vector<BYTE> buffer);

};

class Entry {
private:
    std::string name; // Name
    std::string ext; // Extension
    std::string attr; // Attribute
    std::string time; // Last update time
    std::string date; // Last update date
    int cluster; // Starting cluster
    int size; // Size
    BYTE inByte[32];
public:
    void setByte(BYTE sector[], int pos) {
        for (int i = 0; i < 32; i++) {
            this->inByte[i] = sector[pos + i];
        }
    }
    void formatName() {
        int len = this->name.length(), index = 0;
        char* temp = new char [len + 1];
        for (char c : this->name) temp[index++] = c;
        index--;
        while (index > -1 && this->name[index] == ' ') index--;
        temp[index + 1] = '\0';
        this->name = temp;
        delete []temp;
    }

    void setName(std::string name) {
        this->name = name;
    }

    void setExt(std::string ext) {
        this->ext = ext;
    }

    void setAttr(std::string attr) {
        this->attr = attr;
    }

    void setTime(std::string time) {
        this->time = time;
    }

    void setDate(std::string date) {
        this->date = date;
    }

    void setCluster(int clus) {
        this->cluster = clus;
    }

    void setSize(int size) {
        this->size = size;
    }

    std::string getAttr() {
        return this->attr;
    }
    
    friend std::ostream& operator << (std::ostream&, const Entry&);
};

#endif /* FAT32_HPP_ */