#include <TinyGsmClient.h>
#include "about_message_queue.h"

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

bool isPaymentMessageMatched(String message) {
    String unicodeTwenty = "00320030002E00300030"; // 20.00
    String receiveTransferInThai = "0E230E310E1A0E420E2D0E19"; // รับโอน Kbank
    String receivedMoneyInThai = "0E400E070E340E190E400E020E490E32"; // เงินเข้า BBL and Kbank
    String forcedPaymentMessage = "PONGODDS"; // เงินเข้า BBL and Kbank
    bool isTransferInThaiOrMoneyInThaiMatched = message.indexOf(receiveTransferInThai) != -1 ||
                                                message.indexOf(receivedMoneyInThai) != -1;
    bool isTextMatched = message.indexOf(unicodeTwenty) != -1 && isTransferInThaiOrMoneyInThaiMatched;
    bool isForcedPayment = message.indexOf(forcedPaymentMessage) != -1;

    return isTextMatched || isForcedPayment;

}

bool newSMS() {

    Serial.println("checkSMSAvailable >>> ");

    String data = readSerialDataSMS();
    if (!data.isEmpty()) {
        Serial.println("receive sms and produce >" + data);

        if (isPaymentMessageMatched(data)) {
            Serial.println("Message matched 20.00");
            return true;
        }

    }

    return false;

}


