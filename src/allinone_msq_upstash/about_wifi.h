
#include <WiFi.h>

const char* ssid = "ratexx_2g";
const char* password = "pongpong";
// const char* ssid = "Tenda_Newhome_2.4G";
// const char* password = "Newhome1234";



void initiateWifi() {
  // Connect to Wi-Fi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  Serial.print(" helloo ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println(" connected");
  // Print ESP32 IP Address
  Serial.println(WiFi.localIP());
}
// void loopWiFi() {
//   Serial.println("Produce >>> ");
//   produceViaWiFi();
//   delay(30000);
// }