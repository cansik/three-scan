//
// Created by Florian Bruggisser on 2019-05-21.
//

#include <util/ESPUtils.h>
#include "SerialRemoteControl.h"

SerialRemoteControl::SerialRemoteControl(ThreeScanApp *app, ScanState *scanState) {
    this->app = app;
    this->scanState = scanState;
}

void SerialRemoteControl::setup() {
    BaseController::setup();

    Serial.setTimeout(20);
}

void SerialRemoteControl::loop() {
    BaseController::loop();

    if (timer.elapsed()) {
        // read command
        inputString = Serial.readStringUntil('\n');

        // if no input -> opt out
        if (inputString.length() == 0) {
            return;
        }

        parseCommands(inputString);
    }
}

void SerialRemoteControl::parseCommands(const String &input) {
    // received accelerometer data
    /*
    if (input.startsWith("DAT:ACC")) {
        int x = 0;
        int y = 0;
        int z = 0;

        // parse data
        sscanf(input.c_str(), "DAT:ACC;%d;%d;%d", &x, &y, &z);
    }
     */

    // restart device
    if (input.startsWith("CMD:RESTART")) {
        ESPUtils::hardRestart();
    }

    // start scan
    if (input.startsWith("CMD:START")) {
        scanState->requestStartScan = true;
    }

    // stop scan
    if (input.startsWith("CMD:STOP")) {
        scanState->requestStopScan = true;
    }

    // list settings
    if (input.startsWith("CMD:LST")) {
        listSettings();
    }

    // change serial transfer
    if (input.startsWith("CMD:SET")) {
        updateSetting(input);
    }

}

void SerialRemoteControl::listSettings() {
    Serial.printf("---- Settings ----\n"
                  "Version: %d\n"
                  "Start Angle: %f\n"
                  "End Angle: %f\n"
                  "Angle Step: %f\n"
                  "Slice Iteration Count: %d\n"
                  "Stand Filter Size: %d\n"
                  "Min Signal Strength: %d\n"
                  "Serial Transfer: %s\n"
                  "------------------\n",
                  app->getSettings().getVersion(),
                  app->getSettings().getStartAngle(),
                  app->getSettings().getEndAngle(),
                  app->getSettings().getAngleStep(),
                  app->getSettings().getSliceIterationCount(),
                  app->getSettings().getStandFilterSize(),
                  app->getSettings().getMinSignalStrength(),
                  app->getSettings().isSerialTransfer() ? "true" : "false");
}

void SerialRemoteControl::updateSetting(const String &input) {
    auto inputCString = input.c_str();

    // update serial transfer
    if (input.startsWith("CMD:SET:ST")) {
        int enabled = 0;
        sscanf(inputCString, "CMD:SET:ST:%d", &enabled);
        app->getSettings().setSerialTransfer(enabled == 1);

        Serial.printf("Serial Transfer: %s\n", app->getSettings().isSerialTransfer() ? "true" : "false");
    }
}
