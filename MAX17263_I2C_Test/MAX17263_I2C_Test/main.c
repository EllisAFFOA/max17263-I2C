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
#include "avr/eeprom.h"
#include "avr/sfr_defs.h"
#include "util/delay.h"

#include "i2c.h"
#include "max17263.h"
#include "max17263_regmap.h"


int main(void){
	
	uint16_t data;
	Max17263_t max;
	
	// disable pullups SDA, SCL
	DDRD  |= ((1 << PORTD0) | (1 << PORTD1));
	//PORTD |= ((1 << PORTD0) | (1 << PORTD1));
	PORTD &= ~((1 << PORTD0) | (1 << PORTD1));
	DDRD  &= ~((1 << PORTD0) | (1 << PORTD1));

	i2c_init(F_CPU, I2C_SCL_400KHZ);
	
	if (DEBUG)
		max_debugWrite(DEBUG_ADDR, DEBUG_STARTUP_CODE);
	
	max.config.designcap = 0x0960;
	max.config.ichgterm = IchgTerm_DEF;
	max.config.vempty = Vempty_DEF;
	max.config.modelcfg = REFRESH | R100;
	max_loadConfig(&max);
	
	_delay_ms(100);
	
	
	while(1){
		
		// power on reset has occured	
		if (max_checkPOR()) {
			
			if (DEBUG)	
				max_debugWrite(DEBUG_ADDR, DEBUG_POR_CODE);
			
			// configure
			max_loadConfig(&max);
		}
		
		// save learned parameters if bit 6 of Cycles Register has toggled
		if (max_checkCycles(&max)) {
			max_saveLearnedParameters(&max);
		}
		
		max_readFuelGauge(&max);
		
		if (DEBUG) {
			max_debugEEPROM();
			max_debugDataStruct(&max);
		}
		
		if (DEBUG_FUEL_GAUGE) {
			max_debugFuelGauge();
		}
		
		
		
		_delay_ms(1000);
    }
}