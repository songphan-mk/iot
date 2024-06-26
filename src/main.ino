
#define TINY_GSM_MODEM_SIM800

#include "all_in_sim/utilities.h"
#include "all_in_sim/about_sim.h"
#include "all_in_sim/about_machine.h"
#include "all_in_sim/about_wifi.h"

#define ESP_PINOUT (0)

void setup() {
    Serial.begin(115200);
    pinMode(ESP_PINOUT, OUTPUT);
    digitalWrite(ESP_PINOUT, HIGH);
    initiateSIMCard();
}

void loop() {

    if (newSMS()) {
        digitalWritePulse20Bath();
        Serial.println("SerialAT only have new SMS");
    }

    delay(1000); // delay 3S
}
