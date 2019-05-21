//
// Created by Florian Bruggisser on 2019-05-21.
//

#ifndef THREE_SCAN_SERIALREMOTECONTROL_H
#define THREE_SCAN_SERIALREMOTECONTROL_H


#include <controller/BaseController.h>
#include <controller/app/ThreeScanApp.h>

class SerialRemoteControl : public BaseController {

private:
    ThreeScanApp *app;

    Timer timer = Timer(1000 / 20);

    String inputString;

    void parseCommands(const String &input);

    void listSettings();

    void updateSetting(const String &input);

public:
    explicit SerialRemoteControl(ThreeScanApp *app);

    void setup() override;

    void loop() override;
};


#endif //THREE_SCAN_SERIALREMOTECONTROL_H
