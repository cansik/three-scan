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
    currentAngle = settings.getStartAngle();
    currentAngleChanged = true;
    isFirstAngle = true;
    fullPointCounter = 0;
    maxPointCount = 0;
    currentSliceIteration = 0;
    filteredPoints = 0;

    filterStartAngle = 180.0f - (settings.getStandFilterSize() / 2.0f);
    filterEndAngle = 180.0f + (settings.getStandFilterSize() / 2.0f);

    watch.start();

    // create name and output file
    if (!settings.isSerialTransfer()) {
        storage->secureMount();
        auto fileName = storage->getFreeFilePath("/scan", ".ply");
        storage->unmount();
        cloudFile->create(fileName);
    }

    // clear buffer
    Serial.println("clearing buffer...");
    buffer.clear();

    Serial.println("starting sweep...");
    sweep->open();

    Serial.println("setting sweep settings...");
    sweep->setMotorSpeed(settings.getMotorSpeed());
    sweep->setSampleRate(settings.getSampleRate());

    Serial.println("waiting for sweep to be ready...");
    sweep->waitUntilMotorReady();

    Serial.printf("current motor speed: %d\n", sweep->getMotorSpeed());
    Serial.printf("current sample rate: %d\n", sweep->getSampleRate());

    // move servo to start position
    servo->movePrecise(currentAngle, true);
    delay(500);

    Serial.println("start scanning");
    sweep->startScanning();
    syncTimeoutTimer.reset();
}

void ThreeScanApp::endScan() {
    sweep->stopScanning();
    servo->reset();
    sweep->close();
    Serial.println("finished scanning!");

    delay(1000);

    // store data
    if (!settings.isSerialTransfer()) {
        Serial.println("writing data...");
        saveData();
        Serial.println("done!");
    } else {
        Serial.printf("TST:END\n");
    }

    StatusLed::turnOff();

    watch.stop();
    Serial.printf("Scan Took: %lu ms", watch.elapsed());

    scanning = false;
}

void ThreeScanApp::runScan() {
    // move
    if (currentAngleChanged) {
        servo->movePrecise(currentAngle, true);
        currentAngleChanged = false;
    }

    // check sync timeout
    if (syncTimeoutTimer.elapsed()) {
        Serial.println("no sync received, try to start sweep again!");
        sweep->stopScanning();
        delay(500);
        sweep->startScanning();
    }

    // check if scan is ready
    auto success = false;
    ScanPacket reading = sweep->getReading(success);

    if (success) {
        if (reading.isSync()) {
            syncTimeoutTimer.reset();

            if (waitForSync) {
                waitForSync = false;

                // update x angle
                if (isFirstAngle) {
                    isFirstAngle = false;
                } else {
                    Serial.printf("sync %2f - points: %d max: %d filtered: %d\n",
                                  currentAngle, fullPointCounter, maxPointCount, filteredPoints);
                    Serial.printf("TST:SYN\n");

                    // update max point
                    maxPointCount = (maxPointCount < pointCounter) ? pointCounter : maxPointCount;

                    // check if buffer is still enough
                    int mpc = static_cast<int>(std::llround(maxPointCount * 1.2));
                    if (buffer.getMaxSize() < buffer.length() + mpc && !settings.isSerialTransfer()) {
                        writing = true;
                        sweep->stopScanning();
                        delay(500);

                        Serial.print("buffer may exceeding...appending...");
                        cloudFile->appendBuffer(buffer);
                        buffer.clear();
                        Serial.println("done!");
                        syncTimeoutTimer.reset();

                        // reset scanning
                        writing = false;
                        sweep->startScanning();
                    }

                    currentSliceIteration++;
                    if (settings.getSliceIterationCount() <= currentSliceIteration) {
                        currentAngle += settings.getAngleStep();
                        currentAngleChanged = true;
                        currentSliceIteration = 0;
                    }
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

        // filter by signal strength and stand angle
        auto signalFilter = reading.getSignalStrength() < settings.getMinSignalStrength();
        auto angle = abs(reading.getAngleDegrees());
        auto standFilter = angle >= filterStartAngle && angle <= filterEndAngle;

        if (signalFilter || standFilter) {
            filteredPoints++;
            return;
        }

        // new package received
        if (settings.isSerialTransfer()) {
            auto v = Vertex(reading.getAngleDegrees(), currentAngle, reading.getDistanceCentimeters(),
                            reading.getSignalStrength());
            auto spherical = v.getSphericalPosition();

            Serial.printf("TST:DAT:%6f:%6f:%6f:%d\n", spherical.x, spherical.y, spherical.z, v.getSignalStrength());
        } else {
            // add data to buffer
            buffer.add(new Vertex(reading.getAngleDegrees(), currentAngle, reading.getDistanceCentimeters(),
                                  reading.getSignalStrength()));
        }

        pointCounter++;
        fullPointCounter++;
    }

    // check end condition
    if (currentAngle > settings.getEndAngle()) {
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
    if (settings.getVersion() != THREE_SCAN_SETTINGS_VERSION) {
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
    writing = true;

    Serial.println("appending buffer...");
    cloudFile->appendBuffer(buffer);

    Serial.printf("clearing buffer (%d)\n", buffer.length());
    buffer.clear();

    Serial.println("closing file...");
    cloudFile->close();
    writing = false;
}

float ThreeScanApp::getScanProgress() {
    return MathUtils::map(currentAngle, settings.getStartAngle(), settings.getEndAngle(), 0.0f, 1.0f);
}

bool ThreeScanApp::isSDMounted() {
    return storage->isConnected();
}

bool ThreeScanApp::isScanning() const {
    return scanning;
}

volatile float ThreeScanApp::getCurrentAngle() const {
    return currentAngle;
}

volatile int ThreeScanApp::getFullPointCounter() const {
    return fullPointCounter;
}

volatile int ThreeScanApp::getFilteredPoints() const {
    return filteredPoints;
}

volatile bool ThreeScanApp::isWriting() const {
    return writing;
}

const String &ThreeScanApp::getPath() const {
    return cloudFile->getPath();
}

unsigned long ThreeScanApp::getElapsedTime() {
    return watch.elapsed();
}
