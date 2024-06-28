#include <Arduino.h>
#include "utilities.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
//#include <ArduinoJson.h>

const char *KAFKA_PROVIDER_URL = "https://natural-loon-5244-us1-rest-kafka.upstash.io/produce/payment-gateway/";
const char *KAFKA_PROVIDER_USERNAME = "bmF0dXJhbC1sb29uLTUyNDQk204VwE7q5xaYBmiOhIdorm5jVpTtVktS1cQqBtc";
const char *KAFKA_PROVIDER_PASSWORD = "ODQzMDJjNTYtNzE5Yi00ZjA2LWFlOGEtNTAzNjg2ZjFkMmQw";

void kafkaProducePaymentData() {
    WiFiClientSecure client;
    client.setInsecure();

    //create an HTTPClient instance
    HTTPClient https;

    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] Producer begin...\n");
    https.addHeader("Kafka-Auto-Offset-Reset", "earliest");
    https.addHeader("Content-Type", "application/json");
    https.setAuthorization(KAFKA_PROVIDER_USERNAME, KAFKA_PROVIDER_PASSWORD);

    String jsonData = "{\"price\":\"20B\"}";  // เปลี่ยนข้อมูล JSON ตามที่ต้องการ

    String url = String(KAFKA_PROVIDER_URL) + jsonData;

    Serial.println("URL ENCODE >>" + url);

    https.begin(client, url);

    int httpResponseCode = https.GET();


    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = https.getString();
        Serial.println("Response: " + response);
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }
    https.end();
}
