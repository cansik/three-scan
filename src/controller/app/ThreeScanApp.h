#ifndef TIL_INSTALLATION_H
#define TIL_INSTALLATION_H

#include <EEPROM.h>
#include <assert.h>
#include "EEPROM.h"
#include "model/AppSettings.h"
#include "controller/BaseController.h"

#define EEPROM_SIZE 0xFF
#define EEPROM_START_ADDRESS 0x00

class ThreeScanApp : public BaseController {
private:
    AppSettings settings;

public:
    ThreeScanApp();

    void loadFromEEPROM();

    void saveToEEPROM();

    void loadDefaultSettings();

    AppSettings &getSettings();

    void setup() override;

    void loop() override;
};


#endif //TIL_INSTALLATION_H
