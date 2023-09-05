#include <Wire.h>

#define MAX17263_STARTUP        0xAAAA
#define MAX17263_STARTUP_DONE   0xBBBB
#define MAX17263_POR            0xCCCC
#define MAX17263_STRUCT         0xDDDD
#define MAX17263_EEPROM         0xEEEE
#define MAX17263_EEPROM_INIT	  0xAABB
#define MAX17263_FUEL_GAUGE 	  0xCCDD



void setup() {
  
  Serial.begin(9600);
  while (!Serial)
     delay(10);
  Serial.println("------------------------------------");
  Serial.println("\tMAX17263 I2C Debug");
  Serial.println("------------------------------------\n");


  Wire.begin(0x69);
  Wire.onReceive(i2c_event);
}


void loop() {
  delay(100);
}


void i2c_event(int len) {
  uint8_t i = 0;
  uint16_t code;
  uint16_t buffer;
  const char* struct_label[] = {
    "DesignCap : ", "IchgTerm  : ", "Vempty\t  : ", "ModelCFG  : ",
    "RepCap\t  : ", "RepSOC\t  : ", "TTE\t  : ", "RCOMP0\t  : ",
    "TempCo\t  : ", "FullCapRep: ", "Cycles\t  : ", "FullCapNom: "
  };
  const char* eeprom_label[] = {
    "RCOMP0\t  ", "TempCo\t  ", "FullCapRep", "Cycles\t  ", 
    "FullCapNom"
  };
  const char* gauge_label[] = {
    "RepCap\t  : ", "RepSOC\t  : ", "TTE\t  : "
  };

  Serial.println("Received " + String(len) + " bytes:\n");
  
  while(Wire.available()) {
    
    code = Wire.read();
    code |= (Wire.read() << 8);

    //Serial.print("\tCode: ");
    //Serial.println(code, HEX);
    
    switch(code) {

      case MAX17263_STARTUP:
        Serial.println("Startup Sequence...");
        break;
      
      case MAX17263_STARTUP_DONE:
        Serial.println("Startup Sequence Complete");
        break;

      case MAX17263_POR:
        Serial.println("Power On Reset (POR) Detected...");
        break;

      case MAX17263_STRUCT:
        Serial.println("\tMAX17263 DATA STRUCT\n");
        while(Wire.available()) {
          buffer = Wire.read();
          buffer |= (Wire.read() << 8);
          Serial.print(struct_label[i]);
          Serial.print("\t");
          Serial.print(buffer, HEX);
          Serial.print("\t");
          Serial.println(buffer, BIN);
          i++;
        }
        break;

      case MAX17263_EEPROM:
        Serial.println("\tEEPROM SAVED PARAMETERS\n");
        while(Wire.available()) {
        
          
          // eeprom address sent first
          Serial.print(eeprom_label[i]);
          buffer = Wire.read();
          buffer |= (Wire.read() << 8);
          Serial.print(" [");
          Serial.print(buffer, HEX);
          Serial.print("]");

          // eeprom data sent second
          buffer = Wire.read();
          buffer |= (Wire.read() << 8);
          Serial.print("\t");
          Serial.println(buffer, HEX);

          i++;
        }
        break;

      case MAX17263_EEPROM_INIT:
        Serial.println("No EEPROM Config Data...");
        Serial.println("Loading Config Data...");
        Serial.println("Starting Addr = [0x0002]");
        break;

      case MAX17263_FUEL_GAUGE:
        Serial.println("\tFUEL GAUGE READINGS\n");
        while(Wire.available()) {
          buffer = Wire.read();
          buffer |= (Wire.read() << 8);
          Serial.print(gauge_label[i]);
          Serial.print("\t");
          Serial.println(buffer, HEX);
          i++;
        }
        break;

      default:
        Serial.print(code, HEX);
        break;
    }

    Serial.println("\n------------------------------------\n");
  }
}