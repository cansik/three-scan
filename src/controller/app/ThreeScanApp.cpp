#include <cmath>

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
    storage->printSDInfo();

    // setting up data file
    cloudFile = new CloudFile(storage);
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
    isFirstAngle = true;
    fullPointCounter = 0;
    maxPointCount = 0;

    // create name and output file
    storage->secureMount();
    auto fileName = storage->getFreeFilePath("/scan", ".ply");
    storage->unmount();
    cloudFile->create(fileName);

    // clear buffer
    Serial.println("clearing buffer...");
    buffer.clear();

    Serial.println("starting sweep...");
    sweep->open();

    Serial.println("setting sweep settings...");
    sweep->setMotorSpeed(MOTOR_SPEED_CODE_5_HZ);
    sweep->setSampleRate(SAMPLE_RATE_CODE_500_HZ);

    Serial.println("waiting for sweep to be ready...");
    sweep->waitUntilMotorReady();

    Serial.printf("current motor speed: %d\n", sweep->getMotorSpeed());
    Serial.printf("current sample rate: %d\n", sweep->getSampleRate());

    // move servo to start position
    servo->movePrecise(currentAngle, true);
    delay(500);

    Serial.println("start scanning");
    sweep->startScanning();
    syncTimoutTimer.reset();
}

void ThreeScanApp::endScan() {
    sweep->stopScanning();
    servo->reset();
    sweep->close();
    Serial.println("finished scanning!");

    delay(1000);

    // store data
    Serial.println("writing data...");
    saveData();
    Serial.println("done!");

    StatusLed::turnOff();

    scanning = false;
}

void ThreeScanApp::runScan() {
    // move
    if (currentAngleChanged) {
        servo->movePrecise(currentAngle, true);
        currentAngleChanged = false;
    }

    // check sync timeout
    if (syncTimoutTimer.elapsed()) {
        Serial.println("no sync received, try to start sweep again!");
        sweep->stopScanning();
        delay(1000);
        sweep->startScanning();
    }

    // check if scan is ready
    auto success = false;
    ScanPacket reading = sweep->getReading(success);

    if (success) {
        if (reading.isSync()) {
            syncTimoutTimer.reset();

            if (waitForSync) {
                waitForSync = false;

                // update x angle
                if (isFirstAngle) {
                    isFirstAngle = false;
                } else {
                    Serial.printf("sync %2f - points: %d max: %d\n", currentAngle, fullPointCounter,
                                  maxPointCount);

                    // update max point
                    maxPointCount = (maxPointCount < pointCounter) ? pointCounter : maxPointCount;

                    // check if buffer is still enough
                    int mpc = static_cast<int>(std::llround(maxPointCount * 1.2));
                    if (buffer.getMaxSize() < buffer.length() + mpc) {
                        sweep->stopScanning();
                        delay(500);

                        Serial.println("buffer may exceeding...");
                        Serial.println("appending buffer...");
                        cloudFile->appendBuffer(buffer);
                        buffer.clear();
                        Serial.println("done!");
                        syncTimoutTimer.reset();

                        // reset scanning
                        sweep->startScanning();
                    }

                    currentAngle += scanSettings.angleStep;
                    currentAngleChanged = true;
                }

                pointCounter = 0;
            } else {
                waitForSync = true;
                sweep->stopScanning();
                sweep->startScanning();
            }
        }

        if (waitForSync)
            return;

        // new package received
        //Serial.printf("%d. (%2f°):\t%2f°\t%d cm\n", pointCounter, currentAngle, reading.getAngleDegrees(),
        //reading.getDistanceCentimeters());

        // add data to buffer
        buffer.add(new Vertex(reading.getAngleDegrees(), currentAngle, reading.getDistanceCentimeters(),
                              reading.getSignalStrength()));
        pointCounter++;
        fullPointCounter++;
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
    delay(1000);

    Serial.println("appending buffer...");
    cloudFile->appendBuffer(buffer);

    Serial.printf("clearing buffer (%d)\n", buffer.length());
    buffer.clear();

    Serial.println("closing file...");
    cloudFile->close();
}
