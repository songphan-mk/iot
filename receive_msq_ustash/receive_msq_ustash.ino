
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>

// Replace with your network credentials
// const char* ssid = "Tenda_Newhome_2.4G";
// const char* password = "Newhome1234";

const char* ssid = "ratexx_sim";
const char* password = "pongpong";

const char* kafkaURL = "https://nearby-horse-14820-eu1-rest-kafka.upstash.io/consume/GROUP_NAME/GROUP_INSTANCE_NAME/i-paid";
const char* kafkaAuthUser = "bmVhcmJ5LWhvcnNlLTE0ODIwJFl75GdJEUOcpvy0uvjGOvg2gYsgs79-EO7f0dQ";
const char* kafkaAuthPassword = "OTdlZDhiNmUtNTIxNS00MGQ1LWE1NTUtNTcyMWQwYWI0YWQ1";

// Define a structure to represent your data model
// Define a structure to represent the inner object in the "value" field
struct ValueData {
  String topic;
  String machine;
  String price;
  // Add more fields as needed
};

struct MyDataModel {
  ValueData value;
  String topic;
  int  partition; 
  int  offset; 
  int  timestamp;
  String key;
  String  headers;

  // Add more fields as needed
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output
  Serial.begin(115200);
  //Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  //Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
}




void parseValue(const String& jsonValue, ValueData& data) {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, jsonValue);

  data.topic = doc["topic"].as<String>();
  data.machine = doc["machine"].as<String>();
  data.price = doc["price"].as<String>();
  // Add parsing for additional fields if necessary
}

void parseJson(const String& payload, MyDataModel& data) {
  DynamicJsonDocument jsonDoc(1024); // Adjust size as needed
  DeserializationError error = deserializeJson(jsonDoc, payload);
  Serial.print("Call parsing JSON: ");
  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return; // Exit the function early if there's an error
  }
  
  data.topic = jsonDoc["topic"].as<String>(); // Convert to String
  data.key = jsonDoc["key"].as<String>(); // Convert to String

  data.topic = jsonDoc["topic"].as<String>();
  data.key = jsonDoc["key"].as<String>();
          // Example usage of parsed data
  Serial.println("Parsed data:");
  Serial.print("topic: ");
  Serial.println(data.topic);
  Serial.print("key: ");
  Serial.println(data.key);
        

}

void showSignal(String price){
    if (price == "20B") {
      digitalWrite(LED_BUILTIN, LOW);
      Serial.println("LED turned on");
    } else {
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.println("LED turned off");
    }}

// void loop(){
//   test_json();
//   delay(12000);
// }

void loop() {
  // wait for WiFi connection
  // #define OUTPIN 1;
  digitalWrite(D1, HIGH);
  digitalWrite(D1, LOW);
  digitalWrite(D2, HIGH);
  digitalWrite(D2, LOW);
  digitalWrite(D3, HIGH);
  digitalWrite(D3, LOW);
  digitalWrite(D4, HIGH);
  digitalWrite(D4, LOW);
  // if ((WiFi.status() == WL_CONNECTED)) {

  //   std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  //   // Ignore SSL certificate validation
  //   client->setInsecure();
    
  //   //create an HTTPClient instance
  //   HTTPClient https;
    
  //   //Initializing an HTTPS communication using the secure client
  //   Serial.print("[HTTPS] begin...\n");
  //   https.addHeader("Kafka-Auto-Offset-Reset", "earliest");
  //   https.addHeader("Content-Type", "application/json");
  //   https.setAuthorization(kafkaAuthUser, kafkaAuthPassword);
  //   if (https.begin(*client, kafkaURL)) {  // HTTPS
  //     Serial.print("[HTTPS] GET...\n");
  //     // start connection and send HTTP header
  //     int httpCode = https.GET();
  //     // httpCode will be negative on error
  //     if (httpCode > 0) {
  //       // HTTP header has been send and Server response header has been handled
  //       Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  //       // file found at server
  //       if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
  //         String payload = https.getString();  
  //         Serial.println("Raw JSON data:");
  //         pjson(payload);
  //         //Serial.println(payload); // Print the raw JSON data
  //       }  
        
        

  //     } else {
  //       Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
  //     }

  //     https.end();
  //   } else {
  //     Serial.printf("[HTTPS] Unable to connect\n");
  //   }
  // }
  
  // Serial.println("Waiting 2min before the next round...");
  delay(1000);
}

void pjson(String jsondd) {
  Serial.begin(115200);
  Serial.println(jsondd);

  // Example JSON data
  const char* json = jsondd.c_str();;

  // Parse JSON data
  StaticJsonDocument<200> doc;
  deserializeJson(doc, json);

  // Extract array from JSON data
  JsonArray array = doc.as<JsonArray>();

  // Iterate over each object in the array
  for (JsonObject obj : array) {
    // Extract values from each object
    MyDataModel data;
    data.topic = obj["topic"].as<String>();
    data.partition = obj["partition"];
    data.offset = obj["offset"];
    data.timestamp = obj["timestamp"];
    data.key = obj["key"].as<String>();
     // Parse the JSON string inside the 'value' field
    parseValue(obj["value"].as<String>(), data.value);
    
    
    

    // Example usage of parsed data
    Serial.println("Parsed data:");
    Serial.print("Topic: ");
    Serial.println(data.topic);
    Serial.print("Partition: ");
    Serial.println(data.partition);
    Serial.print("Offset: ");
    Serial.println(data.offset);
    Serial.print("Timestamp: ");
    Serial.println(data.timestamp);
    Serial.print("Key: ");
    Serial.println(data.key);
        Serial.print("Value - Topic: ");
    Serial.println(data.value.topic);
    Serial.print("Value - Machine: ");
    Serial.println(data.value.machine);
    Serial.print("Value - Price: ");
    Serial.println(data.value.price);
    showSignal(data.value.price);
    
  }
}


