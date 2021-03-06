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
    uint8_t controlPowerPin;

    boolean connected = false;

    void turnPowerOn();

    void turnPowerOff();


public:
    explicit SDCardStorage(uint8_t controlPowerPin, uint8_t csPin, uint8_t misoPin = 19, uint8_t mosiPin = 23,
                           uint8_t sckPin = 18);

    void setup();

    void secureMount(unsigned int delayTime = 250);

    void mount();

    void unmount();

    String getFreeFilePath(String prefix, String extension);

    void printSDInfo();

    void writeString(const String &path, const String &content, bool overwrite = false);

    boolean isConnected() const;
};


#endif //THREE_SCAN_STORAGE_H
