#include <Wire.h>

void setup() {
  
  Serial.begin(9600);
  while (!Serial)
     delay(10);
  Serial.println("\nI2C Debug");

  Wire.begin(0x69);
  Wire.onReceive(i2c_event);
}


void loop() {
  delay(100);
}


void i2c_event(int len) {
  
  Serial.println("Received " + String(len) + " bytes:\n");
  while(Wire.available()) {
    uint8_t data = Wire.read();
    Serial.print(data, HEX);
    Serial.print("\t");
    Serial.println(data, BIN);
   
  }
  Serial.println("\n--------------\n");
}