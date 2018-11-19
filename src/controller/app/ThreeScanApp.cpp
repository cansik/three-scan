//
// Created by Florian on 30.11.17.
//

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
    sweep->open();
    //sweep->setMotorSpeed(MOTOR_SPEED_CODE_0_HZ);

    // setup sd card
    Serial.println("setting up sd card...");
    storage->setup();
    storage->printSDInfo();
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

    Serial.println("resetting sweep...");
    sweep->reset();

    Serial.println("setting sweep settings...");
    Serial.printf("current motor speed: %d\n", sweep->getMotorSpeed());
    sweep->setMotorSpeed(MOTOR_SPEED_CODE_1_HZ);
    sweep->setSampleRate(SAMPLE_RATE_CODE_1000_HZ);

    Serial.println("waiting for sweep to be ready...");
    sweep->waitUntilMotorReady();
    sweep->startScanning();

    Serial.println("start scanning");
}

void ThreeScanApp::endScan() {
    sweep->stopScanning();
    servo->reset();
    sweep->setMotorSpeed(MOTOR_SPEED_CODE_0_HZ);
    Serial.println("finished scanning!");

    scanning = false;
}

void ThreeScanApp::runScan() {
    // move
    servo->movePrecise(currentAngle);

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
        pointCounter++;
    }

    // check end condition
    if (currentAngle >= scanSettings.endAngle) {
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
