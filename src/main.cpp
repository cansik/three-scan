#include <Arduino.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <inttypes.h>
#include <SPI.h>
#include <SD.h>
#include <controller/driver/SDCardStorage.h>
#include <util/StatusLed.h>

#include "controller/BaseController.h"
#include "model/ThreeScanApp.h"
#include "controller/network/NetworkController.h"
#include "controller/network/OTAController.h"
#include "controller/network/OscController.h"
#include "util/MathUtils.h"

// global
#define SD_SELECT_PIN 12

// serial
#define BAUD_RATE 115200

// network
#define DEVICE_NAME "three-scanner"

#define SSID_NAME "three-scan"
#define SSID_PASSWORD "TILbildspur"

#define OTA_PASSWORD "bildspur"
#define OTA_PORT 8266

#define OSC_OUT_PORT 9000
#define OSC_IN_PORT 8000

// typedefs
typedef BaseController *BaseControllerPtr;

// variables
auto app = ThreeScanApp();
auto sdCardStorage = SDCardStorage(SD_SELECT_PIN);

// controllers
auto network = NetworkController(DEVICE_NAME, SSID_NAME, SSID_PASSWORD, WIFI_AP);
auto ota = OTAController(DEVICE_NAME, OTA_PASSWORD, OTA_PORT);
auto osc = OscController(OSC_IN_PORT, OSC_OUT_PORT);

// controller list
BaseControllerPtr controllers[] = {
        &network,
        &ota,
        &osc,
};

// methods
void handleOsc(OSCMessage &msg);

void sendRefresh();

void setup() {
    Serial.begin(BAUD_RATE);

    // setup status led
    //StatusLed::setup();

    // wait some seconds for debugging
    delay(5000);

    //StatusLed::turnOn();

    // setup sd card
    sdCardStorage.setup();
    sdCardStorage.printSDInfo();

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

    // add osc mdns
    MDNS.addService("_osc", "_udp", OSC_IN_PORT);

    Serial.println("setup finished!");
    sendRefresh();
}

void loop() {
    // loop controllers
    for (auto &controller : controllers) {
        controller->loop();
    }
}

void handleOsc(OSCMessage &msg) {
    // global
    msg.dispatch("/threescan/hello", [](OSCMessage &msg) {
        auto test = msg.getFloat(0);
    });

    sendRefresh();
}

void sendRefresh() {
    osc.send("/threescan/hello", 0);
}