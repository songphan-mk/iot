#include <TinyGsmClient.h>
#include "utilities.h"

// Define the serial console for debug prints, if needed
//#define TINY_GSM_DEBUG SerialMon

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// See all AT commands, if wanted
//#define DUMP_AT_COMMANDS



#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif


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

void initiateSIMCard()
{

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
    Serial.print("Init success, start to send message to  ");
    modem.sendAT(GF("+CNMI=1,2,0,0,0"));
    SerialMon.println("GSM module is ready to receive SMS");

}

bool newSMS() {
    bool rtnFlag = false;
    const char *unicodeTwenty = "00320030002E00300030";
    Serial.println("checkSMSAvailable >>> ");
    if (SerialAT.available()||Serial.available()) {
        String data = readSerialDataSMS();
        if (data.indexOf(unicodeTwenty) != -1) {
            Serial.println("receive sms and produce >" + data);
            return true;
            //tricker open machine
            //kafkaProducePaymentData();
        }
        return false;
    } else {
        return false;
        
    }

}

