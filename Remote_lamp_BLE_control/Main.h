#ifndef MAIN_H
#define MAIN_H

#include "HandleInput.h"
#include "AdvData.h"

HandleInput hi;
AdvData ad;

void init(){
    Serial.begin(115200);

    Serial.println("Ninja lamp remote v2.5\n");
    Serial.println("To send a command use / and don't forget to add a line feed at the end.");
    Serial.println("For more information type /help.");
}

#endif