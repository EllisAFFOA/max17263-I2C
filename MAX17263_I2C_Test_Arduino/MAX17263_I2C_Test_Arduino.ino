#ifndef   F_CPU
  #define F_CPU 8000000UL
#endif


#include "avr/io.h"
#include "avr/sleep.h"
#include "avr/power.h"
#include "avr/wdt.h"
#include "avr/eeprom.h"
#include "util/delay.h"

#include "i2c.h"
#include "max17263.h"

#define F_TIMER1      7812.5
#define COUNT_1_MIN   60/8

#define LED_PIN   PORTC7
#define LED_DIR   DDRC
#define LED_PORT  PORTC
#define LED_READ  PINC

#define I2C_PORT  PORTD
#define I2C_DIR   DDRD
#define SCL_PIN   PORTD0
#define SCL_READ  PIND
#define SDA_PIN   PORTD1
#define SDA_READ  PIND


volatile uint8_t sleep_count = 0;
ISR(WDT_vect) {
  sleep_count++;
  wdt_disable();
}

void wdt_on(void) {
  MCUSR  = 0;                                   // Clear reset flags 
  WDTCSR = (_BV(WDCE) | _BV(WDE));              // Enable Change bit
  WDTCSR = (_BV(WDP3) | _BV(WDP0) | _BV(WDIE)); // Set timeout to 8s, Enable WDT interrupts 
  wdt_reset();
}

void io_init(void) {
  cli();
  
  // Disable pullups SDA, SCL
	I2C_DIR  |= (_BV(SDA_PIN)  | _BV(SCL_PIN));
	I2C_PORT |= (_BV(SDA_PIN) | _BV(SCL_PIN));

  // Enable LED monitor, output low
  LED_DIR  |= _BV(LED_PIN);
  LED_PORT &= _BV(LED_PIN);
  
  sei();
}

void start_sleep(void) {
  cli();
  ADCSRA = 0;
  wdt_on();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sei();
}


void process_battery(void) {
  
  // Power on reset has occured
  // We need to reload configuration
  if (max_checkPOR())
    max_loadConfig();

  // Save learned parameters 
  // When bit 6 of Cycles Reg has toggled
  if (max_checkCycles())
    max_saveLearnedParameters();

  #ifdef MONITOR
    max_readFuelGauge();
  #endif
  
  #ifdef DEBUG
    //max_debugDataStruct();
    //max_debugEEPROM();
  #endif
}

void setup() {

  #ifdef DEBUG
    while(!Serial);
    Serial.begin(9600);
		Serial.println("Start MAX17263 Test");
    _delay_ms(1000);
  #endif

  
  uint8_t buffer[22];
  uint16_t read;
  uint16_t data[11] = {
	DEBUG_EEPROM_CODE,
	EEPROM_RCOMP0_ADDR, eeprom_read_word((uint16_t *) EEPROM_RCOMP0_ADDR),
	EEPROM_TempCo_ADDR, eeprom_read_word((uint16_t *) EEPROM_TempCo_ADDR),
	EEPROM_FullCapRep_ADDR, eeprom_read_word((uint16_t *) EEPROM_FullCapRep_ADDR),
	EEPROM_Cycles_ADDR, eeprom_read_word((uint16_t *) EEPROM_Cycles_ADDR),
	EEPROM_FullCapNom_ADDR, eeprom_read_word((uint16_t *) EEPROM_FullCapNom_ADDR),
  };

  io_init();
	i2c_init(F_CPU, I2C_SCL_100KHZ);

  // set cell capacity and charge termination
  max_setSenseResistor(10);
  max_setCellCap(1200);
  max_setChargeTerm(100);
  
  // set LED driver operation
  max_setLEDBars(4);
  max_setLEDMode(LED_MODE_PUSH_BUTTON_TIMER);
  max_setLEDBrightness(LED_MAX_BRIGHTNESS);
  max_setLEDTimer(LED_TIME_1300MS);
  max_enLEDChargeIndicator(true);
  //max_enLEDEmptyBlink(true);

  // load configuration settings
	max_loadConfig();

  #ifdef DEBUG
		Serial.println("Done with setup");
  #endif
}

void loop() {

  while(1) {
    uint8_t buffer[8];
    uint16_t data[4] = {
      0x0123, 0x4567, 0x89AB, 0xCDEF
    };
    
    for(uint8_t i = 0; i < 8; i+=3) {
    buffer[i] = (uint8_t)(data[i%2] & 0x00FF);
    buffer[i+1] = (uint8_t)((data[i%2] >> 8) & 0x00FF);
    }
    for(uint8_t i = 0; i < 8; i++) {
      Serial.println(buffer[i], HEX);
    }
  }

  // Request battery data every X minutes
  if (sleep_count == 2) {
    process_battery();
    LED_PORT ^= _BV(LED_PIN);
    sleep_count = 0;
  }

  // enter sleep
  start_sleep();
  sleep_cpu();
  /**

    sleeping...

  **/
  sleep_disable();

  /*
  cli();
  
  // Power on reset has occured
  // We need to reload configuration
  if (max_checkPOR())
    max_loadConfig();

  
  // Save learned parameters 
  // When bit 6 of Cycles Reg has toggled
  if (max_checkCycles())
    max_saveLearnedParameters();

  
  #ifdef MONITOR
    //max_readFuelGauge();
  #endif
  
  #ifdef DEBUG
    max_debugDataStruct();
    max_debugEEPROM();
  #endif

  //Serial.println(TWCR, BIN);

  start_sleep();
  sleep_cpu();

  //Serial.println(TWCR, BIN);
  //Serial.println(EIFR, BIN);
  //Serial.println(USBINT, BIN);
  //Serial.println(UDINT, BIN);
  

  //Serial.println("Done Sleep");
  //while(1);
  
  _delay_ms(1000);
  */
}
