
#define TINY_GSM_MODEM_SIM900
#include "utilities.h"
#include "about_sim.h"
#include "about_machine.h"


void setup() {
  Serial.begin(115200);
  initiateSIMCard();
}
void loop() {

    if (SerialAT.available()) {
        if(newSMS()){
          Serial.println("SerialAT have new SMS");
          digitalWritePulse20Bath();
        }
    }
    if (Serial.available()) {
        if(newSMS()){
          digitalWritePulse20Bath();
          Serial.println("Serial only have new SMS");
        }
    }
    delay(3000); // delay 3S
}