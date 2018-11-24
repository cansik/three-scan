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

    // mount sd card
    Serial.println("mounting sd card...");
    storage->mount();

    if (storage->isConnected())
        StatusLed::turnOn();

    storage->printSDInfo();

    //storage->writeString("/juan", "hello world", true);

    // setting up data file
    plyFile = new PLYFile(storage);
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

    Serial.println("starting sweep...");
    sweep->open();

    Serial.println("setting sweep settings...");
    sweep->setMotorSpeed(MOTOR_SPEED_CODE_5_HZ);
    sweep->setSampleRate(SAMPLE_RATE_CODE_500_HZ);

    // clear buffer
    buffer.clear();

    Serial.println("waiting for sweep to be ready...");
    sweep->waitUntilMotorReady();

    Serial.printf("current motor speed: %d\n", sweep->getMotorSpeed());
    Serial.printf("current sample rate: %d\n", sweep->getSampleRate());

    Serial.println("start scanning");
    sweep->startScanning();
}

void ThreeScanApp::endScan() {
    sweep->stopScanning();
    servo->reset();
    sweep->close();
    Serial.println("finished scanning!");

    // store data
    if (storage->isConnected()) {
        Serial.println("writing data...");
        saveData();
        Serial.println("done!");

        storage->unmount();
    }

    StatusLed::turnOff();
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
            Serial.printf("sync %2f", currentAngle);
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

        // new package received
        Serial.printf("%d. (%2f°):\t%2f°\t%d cm\n", pointCounter, currentAngle, reading.getAngleDegrees(),
                      reading.getDistanceCentimeters());

        // add data to buffer
        buffer.add(new Vertex(currentAngle, reading.getAngleDegrees(), reading.getDistanceCentimeters(),
                              reading.getSignalStrength()));
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

void ThreeScanApp::saveData() {
    // reset card storage
    if (storage->isConnected())
        storage->unmount();
    StatusLed::turnOff();
    delay(2000);

    storage->setup();
    storage->mount();

    if (storage->isConnected()) {
        StatusLed::turnOn();
    }

    // create file
    plyFile->create(storage->getFreeFilePath("/scan", ".ply"), buffer.length());

    Serial.println("appending buffer...");
    for (unsigned int i = 0; i < buffer.length(); i++) {
        plyFile->append(buffer.get(i));
    }

    Serial.println("writing file...");
    plyFile->close();
}
