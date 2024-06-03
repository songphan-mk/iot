
// #include <Arduino.h>
// #include "utilities.h"
// #include <WiFi.h>
// #include <HTTPClient.h>
// #include <WiFiClientSecure.h>
// #include <ArduinoJson.h>

#include "about_led.h"
#include "about_mq.h"
#include "about_sim.h"
#include "about_wifi.h"


#define PULSE_OUT 0

void digitalWritePulse20Bath() {

    for (int i = 0; i <= 19; i++) {
        digitalWrite(PULSE_OUT, HIGH);
        delay(50);

        digitalWrite(PULSE_OUT, LOW);
        delay(100);
    }
}

void setup() {
Serial.begin(115200);
   initialLed();
   initiateWifi();
   initiateSIMCard();
}


void loop() {
    RegStatus sim = modem.getRegistrationStatus();
    Serial.println("REG_UNREGISTERED >" + String(REG_UNREGISTERED));
    if (sim == 0) {
        led(LED_BLINK,LED_SIM);// digitalWrite(LED_SIM, LOW);
    }

    if (sim == REG_UNREGISTERED) {
        led(LED_BLINK,LED_SIM);// digitalWrite(LED_SIM, LOW);
    }
    if ((WiFi.status() == WL_CONNECTED)) {
        led(LED_ON,LED_WIFI);
        kafkaConsumePaymentData();
        checkSMSAvailable();
    } else {
        //digitalWrite(LED_WIFI, LOW);
    }

    
}