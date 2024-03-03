#include "entry.hpp"

///////////////////////////////////////////////////////// Fat32 Entry /////////////////////////////////////////////////////////

void Entry::setName(std::string name) {
    this->name = name;
}

void Entry::setExt(std::string ext) {
    this->ext = ext;
}

void Entry::setAttr(std::string attr) {
    this->attr = attr;
}

void Entry::setTime(std::string time) {
    this->time = time;
}

void Entry::setDate(std::string date) {
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

std::string Entry::getAttr() {
    return this->attr;
}

uint64_t Entry::getStartCluster() {
    return this->cluster;
}

uint64_t Entry::getSize() {
    return this->size;
}

void Entry::FormatName() {
    uint64_t len = this->name.length(), index = 0;
    char* temp = new char [len + 1];
    for (char c : this->name) temp[index++] = c;
    index--;
    while (index > -1 && this->name[index] == ' ') index--;
    temp[index + 1] = '\0';
    this->name = temp;
    delete []temp;
}

std::ostream& operator << (std::ostream& out, const Entry& cur) {
    out << "Name: " << cur.name;
    if (cur.ext != "") out << "." << cur.ext;
    out << std::endl;
    out << "Attribute: " << cur.attr << std::endl;
    out << "Last modified time: " << cur.date << ", " << cur.time << std::endl;
    out << "Starting cluster: " << cur.cluster << std::endl;
    out << "Size: " << cur.size << std::endl;
    return out;
}

///////////////////////////////////////////////////////// MFT Entry /////////////////////////////////////////////////////////

void MFTEntry::setName(std::string name) {
    this->name = name;
}

void MFTEntry::setExt(std::string ext) {
    this->ext = ext;
}

void MFTEntry::setAttr(std::string attr) {
    this->attr = attr;
}

void MFTEntry::setTime(std::string time) {
    this->time = time;
}

void MFTEntry::setDate(std::string date) {
    this->date = date;
}

void MFTEntry::setCluster(int clus) {
    this->cluster = clus;
}

void MFTEntry::setSize(int size) {
    this->size = size;
}

void MFTEntry::FormatName() {
    int len = this->name.length(), index = 0;
    char* temp = new char [len + 1];
    for (char c : this->name) temp[index++] = c;
    index--;
    while (index > -1 && this->name[index] == ' ') index--;
    temp[index + 1] = '\0';
    this->name = temp;
    delete []temp;
}

std::ostream& operator << (std::ostream& out, const MFTEntry& cur) {
    out << "Name: " << cur.name;
    if (cur.ext != "") out << "." << cur.ext;
    out << std::endl;
    out << "Attribute: " << cur.attr << std::endl;
    out << "Last modified time: " << cur.date << ", " << cur.time << std::endl;
    out << "Starting cluster: " << cur.cluster << std::endl;
    out << "Size: " << cur.size << std::endl;
    return out;
}