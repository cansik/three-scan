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
#include "EEPROM.h"
#include "model/AppSettings.h"
#include "controller/BaseController.h"

#define EEPROM_SIZE 0xFF
#define EEPROM_START_ADDRESS 0x00

class ThreeScanApp : public BaseController {
private:
    typedef SDCardStorage *StoragePtr;
    typedef SweepESP32 *SweepESP32Ptr;
    typedef PreciseServo *PreciseServoPtr;

    AppSettings settings;
    ScanSettings scanSettings;

    StoragePtr storage;
    SweepESP32Ptr sweep;
    PreciseServoPtr servo;

    Timer scanTimer = Timer(33);

    bool scanning = false;
    float currentAngle = 0.0f;
    bool currentAngleChanged = false;
    int pointCounter = 0;
    bool waitForSync = false;

    String data = "xa,xy,distance\n";

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
};


#endif //TIL_INSTALLATION_H
