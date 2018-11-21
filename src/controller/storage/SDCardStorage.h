//
// Created by Florian Bruggisser on 19.11.18.
//

#ifndef THREE_SCAN_STORAGE_H
#define THREE_SCAN_STORAGE_H


#include <cstdint>
#include <FS.h>
#include <SD.h>

class SDCardStorage {
private:
    uint8_t misoPin;
    uint8_t mosiPin;
    uint8_t sckPin;
    uint8_t csPin;

    boolean connected = false;


public:
    explicit SDCardStorage(uint8_t csPin, uint8_t misoPin = 19, uint8_t mosiPin = 23, uint8_t sckPin = 18);

    void setup();

    void mount();

    void unmount();

    void printSDInfo();

    void writeString(const String &path, const String &content, bool overwrite = false);

    boolean isConnected() const;
};


#endif //THREE_SCAN_STORAGE_H
