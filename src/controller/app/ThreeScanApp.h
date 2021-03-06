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
#include <util/StopWatch.h>
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
    StopWatch watch = StopWatch();

    volatile bool scanning = false;
    volatile float currentAngle = 0.0f;
    volatile bool writing = false;

    bool currentAngleChanged = false;
    bool waitForSync = false;
    bool isFirstAngle = true;

    int pointCounter = 0;
    volatile int fullPointCounter = 0;
    volatile int filteredPoints = 0;

    int maxPointCount = 0;

    int currentSliceIteration = 0;

    float filterStartAngle = 0;
    float filterEndAngle = 0;

    MutableBuffer<Vertex> buffer = MutableBuffer<Vertex>(MAX_BUFFER_SIZE);

    void runScan();

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

    void endScan();

    float getScanProgress();

    bool isSDMounted();

    bool isScanning() const;

    volatile float getCurrentAngle() const;

    volatile int getFullPointCounter() const;

    volatile int getFilteredPoints() const;

    volatile bool isWriting() const;

    const String &getPath() const;

    unsigned long getElapsedTime();
};


#endif //TIL_INSTALLATION_H
