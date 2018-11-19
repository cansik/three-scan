#include <Arduino.h>
#include <Wire.h>
#include <ESPmDNS.h>
#include <inttypes.h>
#include <SPI.h>
#include <SD.h>
#include <controller/storage/SDCardStorage.h>
#include <util/StatusLed.h>
#include <driver/Sweep/Sweep.h>
#include <driver/sweep/SweepESP32.h>
#include <driver/motor/PreciseServo.h>

#include "controller/BaseController.h"
#include "controller/app/ThreeScanApp.h"
#include "controller/network/NetworkController.h"
#include "controller/network/OTAController.h"
#include "controller/network/OscController.h"
#include "util/MathUtils.h"

// global
#define SD_SELECT_PIN 12

#define SERVO_PIN 4

#define SWEEP_HW_SERIAL 1

#define SWEEP_TX 26
#define SWEEP_RX 27

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

// controllers
auto network = NetworkController(DEVICE_NAME, SSID_NAME, SSID_PASSWORD, WIFI_AP);
auto ota = OTAController(DEVICE_NAME, OTA_PASSWORD, OTA_PORT);
auto osc = OscController(OSC_IN_PORT, OSC_OUT_PORT);

// variables
auto sdCardStorage = SDCardStorage(SD_SELECT_PIN);
auto sweep = SweepESP32(SWEEP_RX, SWEEP_TX, HardwareSerial(SWEEP_HW_SERIAL));
auto servo = PreciseServo(SERVO_PIN);

auto app = ThreeScanApp(&sdCardStorage, &sweep, &servo);

// controller list
BaseControllerPtr controllers[] = {
        &network,
        &ota,
        &osc,
        &app
};

// methods
void handleOsc(OSCMessage &msg);

void sendRefresh();

void setup() {
    Serial.begin(BAUD_RATE);

    // setup status led
    StatusLed::setup();

    // wait some seconds for debugging
    delay(5000);

    StatusLed::turnOn();

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


    delay(3000);
    app.startScan();
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