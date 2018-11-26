#include <Arduino.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <inttypes.h>
#include <SPI.h>
#include <SD.h>
#include <controller/storage/SDCardStorage.h>
#include <util/StatusLed.h>
#include <driver/sweep/Sweep.h>
#include <driver/sweep/SweepESP32.h>
#include <driver/motor/PreciseServo.h>
#include <controller/network/HeartBeat.h>

#include "controller/BaseController.h"
#include "controller/app/ThreeScanApp.h"
#include "controller/network/NetworkController.h"
#include "controller/network/OTAController.h"
#include "controller/network/OscController.h"
#include "util/MathUtils.h"

// global
#define SD_SELECT_PIN 12

#define SERVO_PIN 4

#define SWEEP_RX 27
#define SWEEP_TX 26
#define PWR_PIN_1 15
#define PWR_PIN_2 5

#define DEBUG_BTN_PIN 14

// serial
#define BAUD_RATE 115200

// network
#define DEVICE_NAME "three-scanner"

#define SSID_NAME "three-scan"
#define SSID_PASSWORD "3scan-app"

#define OTA_PASSWORD "bildspur"
#define OTA_PORT 8266

#define OSC_OUT_PORT 9000
#define OSC_IN_PORT 8000

bool debugButtonState = false;

// typedefs
typedef BaseController *BaseControllerPtr;

// controllers
auto network = NetworkController(DEVICE_NAME, SSID_NAME, SSID_PASSWORD, WIFI_AP);
auto ota = OTAController(DEVICE_NAME, OTA_PASSWORD, OTA_PORT);
auto osc = OscController(OSC_IN_PORT, OSC_OUT_PORT);

// variables
auto sdCardStorage = SDCardStorage(SD_SELECT_PIN);
auto sweep = SweepESP32(SWEEP_RX, SWEEP_TX, PWR_PIN_1, PWR_PIN_2);
auto servo = PreciseServo(SERVO_PIN);
auto heartbeat = HeartBeat(1000);

auto app = ThreeScanApp(&sdCardStorage, &sweep, &servo);

// controller list
BaseControllerPtr controllers[] = {
        &network,
        &ota,
        &osc,
        &app,
        &heartbeat
};

// methods
void handleOsc(OSCMessage &msg);

void sendRefresh();

void checkDebugButton();

void setup() {
    Serial.begin(BAUD_RATE);

    // setup debug btn
    pinMode(DEBUG_BTN_PIN, INPUT_PULLDOWN);

    // setup status led
    StatusLed::setup();

    // wait some seconds for debugging
    if (Serial)
        delay(5000);

    StatusLed::turnOff();

    // setup random seed
    randomSeed(static_cast<unsigned long>(analogRead(0)));

    // load settings
    app.loadFromEEPROM();

    // setup controllers
    for (auto &controller : controllers) {
        controller->setup();
    }

    // setup handlers
    osc.onMessageReceived(handleOsc);
    heartbeat.onHeartbeat(sendRefresh);

    // add osc mdns
    MDNS.addService("_osc", "_udp", OSC_IN_PORT);

    Serial.println("setup finished!");
    sendRefresh();
}

void loop() {
    // loop controllers
    for (auto &controller : controllers) {
        controller->loop();

        // btn
        checkDebugButton();
    }
}

void checkDebugButton() {
    auto state = digitalRead(DEBUG_BTN_PIN);

    if (state && !debugButtonState) {
        // turns on
        debugButtonState = true;

        Serial.println("debug button pressed!");
        app.startScan();
    }

    if (!state && debugButtonState) {
        // turns off
        debugButtonState = false;
    }
}

void handleOsc(OSCMessage &msg) {
    // global
    msg.dispatch("/threescan/start", [](OSCMessage &msg) {
        app.startScan();
    });

    msg.dispatch("/threescan/save", [](OSCMessage &msg) {
        app.saveToEEPROM();
    });

    msg.dispatch("/threescan/load", [](OSCMessage &msg) {
        app.loadFromEEPROM();
    });

    msg.dispatch("/threescan/default", [](OSCMessage &msg) {
        app.loadDefaultSettings();
    });

    msg.dispatch("/threescan/refresh", [](OSCMessage &msg) {
        // just send a refresh
    });

    // settings
    msg.dispatch("/threescan/startAngle", [](OSCMessage &msg) {
        app.getSettings().setStartAngle(MathUtils::round_n(msg.getFloat(0), 0));
    });

    msg.dispatch("/threescan/endAngle", [](OSCMessage &msg) {
        app.getSettings().setEndAngle(MathUtils::round_n(msg.getFloat(0), 0));
    });

    msg.dispatch("/threescan/angleStep", [](OSCMessage &msg) {
        app.getSettings().setAngleStep(MathUtils::round_n(msg.getFloat(0), 2));
    });

    sendRefresh();
}

void sendRefresh() {
    osc.send("/threescan/sd/mounted", app.isSDMounted() ? 1.0f : 0.0f);
    osc.send("/threescan/scan/running", app.isScanning() ? 1.0f : 0.0f);
    osc.send("/threescan/scan/progress", app.getScanProgress());

    // send settings
    osc.send("/threescan/startAngle", app.getSettings().getStartAngle());
    osc.send("/threescan/endAngle", app.getSettings().getEndAngle());
    osc.send("/threescan/angleStep", app.getSettings().getAngleStep());
}