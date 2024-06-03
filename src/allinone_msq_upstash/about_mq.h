
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <iostream>
#include <regex>
#include <string>
#include <ArduinoJson.h>






const char *KAFKA_CONSUMER_URL = "https://nearby-horse-14820-eu1-rest-kafka.upstash.io/consume/GROUP_NAME/GROUP_INSTANCE_NAME/i-paid";
const char *KAFKA_CONSUMER_AUTH_USER = "bmVhcmJ5LWhvcnNlLTE0ODIwJFl75GdJEUOcpvy0uvjGOvg2gYsgs79-EO7f0dQ";
const char *KAFKA_CONSUMER_AUTH_PASSWORD = "OTdlZDhiNmUtNTIxNS00MGQ1LWE1NTUtNTcyMWQwYWI0YWQ1";

const char *KAFKA_PROVIDER_URL = "https://nearby-horse-14820-eu1-rest-kafka.upstash.io/produce/i-paid/";
const char *KAFKA_PROVIDER_USERNAME = "bmVhcmJ5LWhvcnNlLTE0ODIwJFl75GdJEUOcpvy0uvjGOvg2gYsgs79-EO7f0dQ";
const char *KAFKA_PROVIDER_PASSWORD = "OTdlZDhiNmUtNTIxNS00MGQ1LWE1NTUtNTcyMWQwYWI0YWQ1";


String response;



struct ValueData {
    String topic;
    String machine;
    String price;
    // Add more fields as needed
};

struct MyDataModel {
    ValueData value;
    String topic;
    int partition;
    int offset;
    int timestamp;
    String key;
    String headers;

    // Add more fields as needed
};

void parseValue(const String &jsonValue, ValueData &data) {
    StaticJsonDocument<200> doc;
    deserializeJson(doc, jsonValue);

    data.topic = doc["topic"].as<String>();
    data.machine = doc["machine"].as<String>();
    data.price = doc["price"].as<String>();
    // Add parsing for additional fields if necessary
}
void parseJson(String jsondd) {
    Serial.println(jsondd);

    // Example JSON data
    const char *json = jsondd.c_str();
    ;

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
        Serial.println("Value - Machine: " + data.value.machine);
        Serial.println("Value - Price: " + data.value.price);

        if (data.value.price == "20B") {
            Serial.print("Received 20 Bath");
        }
    }
}

void kafkaConsumePaymentData() {
    WiFiClientSecure client;
    client.setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] Consumer begin...\n");

    https.addHeader("Kafka-Auto-Offset-Reset", "earliest");
    https.addHeader("Content-Type", "application/json");
    https.setAuthorization(KAFKA_CONSUMER_AUTH_USER, KAFKA_CONSUMER_AUTH_PASSWORD);

    if (https.begin(client, KAFKA_CONSUMER_URL)) {
        int httpCode = https.GET();
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println("Raw payload data:");
            parseJson(payload);
        } else {
            Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }

        https.end();

    } else {
        Serial.printf("[HTTPS] Unable to connect\n");
    }
    Serial.println("Waiting 2min before the next round...");
    delay(500);
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

    String jsonData = "{\"topic\":\"\",\"machine\":\"A\",\"price\":\"20B\"}";  // เปลี่ยนข้อมูล JSON ตามที่ต้องการ

    String url = String(KAFKA_PROVIDER_URL) + jsonData;

    Serial.println("URL ENCODE >>" + url);

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
    delay(1000);
}

