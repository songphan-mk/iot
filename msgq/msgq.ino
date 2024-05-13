#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "ratexx_2g";
const char* password = "pongpong";

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

 
}

void loop() {
  // Your code here
  Serial.println("Begin Consume topic i-paid");
  delay(1000);
 // Make HTTP request
  HTTPClient http;
  http.begin("https://nearby-horse-14820-eu1-rest-kafka.upstash.io/consume/GROUP_NAME/GROUP_INSTANCE_NAME/i-paid");
  http.addHeader("Kafka-Auto-Offset-Reset", "earliest");
  http.setAuthorization("bmVhcmJ5LWhvcnNlLTE0ODIwJFl75GdJEUOcpvy0uvjGOvg2gYsgs79-EO7f0dQ", "OTdlZDhiNmUtNTIxNS00MGQ1LWE1NTUtNTcyMWQwYWI0YWQ1");

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("HTTP error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
