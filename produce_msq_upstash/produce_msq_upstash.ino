/**
 * @file      SendSMS.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-11-16
 * @note      Not support T-SIM7672
 * *  Although the manual of SIM7672G states that it has the functions of making voice calls and sending text messages, 
 * *  the current firmware does not support it.
 */

#include "utilities.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <iostream>
#include <regex>
#include <string>

const char* ssid = "ratexx_sim";
const char* password = "pongpong";

// const char* ssid = "Tenda_Newhome_2.4G";
// const char* password = "Newhome1234";

const char* UPSTASH_KAFKA_REST_URL="https://nearby-horse-14820-eu1-rest-kafka.upstash.io/produce/i-paid/";
const char* UPSTASH_KAFKA_REST_USERNAME="bmVhcmJ5LWhvcnNlLTE0ODIwJFl75GdJEUOcpvy0uvjGOvg2gYsgs79-EO7f0dQ";
const char* UPSTASH_KAFKA_REST_PASSWORD="OTdlZDhiNmUtNTIxNS00MGQ1LWE1NTUtNTcyMWQwYWI0YWQ1";

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// See all AT commands, if wanted
#define DUMP_AT_COMMANDS

#define SMS_TARGET  "+66824979138" //Change the number you want to send sms message

#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

String response;

void setup_sim()
{
    Serial.begin(115200);
    // Turn on DC boost to power on the modem
#ifdef BOARD_POWERON_PIN
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
#endif

    // Set modem reset pin ,reset modem
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL); delay(100);
    digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL); delay(2600);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);

    // Turn on modem
    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(1000);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);

    // Set ring pin input
    pinMode(MODEM_RING_PIN, INPUT_PULLUP);

    // Set modem baud
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

    Serial.println("Start modem...");
    delay(3000);

    while (!modem.testAT()) {
        delay(10);
    }

    // Wait PB DONE
    delay(10000);

    Serial.print("Init success, start to send message to  ");
    Serial.println(SMS_TARGET);
    
    String imei = modem.getIMEI();
    Serial.printf("imei >>>>> ",imei);
    // bool res = modem.sendSMS(SMS_TARGET, String("Hello from ") + imei);

    Serial.print("Send sms message ");

    modem.sendAT(GF("+CNMI=1,2,0,0,0"));
    SerialMon.println("GSM module is ready to receive SMS");
}

void loop_sim() {
  // Check if data is available to read
  if (SerialAT.available()) {
     String data = readSerialDataSMS();
        // Convert String (which is UTF-8 encoded) to std::string if needed
        std::string utf8String = data.c_str();
        // Print the received data
        Serial.println(data); // Prints the data to the Serial monitor
        // Process the data as needed
        // Example: Print the std::string representation
        Serial.println(utf8String.c_str());
  }
  delay(12000);
}

bool isContainReceivedTwentyBaht(String& unicodeMessage) {
    // Regular expression pattern "00320030002E00300030" = "เงินเข้า 20.00"
    //00310036002F00300035002F00360037002000320030003A0033003700200E1A0E0A00200058002D003300300031003500200E400E070E340E190E400E020E490E32002000320030002E0030003000200E040E070E400E2B0E250E370E2D002000320032002C003500350035002E0038003300200E1A002E

  std::regex pattern("^.*00320030002E00300030.*$");
    // แปลง String ของ Arduino เป็น std::string
    std::string stdStr = std::string(unicodeMessage.c_str());

    
    // ใช้งาน std::regex_search กับ std::string
    return std::regex_search(stdStr, pattern);
}

String readSerialDataSMS() {
    String result;
    while (SerialAT.available()) {
        // Read a byte from the serial buffer
        char c = SerialAT.read();
        // Append the byte to the result string
        result += c;
    }
    return result;
}



void setupWiFi() {
  Serial.begin(115200);
  Serial.println(" helloo pong");
  // Connect to Wi-Fi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  Serial.print(" helloo ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("2");
  }
  Serial.println(" connected");

  // Print ESP32 IP Address
  Serial.println(WiFi.localIP());
}

void produceViaWiFi(){
    WiFiClientSecure client;
    // ข้ามการตรวจสอบใบรับรอง (ไม่แนะนำสำหรับการใช้งานจริง)
    client.setInsecure();

    //create an HTTPClient instance
    HTTPClient https;
    
    //Initializing an HTTPS communication using the secure client
    Serial.print("[HTTPS] begin...\n");
    https.addHeader("Kafka-Auto-Offset-Reset", "earliest");
    https.addHeader("Content-Type", "application/json");
    https.setAuthorization(UPSTASH_KAFKA_REST_USERNAME, UPSTASH_KAFKA_REST_PASSWORD);

 // ข้อมูล JSON ที่ต้องการส่ง (แปลงเป็น query parameters)
    //String jsonData = "{\"topic\":\"i-paid\"}"; // เปลี่ยนข้อมูล JSON ตามที่ต้องการ
    String jsonData = "{\"topic\":\"\",\"machine\":\"A\",\"price\":\"20B\"}"; // เปลี่ยนข้อมูล JSON ตามที่ต้องการ
    //String url = String(UPSTASH_KAFKA_REST_URL) + "?" + urlencode(jsonData);
    String url = String(UPSTASH_KAFKA_REST_URL) + jsonData;
    Serial.println("URL ENCODE >>"+url);
    https.begin(client, url);

    // ส่งคำขอ GET
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

  // รอ 10 วินาทีก่อนโพสต์อีกครั้ง
  delay(10000);
}



void loopWiFi() {
  Serial.println("Produce >>> ");
  produceViaWiFi();
  delay(30000);
}

void loop(){
  // Check if data is available to read
  Serial.println("Call loop function >>> ");
  if (SerialAT.available()) {
     String data = readSerialDataSMS();
     if(data.indexOf("00320030002E00300030") != -1) {
      Serial.println("receive sms and produce >" + data );
      produceViaWiFi();
     }
       

  }else{
    Serial.println("modem not available");
  }


  delay(12000);
}

void setup(){
  setupWiFi();
  setup_sim();
}


