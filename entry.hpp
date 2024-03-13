#ifndef ENTRY_HPP_
#define ENTRY_HPP_

#include "typedef.hpp"

#ifndef FAT32_ENTRY_
#define FAT32_ENTRY_

class Entry {
private:
    // Entry specific
    std::wstring name; // Name
    std::wstring ext; // Extension
    std::wstring attr; // Attribute
    std::wstring time; // Last update time
    std::wstring date; // Last update date
    uint64_t cluster; // Starting cluster
    uint64_t size; // Size

    //
    BYTE inByte[32];
public:
    Entry() = default;

    // Setters
    void setByte(BYTE sector[], uint64_t pos);
    void setName(std::wstring name);
    void setExt(std::wstring ext);
    void setAttr(std::wstring attr);
    void setTime(std::wstring time);
    void setDate(std::wstring date);
    void setCluster(uint64_t clus);
    void setSize(uint64_t size);

    // Getters
    std::wstring getAttr();
    uint64_t getStartCluster();
    uint64_t getSize();
    std::wstring getExt();

    // Methods
    void FormatName(); // Remove trailing spaces

    // Overload
    friend std::wostream& operator << (std::wostream&, const Entry&);

    ~Entry() = default;
};

#endif /* FAT32_ENTRY_ */

#ifndef MFT_ENTRY_
#define MFT_ENTRY_

class MFTEntry {
private:
    std::wstring name; // Name
    std::wstring ext; // Extension
    std::wstring type; // Attribute
    std::wstring time; // Last update time
    std::wstring date; // Last update date
    int mftEntry; // Starting cluster
    int size; // Size
public:
    MFTEntry() = default;

    // Setters
    void setName(std::wstring name);
    void setExt(std::wstring ext);
    void setAttr(std::wstring attr);
    void setTime(std::wstring time);
    void setDate(std::wstring date);
    void setCluster(int clus);
    void setSize(int size);

    // Getters
    std::wstring getAttr();
    uint64_t getEntry();
    std::wstring getExt();

    // Methods
    void FormatName(); // Remove trailing spaces

    friend std::wostream& operator<<(std::wostream&, const MFTEntry&);
};

#endif /* MFT_ENTRY_ */

#endif /* ENTRY_HPP_ */