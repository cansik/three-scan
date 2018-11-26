#ifndef TIL_INSTALLATION_H
#define TIL_INSTALLATION_H

#include <EEPROM.h>
#include <assert.h>
#include <controller/storage/SDCardStorage.h>
#include <driver/sweep/SweepESP32.h>
#include <driver/motor/PreciseServo.h>
#include <util/Timer.h>
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

#define MAX_BUFFER_SIZE 5000

class ThreeScanApp : public BaseController {
private:
    typedef SDCardStorage *StoragePtr;
    typedef SweepESP32 *SweepESP32Ptr;
    typedef PreciseServo *PreciseServoPtr;
    typedef CloudFile *CloudFilePtr;

    AppSettings settings;

    StoragePtr storage;
    SweepESP32Ptr sweep;
    PreciseServoPtr servo;
    CloudFilePtr cloudFile;

    Timer syncTimeoutTimer = Timer(2000);

    bool scanning = false;
    float currentAngle = 0.0f;
    bool currentAngleChanged = false;
    bool waitForSync = false;
    bool isFirstAngle = true;

    int pointCounter = 0;
    int fullPointCounter = 0;

    int maxPointCount = 0;

    int currentSliceIteration = 0;

    MutableBuffer<Vertex> buffer = MutableBuffer<Vertex>(MAX_BUFFER_SIZE);

    void runScan();

    void endScan();

    void saveData();

public:
    ThreeScanApp(StoragePtr storage, SweepESP32Ptr sweep, PreciseServoPtr servo);

    void loadFromEEPROM();

    void saveToEEPROM();

    void loadDefaultSettings();

    AppSettings &getSettings();

    void setup() override;

    void loop() override;

    void startScan();

    float getScanProgress();

    bool isSDMounted();

    bool isScanning() const;
};


#endif //TIL_INSTALLATION_H
