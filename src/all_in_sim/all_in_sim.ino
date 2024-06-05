
#define TINY_GSM_MODEM_SIM800
#include "utilities.h"
#include "about_sim.h"
#include "about_machine.h"


void setup() {
  Serial.begin(115200);
  initiateSIMCard();
}
void loop() {

        if(newSMS()){
          digitalWritePulse20Bath();
          Serial.println("SerialAT only have new SMS");
        }
    //Serial.println("Serial only have new SMS");
    delay(1000); // delay 3S
}