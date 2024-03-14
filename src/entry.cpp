#include "entry.hpp"

///////////////////////////////////////////////////////// Fat32 Entry /////////////////////////////////////////////////////////

void Entry::setName(std::wstring name) {
    this->name = name;
}

void Entry::setExt(std::wstring ext) {
    this->ext = ext;
}

void Entry::setAttr(std::wstring attr) {
    this->attr = attr;
}

void Entry::setTime(std::wstring time) {
    this->time = time;
}

void Entry::setDate(std::wstring date) {
    this->date = date;
}

void Entry::setCluster(uint64_t clus) {
    this->cluster = clus;
}

void Entry::setSize(uint64_t size) {
    this->size = size;
}

void Entry::setByte(BYTE sector[], uint64_t pos) {
    for (uint64_t i = 0; i < 32; i++) inByte[i] = sector[pos + i];
}

std::wstring Entry::getAttr() {
    return this->attr;
}

uint64_t Entry::getStartCluster() {
    return this->cluster;
}

uint64_t Entry::getSize() {
    return this->size;
}

std::wstring Entry::getExt() {
    return this->ext;
}

void Entry::FormatName() {
    int len = this->name.length(), index = 0;
    wchar_t* temp = new wchar_t [len + 1];
    for (wchar_t c : this->name) temp[index++] = c;
    index--;
    while (index > -1 && this->name[index] == L' ') index--;
    temp[index + 1] = L'\0';
    this->name = temp;
    delete []temp;
}

std::wostream& operator << (std::wostream& out, const Entry& cur) {
    out << L"Name: " << cur.name << std::endl;
    if (cur.ext != L"") out << L"Extension: " << cur.ext << std::endl;
    out << L"Type: " << cur.attr << std::endl;
    out << L"Last modified time: " << cur.date << ", " << cur.time << std::endl;
    out << L"Starting cluster: " << cur.cluster << std::endl;
    out << L"Size: " << cur.size << std::endl;
    return out;
}

///////////////////////////////////////////////////////// MFT Entry /////////////////////////////////////////////////////////

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

std::wostream& operator << (std::wostream& out, const MFTEntry& cur) {
    out << L"Name: " << cur.name << std::endl;
    if (cur.ext != L"") out << L"Extension: " << cur.ext << std::endl;
    out << L"Type: " << cur.type << std::endl;
    out << L"Last modified time: " << cur.date << ", " << cur.time << std::endl;
    out << L"MFT entry index: " << std::dec << cur.mftEntry << std::endl;
    out << L"Size: " << cur.size << std::endl;
    return out;
}