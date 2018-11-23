//
// Created by Florian on 30.11.17.
//

#include <SD.h>
#include <util/StatusLed.h>
#include "ThreeScanApp.h"
#include "util/MathUtils.h"

ThreeScanApp::ThreeScanApp(StoragePtr storage, SweepESP32Ptr sweep, PreciseServoPtr servo) {
    this->settings = AppSettings();
    this->storage = storage;
    this->sweep = sweep;
    this->servo = servo;
}

void ThreeScanApp::setup() {
    BaseController::setup();

    scanSettings = ScanSettings();

    // setup servo
    Serial.println("setting up servo...");
    servo->setup();

    servo->movePulse(1500);
    delay(500);
    servo->reset();

    // setup sweep
    Serial.println("setting up sweep...");
    sweep->setup();

    // setup sd card
    Serial.println("setting up sd card...");
    storage->setup();
}

void ThreeScanApp::loop() {
    BaseController::loop();

    if (scanning)
        runScan();
}

void ThreeScanApp::startScan() {
    scanning = true;
    waitForSync = true;
    currentAngle = scanSettings.startAngle;
    currentAngleChanged = true;

    // setup sd card
    Serial.println("mounting sd card...");
    storage->mount();

    if (storage->isConnected())
        StatusLed::turnOn();

    storage->printSDInfo();

    Serial.println("starting sweep...");
    sweep->open();

    Serial.println("setting sweep settings...");
    sweep->setMotorSpeed(MOTOR_SPEED_CODE_5_HZ);
    sweep->setSampleRate(SAMPLE_RATE_CODE_500_HZ);

    Serial.println("waiting for sweep to be ready...");
    sweep->waitUntilMotorReady();

    Serial.printf("current motor speed: %d\n", sweep->getMotorSpeed());
    Serial.printf("current sample rate: %d\n", sweep->getSampleRate());

    sweep->startScanning();

    Serial.println("start scanning");
}

void ThreeScanApp::endScan() {
    sweep->stopScanning();
    servo->reset();
    sweep->close();
    Serial.println("finished scanning!");

    // store data
    if (storage->isConnected()) {
        Serial.print("writing data...");
        storage->writeString("/juan2.txt", data, true);
        Serial.println("done!");

        storage->unmount();
        StatusLed::turnOff();
    }

    scanning = false;
}

void ThreeScanApp::runScan() {
    // move
    if (currentAngleChanged) {
        servo->movePrecise(currentAngle, true);
        currentAngleChanged = false;
    }

    // check if scan is ready
    auto success = false;
    ScanPacket reading = sweep->getReading(success);

    if (success) {
        if (reading.isSync()) {
            Serial.println("sync");
            pointCounter = 0;

            if (waitForSync) {
                waitForSync = false;

                // update x angle
                currentAngle += scanSettings.angleStep;
                currentAngleChanged = true;
            } else {
                waitForSync = true;
                sweep->stopScanning();
                sweep->startScanning();
            }
        }

        if (waitForSync)
            return;

        Serial.printf("%d. (%2f°):\t%2f°\t%d cm\n", pointCounter, currentAngle, reading.getAngleDegrees(),
                      reading.getDistanceCentimeters());

        data += String(currentAngle) + ";" + String(reading.getAngleDegrees()) + ";" +
                String(reading.getDistanceCentimeters()) + "\n";

        pointCounter++;
    }

    // check end condition
    if (currentAngle > scanSettings.endAngle) {
        endScan();
    }

}

void ThreeScanApp::loadFromEEPROM() {
    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("failed to initialise EEPROM");
        return;
    }

    // set start address
    int address = EEPROM_START_ADDRESS;
    EEPROM.get(address, settings);

    Serial.printf("Loaded Version: %d\n", settings.getVersion());

    // check version and set default if needed
    if (settings.getVersion() != TIL_SETTINGS_VERSION) {
        Serial.println("applying default app settings!");
        loadDefaultSettings();
    }

    EEPROM.end();
}

void ThreeScanApp::saveToEEPROM() {
    if (!EEPROM.begin(EEPROM_SIZE)) {
        Serial.println("failed to initialise EEPROM");
        return;
    }

    // set start address
    int address = EEPROM_START_ADDRESS;
    EEPROM.put(address, settings);

    EEPROM.commit();
    EEPROM.end();
}

AppSettings &ThreeScanApp::getSettings() {
    return settings;
}

void ThreeScanApp::loadDefaultSettings() {
    settings = AppSettings();
}
