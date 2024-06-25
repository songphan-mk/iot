#include <WiFi.h>

#define LED_WIFI 32
const char *ssid = "Santi";
const char *password = "0878128538";


void initiateWifi() {
    Serial.printf("Connecting to %s ", ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        digitalWrite(LED_WIFI, LOW);
        Serial.print('.');
        delay(1000);
    }

    digitalWrite(LED_WIFI, HIGH);

    Serial.printf("Connected to %s Ip %i", ssid,WiFi.localIP());

}
