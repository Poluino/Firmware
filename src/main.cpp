#include <Arduino.h>
#include "FirstBootCheck.h"

FirstBootCheck firstcheck = FirstBootCheck();

void setup(){
    Serial.begin(115200);
    firstcheck.begin();
    firstcheck.check_config();
}


void loop() {

}