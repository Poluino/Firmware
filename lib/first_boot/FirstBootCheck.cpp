#include "FirstBootCheck.h"
#include <Arduino.h>

int spiffs_init = 0;

FirstBootCheck::FirstBootCheck()
{
}

int FirstBootCheck::begin() {
    if (!SPIFFS.begin(true)) {
        Serial.println("Couldn't mount the SPIFFS!");
        spiffs_init = 0;
        return 0;
    } else {
        spiffs_init = 1;
        return 1;
    }
}



int FirstBootCheck::check_config() {
    if (!spiffs_init) {
        Serial.println("SPIFFS hasn't been initiliazed or failed to do so!");
        return 0;
    } else {
        if (!SPIFFS.exists("/config.json")) {
            Serial.println("File does not exist! Returning 0");
            return 0;
        } else {
            return 1;
        }
    }
}