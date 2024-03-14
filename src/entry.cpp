#include "entry.hpp"

///////////////////////////////////////////////////////// Fat32 FATEntry /////////////////////////////////////////////////////////

void FATEntry::setName(std::wstring name) {
    this->name = name;
}

void FATEntry::setExt(std::wstring ext) {
    this->ext = ext;
}

void FATEntry::setAttr(std::wstring attr) {
    this->attr = attr;
}

void FATEntry::setTime(std::wstring time) {
    this->time = time;
}

void FATEntry::setDate(std::wstring date) {
    this->date = date;
}

void FATEntry::setCluster(uint64_t clus) {
    this->cluster = clus;
}

void FATEntry::setSize(uint64_t size) {
    this->size = size;
}

void FATEntry::setByte(BYTE sector[], uint64_t pos) {
    for (uint64_t i = 0; i < 32; i++) inByte[i] = sector[pos + i];
}

std::wstring FATEntry::getAttr() {
    return this->attr;
}

uint64_t FATEntry::getStartCluster() {
    return this->cluster;
}

uint64_t FATEntry::getSize() {
    return this->size;
}

std::wstring FATEntry::getExt() {
    return this->ext;
}

void FATEntry::FormatName() {
    // Remove trailing spaces
    int len = this->name.length(), index = 0;
    wchar_t* temp = new wchar_t [len + 1];
    for (wchar_t c : this->name) temp[index++] = c;
    index--;
    while (index > -1 && this->name[index] == L' ') index--;
    temp[index + 1] = L'\0';
    this->name = temp;
    delete []temp;
}

std::wostream& operator<<(std::wostream& out, const FATEntry& fatEntry) {
    out << L"Name: " << fatEntry.name << std::endl;
    if (fatEntry.ext != L"") out << L"Extension: " << fatEntry.ext << std::endl;
    out << L"Type: " << fatEntry.attr << std::endl;
    out << L"Last modified time: " << fatEntry.date << ", " << fatEntry.time << std::endl;
    out << L"Starting cluster: " << fatEntry.cluster << std::endl;
    out << L"Size: " << fatEntry.size << std::endl;
    return out;
}

///////////////////////////////////////////////////////// MFT FATEntry /////////////////////////////////////////////////////////

void MFTEntry::setName(std::wstring name) {
    this->name = name;
}

void MFTEntry::setExt(std::wstring ext) {
    this->ext = ext;
}

void MFTEntry::setType(std::wstring type) {
    this->type = type;
}

void MFTEntry::setTime(std::wstring time) {
    this->time = time;
}

void MFTEntry::setDate(std::wstring date) {
    this->date = date;
}

void MFTEntry::setEntry(uint64_t entry) {
    this->mftEntry = entry;
}

void MFTEntry::setSize(uint64_t size) {
    this->size = size;
}

std::wstring MFTEntry::getType() {
    return this->type;
}

uint64_t MFTEntry::getEntry() {
    return this->mftEntry;
}

std::wstring MFTEntry::getExt() {
    return this->ext;
}

std::wostream& operator<<(std::wostream& out, const MFTEntry& mftEntry) {
    out << L"Name: " << mftEntry.name << std::endl;
    if (mftEntry.ext != L"") out << L"Extension: " << mftEntry.ext << std::endl;
    out << L"Type: " << mftEntry.type << std::endl;
    out << L"Last modified time: " << mftEntry.date << ", " << mftEntry.time << std::endl;
    out << L"MFT entry index: " << std::dec << mftEntry.mftEntry << std::endl;
    out << L"Size: " << mftEntry.size << std::endl;
    return out;
}