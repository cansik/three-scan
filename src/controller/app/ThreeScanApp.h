#ifndef TIL_INSTALLATION_H
#define TIL_INSTALLATION_H

#include <EEPROM.h>
#include <assert.h>
#include <controller/storage/SDCardStorage.h>
#include <driver/sweep/SweepESP32.h>
#include <driver/motor/PreciseServo.h>
#include <util/Timer.h>
#include <model/ScanSettings.h>
#include <model/Vertex.h>
#include <util/MutableBuffer.h>
#include <controller/storage/PLYFile.h>
#include <controller/storage/CloudFile.h>
#include <util/ExponentialMovingAverage.h>
#include "EEPROM.h"
#include "model/AppSettings.h"
#include "controller/BaseController.h"

#define EEPROM_SIZE 0xFF
#define EEPROM_START_ADDRESS 0x00

#define MAX_BUFFER_SIZE 1000 // 2^15

class ThreeScanApp : public BaseController {
private:
    typedef SDCardStorage *StoragePtr;
    typedef SweepESP32 *SweepESP32Ptr;
    typedef PreciseServo *PreciseServoPtr;
    typedef CloudFile *CloudFilePtr;
    typedef ExponentialMovingAverage *ExponentialMovingAveragePtr;

    AppSettings settings;
    ScanSettings scanSettings;

    StoragePtr storage;
    SweepESP32Ptr sweep;
    PreciseServoPtr servo;
    CloudFilePtr cloudFile;

    Timer scanTimer = Timer(33);

    bool scanning = false;
    float currentAngle = 0.0f;
    bool currentAngleChanged = false;
    bool waitForSync = false;
    bool isFirstAngle = true;

    int pointCounter = 0;
    int fullPointCounter = 0;

    int maxPointCount = 0;

    MutableBuffer<Vertex> buffer = MutableBuffer<Vertex>(MAX_BUFFER_SIZE);

    void runScan();

public:
    ThreeScanApp(StoragePtr storage, SweepESP32Ptr sweep, PreciseServoPtr servo);

    void loadFromEEPROM();

    void saveToEEPROM();

    void loadDefaultSettings();

    AppSettings &getSettings();

    void setup() override;

    void loop() override;

    void startScan();

    void endScan();

    void saveData();
};


#endif //TIL_INSTALLATION_H
