#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ratexx_2g";
const char* password = "pongpong";

void setup() {
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
   // Make GET request
  HTTPClient http;
  http.begin("https://dummy.restapiexample.com/api/v1/employees"); // Replace with your API endpoint
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("[HTTP] GET request failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void loop() {
  Serial.println("Hello, World!");
  delay(1000);
}
