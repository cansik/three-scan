//
// Created by Florian Bruggisser on 19.11.18.
//

#include <SPI.h>
#include <SD.h>
#include <util/StatusLed.h>
#include "SDCardStorage.h"

SDCardStorage::SDCardStorage(uint8_t controlPowerPin, uint8_t csPin, uint8_t misoPin, uint8_t mosiPin, uint8_t sckPin) {
    this->controlPowerPin = controlPowerPin;
    this->csPin = csPin;
    this->misoPin = misoPin;
    this->mosiPin = mosiPin;
    this->sckPin = sckPin;
}


void SDCardStorage::setup() {
    pinMode(sckPin, INPUT_PULLUP);
    pinMode(misoPin, INPUT_PULLUP);
    pinMode(mosiPin, INPUT_PULLUP);
    pinMode(csPin, INPUT_PULLUP);
}

void SDCardStorage::printSDInfo() {
    Serial.printf("SCK: %d, MISO: %d, MOSI: %d, CS: %d\n", sckPin, misoPin, mosiPin, csPin);

    uint8_t cardType = SD.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

boolean SDCardStorage::isConnected() const {
    return connected;
}

void SDCardStorage::writeString(const String &path, const String &content, bool overwrite) {
    // delete file if necessary
    if (SD.exists(path)) {
        if (overwrite) {
            Serial.println("file " + path + " deleted");
            SD.remove(path);
        } else {
            Serial.printf("file %s already exists!", path.c_str());
            return;
        }
    }

    File file = SD.open(path, FILE_WRITE);
    file.print(content);
    file.flush();
    file.close();

    Serial.printf("Write Error: %d\n", file.getWriteError());
}

void SDCardStorage::mount() {
    // turn on sd card

    SPI.begin(sckPin, misoPin, mosiPin, csPin);
    if (!SD.begin(csPin)) {
        Serial.println("Card Mount Failed");
        connected = false;
        return;
    }

    connected = true;
    StatusLed::turnOn();
}

void SDCardStorage::unmount() {
    SD.end();
    connected = false;
    StatusLed::turnOff();
}

String SDCardStorage::getFreeFilePath(String prefix, String extension) {
    String fileName = prefix + extension;

    if (!SD.exists(fileName)) {
        Serial.println("file does not exist: " + fileName);
        return fileName;
    }

    // find new filename
    int i = 1;
    do {
        Serial.println("file already exists: " + fileName);
        fileName = prefix + String(i) + extension;
        i++;
    } while (SD.exists(fileName));

    Serial.println("final file name: " + fileName);
    return fileName;
}

void SDCardStorage::secureMount(unsigned int delayTime) {
    if (isConnected()) {
        unmount();
    }

    delay(delayTime);

    setup();
    mount();

    int mountTries = 1;
    while (!isConnected()) {
        Serial.printf("try to mount (%d)...\n", mountTries);
        setup();
        StatusLed::turnOn();
        delay(250);
        mount();
        StatusLed::turnOff();
        delay(250);
        mountTries++;
    }
}

void SDCardStorage::turnPowerOff() {
    digitalWrite(controlPowerPin, LOW);
}

void SDCardStorage::turnPowerOn() {
    digitalWrite(controlPowerPin, HIGH);
    delay(50);
}
