//
// Created by Florian Bruggisser on 2019-06-04.
//

#include <esp_task_wdt.h>
#include "ESPUtils.h"

void ESPUtils::hardRestart() {
    esp_task_wdt_init(1, true);
    esp_task_wdt_add(nullptr);
    while (true);
}
