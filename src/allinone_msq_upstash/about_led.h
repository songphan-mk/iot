// กำหนดพินสำหรับ LED
#define LED_SIM  27
#define LED_WIFI 33
#define LED_ON 1
#define LED_OFF 0
#define LED_BLINK 2

void initialLed(){
  // กำหนดโหมดของพิน LED เป็น OUTPUT
  pinMode(LED_SIM, OUTPUT);
  pinMode(LED_WIFI, OUTPUT);
  // เริ่มต้นด้วยการปิด LED
  digitalWrite(LED_SIM, LOW);
  digitalWrite(LED_WIFI, LOW);


}
void led(int input,int LED_PIN){
    // ตรวจสอบค่าที่ได้รับ
    if (input == 1) {
      Serial.println("Turning LED ON " + String(LED_PIN));
      digitalWrite(LED_PIN, HIGH); // เปิด LED
    } else if (input == 0) {
      Serial.println("Turning LED OFF " + String(LED_PIN));
      digitalWrite(LED_PIN, LOW); // ปิด LED
    } else if(input == 2){
      Serial.println("Turning LED on Blink " + String(LED_PIN));
      for(int i=0;i<10;i++){  
        Serial.println("Turning LED on Blink " + String(LED_PIN));
        digitalWrite(LED_PIN, LOW);
        delay(500);
        digitalWrite(LED_PIN, HIGH); // ปิด LED
        delay(500);
      }    
    }
    else {
      Serial.println("Invalid input. Enter '0' to turn on LED and '1' to turn off LED.");
    }

}