#define PULSE_OUT 0

void digitalWritePulse20Bath() {

    for (int i = 0; i <= 19; i++) {
        digitalWrite(PULSE_OUT, HIGH);
        delay(50);

        digitalWrite(PULSE_OUT, LOW);
        delay(100);
    }
}
void digitalWritePulse30Bath() {

    for (int i = 0; i <= 29; i++) {
        digitalWrite(PULSE_OUT, HIGH);
        delay(50);

        digitalWrite(PULSE_OUT, LOW);
        delay(100);
    }
}
