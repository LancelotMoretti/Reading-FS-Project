#include <iostream>
#include <Windows.h>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdint>
#include <sstream>
#include <cmath>

#ifndef ENTRY_HPP_
#define ENTRY_HPP_

#ifndef FAT32_ENTRY_
#define FAT32_ENTRY_

class Entry {
private:
    // Entry specific
    std::string name; // Name
    std::string ext; // Extension
    std::string attr; // Attribute
    std::string time; // Last update time
    std::string date; // Last update date
    uint64_t cluster; // Starting cluster
    uint64_t size; // Size

    //
    BYTE inByte[32];
public:
    Entry() = default;

    // Setters
    void setByte(BYTE sector[], uint64_t pos);
    void setName(std::string name);
    void setExt(std::string ext);
    void setAttr(std::string attr);
    void setTime(std::string time);
    void setDate(std::string date);
    void setCluster(uint64_t clus);
    void setSize(uint64_t size);

    // Getters
    std::string getAttr();
    uint64_t getStartCluster();
    uint64_t getSize();

    // Methods
    void FormatName(); // Remove trailing spaces

    // Overload
    friend std::ostream& operator<<(std::ostream&, const Entry&);

    ~Entry() = default;
};

#endif /* FAT32_ENTRY_ */

#ifndef MFT_ENTRY_
#define MFT_ENTRY_

class MFTEntry {
private:
    std::string name; // Name
    std::string ext; // Extension
    std::string attr; // Attribute
    std::string time; // Last update time
    std::string date; // Last update date
    int cluster; // Starting cluster
    int size; // Size
public:
    MFTEntry() = default;

    // Setters
    void setName(std::string name);
    void setExt(std::string ext);
    void setAttr(std::string attr);
    void setTime(std::string time);
    void setDate(std::string date);
    void setCluster(int clus);
    void setSize(int size);

    // Getters
    std::string getAttr();

    // Methods
    void FormatName(); // Remove trailing spaces

    friend std::ostream& operator<<(std::ostream&, const MFTEntry&);
};

#endif /* MFT_ENTRY_ */

#endif /* ENTRY_HPP_ */