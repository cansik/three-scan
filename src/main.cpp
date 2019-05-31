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
#include <controller/interaction/SerialRemoteControl.h>
#include <model/ScanState.h>

#include "controller/BaseController.h"
#include "controller/app/ThreeScanApp.h"
#include "controller/network/NetworkController.h"
#include "controller/network/OTAController.h"
#include "controller/network/OscController.h"
#include "util/MathUtils.h"

#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

// global

// sd
#define SD_CONTROL_POWER_PIN 33
#define SD_CS_PIN 21
#define SD_SCK_PIN 18
#define SD_MOSI_PIN 23
#define SD_MISO_PIN 19

// servo
#define SERVO_PIN 2

// lidar
#define SWEEP_CONTROL_POWER_PIN 32
#define SWEEP_RX 27
#define SWEEP_TX 26

// ui
#define BTN_PIN 22
#define STATUS_LED_PIN 25

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

auto scanState = ScanState();

// typedefs
typedef BaseController *BaseControllerPtr;

// controllers
auto network = NetworkController(DEVICE_NAME, SSID_NAME, SSID_PASSWORD, WIFI_AP);
auto ota = OTAController(DEVICE_NAME, OTA_PASSWORD, OTA_PORT);
auto osc = OscController(OSC_IN_PORT, OSC_OUT_PORT);

// variables
auto sdCardStorage = SDCardStorage(SD_CONTROL_POWER_PIN, SD_CS_PIN, SD_MOSI_PIN, SD_MISO_PIN, SD_SCK_PIN);
auto sweep = SweepESP32(SWEEP_RX, SWEEP_TX, SWEEP_CONTROL_POWER_PIN);
auto servo = PreciseServo(SERVO_PIN);
auto heartBeatTimer = Timer(1000);

auto app = ThreeScanApp(&sdCardStorage, &sweep, &servo);
auto remoteControl = SerialRemoteControl(&app, &scanState);

// controller list
BaseControllerPtr controllers[] = {
        &network,
        &ota,
        &osc,
        &remoteControl,
};

// scan controller list
BaseControllerPtr scanControllers[] = {
        &app
};

// methods
void handleOsc(OSCMessage &msg);

void sendRefresh();

void checkDebugButton();

void scanLoop(void *parameter);

void setup() {
    Serial.begin(BAUD_RATE);

    // setup debug btn
    pinMode(BTN_PIN, INPUT_PULLDOWN);

    // setup status led
    StatusLed::setup(STATUS_LED_PIN);

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

    // setup lidar controller
    for (auto &controller : scanControllers) {
        controller->setup();
    }

    // run scan task
    xTaskCreate(
            scanLoop,         /* Task function. */
            "TaskOne",        /* String with name of task. */
            10000,            /* Stack size in bytes. */
            nullptr,          /* Parameter passed as input of the task */
            1,                /* Priority of the task. */
            nullptr);         /* Task handle. */

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

        // btn
        checkDebugButton();

        if (heartBeatTimer.elapsed())
            sendRefresh();
    }
}

void scanLoop(void *parameter) {
    while (scanState.scanLoopRunning) {
        // disable watchdog
        TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed = 1;
        TIMERG0.wdt_wprotect = 0;

        // start
        if (scanState.requestStartScan) {
            scanState.requestStartScan = false;
            app.startScan();
        }

        // stop
        if (scanState.requestStopScan) {
            scanState.requestStopScan = false;
            app.endScan();
        }

        for (auto &controller : scanControllers) {
            controller->loop();
        }
    }
    vTaskDelete(nullptr);
}

void checkDebugButton() {
    auto state = digitalRead(BTN_PIN);

    if (state && !debugButtonState) {
        // turns on
        debugButtonState = true;

        Serial.println("debug button pressed!");
        if (!app.isScanning())
            scanState.requestStartScan = true;
    }

    if (!state && debugButtonState) {
        // turns off
        debugButtonState = false;
    }
}

void handleOsc(OSCMessage &msg) {
    // global
    msg.dispatch("/threescan/start", [](OSCMessage &msg) {
        if (!app.isScanning())
            scanState.requestStartScan = true;
    });

    msg.dispatch("/threescan/stop", [](OSCMessage &msg) {
        if (app.isScanning())
            scanState.requestStopScan = true;
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
        app.getSettings().setAngleStep(msg.getFloat(0));
    });

    msg.dispatch("/threescan/sliceiteration", [](OSCMessage &msg) {
        app.getSettings().setSliceIterationCount(static_cast<unsigned int>(msg.getFloat(0)));
    });

    msg.dispatch("/threescan/serialtransfer", [](OSCMessage &msg) {
        app.getSettings().setSerialTransfer(msg.getFloat(0) > 0.5f);
    });

    msg.dispatch("/threescan/samplerate", [](OSCMessage &msg) {
        int sampleRate = static_cast<int>(msg.getFloat(0));

        if (sampleRate == 500)
            app.getSettings().setSampleRate(SAMPLE_RATE_CODE_500_HZ);

        if (sampleRate == 750)
            app.getSettings().setSampleRate(SAMPLE_RATE_CODE_750_HZ);

        if (sampleRate == 1000)
            app.getSettings().setSampleRate(SAMPLE_RATE_CODE_1000_HZ);
    });

    msg.dispatch("/threescan/motorspeed", [](OSCMessage &msg) {
        int motorSpeed = static_cast<int>(msg.getFloat(0));

        if (motorSpeed == 1)
            app.getSettings().setMotorSpeed(MOTOR_SPEED_CODE_1_HZ);

        if (motorSpeed == 5)
            app.getSettings().setMotorSpeed(MOTOR_SPEED_CODE_5_HZ);

        if (motorSpeed == 10)
            app.getSettings().setMotorSpeed(MOTOR_SPEED_CODE_10_HZ);
    });

    msg.dispatch("/threescan/strengthfilter", [](OSCMessage &msg) {
        app.getSettings().setMinSignalStrength(static_cast<unsigned int>(msg.getFloat(0)));
    });


    sendRefresh();
}

void sendRefresh() {
    // send infos
    osc.send("/threescan/sd/mounted", app.isSDMounted() ? 1.0f : 0.0f);
    osc.send("/threescan/sd/writing", app.isWriting() ? 1.0f : 0.0f);
    osc.send("/threescan/scan/running", app.isScanning() ? 1.0f : 0.0f);
    osc.send("/threescan/scan/progress", app.getScanProgress());
    osc.send("/threescan/scan/progress/text", String(app.getScanProgress() * 100.0f, 0) + "%");
    osc.send("/threescan/scan/points/text", String(app.getFullPointCounter()));
    osc.send("/threescan/scan/filteredpoints/text", String(app.getFilteredPoints()));
    osc.send("/threescan/scan/angle/text", String(MathUtils::round_n(app.getCurrentAngle(), 2), 2) + "°");

    // send settings
    osc.send("/threescan/startAngle", app.getSettings().getStartAngle());
    osc.send("/threescan/endAngle", app.getSettings().getEndAngle());
    osc.send("/threescan/angleStep/text", app.getSettings().getAngleStep());
    osc.send("/threescan/sliceiteration/text", app.getSettings().getSliceIterationCount());
    osc.send("/threescan/strengthfilter", app.getSettings().getMinSignalStrength());
    osc.send("/threescan/scan/file/text", app.getPath());
    osc.send("/threescan/scan/time/text", MathUtils::timeStampString(app.getElapsedTime()));
    osc.send("/threescan/serialtransfer", app.getSettings().isSerialTransfer() ? 1.0f : 0.0f);

    // motor speed
    if (app.getSettings().getMotorSpeed()[1] == '1')
        osc.send("/threescan/motorspeed/text", "1 Hz");

    if (app.getSettings().getMotorSpeed()[1] == '5')
        osc.send("/threescan/motorspeed/text", "5 Hz");

    if (app.getSettings().getMotorSpeed()[0] == '1' && app.getSettings().getMotorSpeed()[1] == '0')
        osc.send("/threescan/motorspeed/text", "10 Hz");

    // sample rate
    if (app.getSettings().getSampleRate()[1] == '1')
        osc.send("/threescan/samplerate/text", "500 Hz");

    if (app.getSettings().getSampleRate()[1] == '2')
        osc.send("/threescan/samplerate/text", "750 Hz");

    if (app.getSettings().getSampleRate()[1] == '3')
        osc.send("/threescan/samplerate/text", "1000 Hz");


}