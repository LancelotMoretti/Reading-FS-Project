#ifndef DRIVE_HPP_
#define DRIVE_HPP_

class Drive {
public:
    Drive() = default;

protected:
    int BytesPerSector;
    int SectorsPerCluster;

};

#endif /* DRIVE_HPP_ */