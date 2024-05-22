
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <ArduinoJson.h>
#include <TinyGsmClient.h>

#define LED_WIFI 32
#define LED_SIM 33
#define LED_DC_IN 3
#define PULSE_OUT 0


// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Define the serial console for debug prints, if needed
#define TINY_GSM_MODEM_SIM800    // Modem is SIM800
#define TINY_GSM_RX_BUFFER 1024  // Set RX buffer to 1Kb
#define TINY_GSM_DEBUG SerialMon


const char *ssid = "ratexx_sim";
const char *password = "pongpong";

const char *KAFKA_CONSUMER_URL = "https://nearby-horse-14820-eu1-rest-kafka.upstash.io/consume/GROUP_NAME/GROUP_INSTANCE_NAME/i-paid";
const char *KAFKA_CONSUMER_AUTH_USER = "bmVhcmJ5LWhvcnNlLTE0ODIwJFl75GdJEUOcpvy0uvjGOvg2gYsgs79-EO7f0dQ";
const char *KAFKA_CONSUMER_AUTH_PASSWORD = "OTdlZDhiNmUtNTIxNS00MGQ1LWE1NTUtNTcyMWQwYWI0YWQ1";
const char *KAFKA_PROVIDER_URL = "https://nearby-horse-14820-eu1-rest-kafka.upstash.io/produce/i-paid/";
const char *KAFKA_PROVIDER_USERNAME = "bmVhcmJ5LWhvcnNlLTE0ODIwJFl75GdJEUOcpvy0uvjGOvg2gYsgs79-EO7f0dQ";
const char *KAFKA_PROVIDER_PASSWORD = "OTdlZDhiNmUtNTIxNS00MGQ1LWE1NTUtNTcyMWQwYWI0YWQ1";

#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif


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
    Serial.print("Connected");
    Serial.println(WiFi.localIP());
}

void initiateSIMCard() {
    // Turn on DC boost to power on the modem
#ifdef BOARD_POWERON_PIN
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
#endif

    // Set modem reset pin ,reset modem
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
    delay(100);
    digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL);
    delay(2600);
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

    Serial.println("Start Modem...");
    delay(3000);

    while (!modem.testAT()) {
        delay(10);
    }

    // Wait PB DONE
    delay(10000);

    modem.sendAT(GF("+CNMI=1,2,0,0,0"));
    SerialMon.println("GSM module is ready to receive SMS");
}

void kafkaConsumePaymentData() {
    std::unique_ptr <BearSSL::WiFiClientSecure> client(new BearSSL::WiFiClientSecure);
    client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] Consumer begin...\n");

    https.addHeader("Kafka-Auto-Offset-Reset", "earliest");
    https.addHeader("Content-Type", "application/json");
    https.setAuthorization(kafkaConsumeAuthUser, kafkaConsumeAuthPassword);

    if (https.begin(*client, kafkaConsumeURL)) {
        int httpCode = https.GET();
        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();
            Serial.println("Raw payload data:");
            pjson(payload);
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

void digitalWritePulse20Bath() {

    for (int i = 0; i <= 19; i++) {
        digitalWrite(BUILTIN_LED, HIGH);
        digitalWrite(D0, HIGH);
        delay(50);

        digitalWrite(D0, LOW);
        digitalWrite(BUILTIN_LED, LOW);
        delay(100);
    }

}

void parseJson(String jsondd) {
    Serial.println(jsondd);

    // Example JSON data
    const char *json = jsondd.c_str();;

    // Parse JSON data
    StaticJsonDocument<200> doc;
    deserializeJson(doc, json);

    // Extract array from JSON data
    JsonArray array = doc.as<JsonArray>();

    // Iterate over each object in the array
    for (JsonObject obj: array) {
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
            digitalWritePulse20Bath();
        }
    }
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

void checkSMSAvailable() {
    const char *unicodeTwenty = "00320030002E00300030";
    Serial.println("checkSMSAvailable >>> ");
    if (SerialAT.available()) {

        String data = readSerialDataSMS();

        if (data.indexOf(unicodeTwenty) != -1) {
            Serial.println("receive sms and produce >" + data);
            kafkaProducePaymentData();
        }
    } else {
        Serial.println("Not found SMS");
    }
    delay(12000);
}

void setup() {
    // PULSE OUTPUT
    pinMode(PULSE_OUT, OUTPUT);
    // LED WIFI
    pinMode(LED_WIFI, OUTPUT);
    // LED SIM
    pinMode(LED_SIM, OUTPUT);
    // DC IN
    pinMode(LED_DC_IN, OUTPUT);

    pinMode(BUILTIN_LED, OUTPUT);

    Serial.begin(115200);

    initiateWifi();
    initiateSIMCard();
}

void loop() {
    if ((WiFi.status() == WL_CONNECTED)) {
        kafkaConsumePaymentData();
    } else {
        digitalWrite(LED_WIFI, LOW);
    }
}
