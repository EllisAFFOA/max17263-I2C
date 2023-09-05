/*
 * main.c
 *
 * Created: 8/8/2023 1:34:37 PM
 *  Author: Ellis Hobby
 */ 

#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#include <xc.h>

#include "avr/io.h"
#include "avr/sleep.h"
#include "avr/power.h"
#include "avr/wdt.h"
#include "avr/eeprom.h"
#include "avr/interrupt.h"
#include "avr/sfr_defs.h"
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

	#ifdef I2C_DEBUG
		max_readFuelGauge();
		max_debugDataStruct();
		max_debugEEPROM();
	#endif
}


int main(void){
	
	#ifdef I2C_DEBUG
		max_debugWrite(DEBUG_ADDR, DEBUG_STARTUP_CODE);
	#endif
	
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
		max_debugWrite(DEBUG_ADDR, DEBUG_DONE_STARTUP_CODE);
	#endif
	
	while(1){
		
		// Request battery data every 1 minute
		if (sleep_count == COUNT_1_MIN) {
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
	}
}