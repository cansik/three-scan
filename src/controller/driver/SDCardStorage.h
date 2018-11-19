//
// Created by Florian Bruggisser on 19.11.18.
//

#ifndef THREE_SCAN_STORAGE_H
#define THREE_SCAN_STORAGE_H


#include <cstdint>
#include <FS.h>

class SDCardStorage {
private:
    uint8_t misoPin;
    uint8_t mosiPin;
    uint8_t sckPin;
    uint8_t csPin;

public:
    explicit SDCardStorage(uint8_t csPin, uint8_t misoPin = 23, uint8_t mosiPin = 19, uint8_t sckPin = 18);

    void setup();

    void printSDInfo();

    void listDir(fs::FS &fs, const char *dirname, uint8_t levels);

    void createDir(fs::FS &fs, const char *path);

    void removeDir(fs::FS &fs, const char *path);

    void readFile(fs::FS &fs, const char *path);

    void writeFile(fs::FS &fs, const char *path, const char *message);

    void appendFile(fs::FS &fs, const char *path, const char *message);

    void renameFile(fs::FS &fs, const char *path1, const char *path2);

    void deleteFile(fs::FS &fs, const char *path);

    void testFileIO(fs::FS &fs, const char *path);
};


#endif //THREE_SCAN_STORAGE_H
