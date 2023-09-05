/*
 * max17263.h
 *
 * Created: 8/8/2023 3:09:26 PM
 *  Author: Ellis Hobby
 */ 


#ifndef MAX17263_H_
#define MAX17263_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <xc.h>
#include "avr/io.h"
#include "avr/eeprom.h"
#include "util/delay.h"

#include "i2c.h"
#include "max17263_regmap.h"


// set to 1 for debugging
#define DEBUG 1

// set to 1 to include fuel gauge debugging
#define DEBUG_FUEL_GAUGE			1

// debug i2c address
#define DEBUG_ADDR					0x69

// debug parsing codes
#define DEBUG_STARTUP_CODE			0xAAAA
#define DEBUG_POR_CODE				0xBBBB
#define DEBUG_STRUCT_CODE			0xCCCC
#define DEBUG_EEPROM_CODE			0xDDDD
#define DEBUG_EEPROM_INIT_CODE		0xEEEE
#define DEBUG_FUEL_GAUGE_CODE		0xAABB

// max17263 i2c address
#define MAX17263_ADDR	0x36

// first word of eeprom to validate data
#define EEPROM_CHECK_ADDR			0x0000
#define EEPROM_CHECK_VAL			0xBEEF

// learned parameters eeprom r/w addresses
#define EEPROM_RCOMP0_ADDR			0x0002
#define EEPROM_TempCo_ADDR			0x0004
#define EEPROM_FullCapRep_ADDR		0x0006
#define EEPROM_Cycles_ADDR			0x0008
#define EEPROM_FullCapNom_ADDR		0x000A




// config values 
typedef	struct MaxConfig_t {
	uint16_t designcap;
	uint16_t ichgterm;
	uint16_t vempty;
	uint16_t modelcfg;
}MaxConfig_t;


// fuel gauge readings
typedef struct MaxFuelGauge_t {
	uint16_t repcap;
	uint16_t repsoc;
	uint16_t tte;
}MaxFuelGauge_t;


// learned parameters
typedef struct MaxLearned_t {
	uint16_t rcomp0;
	uint16_t tempco;
	uint16_t fullcaprep;
	uint16_t cycles;
	uint16_t fullcapnom;
}MaxLearned_t;


typedef struct MaxLED_t {
	uint16_t cfg1;
	uint16_t cfg2;
	uint16_t cfg3;
}MaxLED_t;


// full max17263 data structure
typedef struct Max17263_t {
	MaxConfig_t config;		// chosen config settings
	MaxFuelGauge_t gauge;	// fuel gauge reading
	MaxLearned_t learned;	// learned params, read every time Cycles.bit6 toggles	
	MaxLED_t led;			// LED settings
}Max17263_t;


// read/write functions
uint16_t max_readRegister(uint8_t reg);
void max_writeRegister(uint8_t reg, uint16_t data);
void max_writeAndVerifyRegister(uint8_t reg, uint16_t data);

// debugging functions
void max_debugRead(uint8_t addr, uint8_t reg);
void max_debugWrite(uint8_t addr, uint16_t data);
void max_debugWriteCode(uint8_t addr, uint16_t code, uint16_t data);
void max_debugDataStruct(Max17263_t* max);
void max_debugFuelGauge(void);
void max_debugEEPROM(void);
void max_debugLED(void);

// max17263 functionality 
uint16_t max_checkPOR(void);
void max_loadConfig(Max17263_t* max);
void max_initLED(void);
void max_readFuelGauge(Max17263_t* max);
uint8_t max_checkCycles(Max17263_t* max);
void max_saveLearnedParameters(Max17263_t* max);

void max_setLEDBars(uint8_t bars);
void max_setLEDGrayScale(uint8_t state);
void max_setLEDChargeIndicator(uint8_t lchg);
void max_setLEDMode(uint8_t md);
void max_setLEDAniMode(uint8_t md);
void max_setLEDAniStep(uint8_t step);
void max_setLEDTimer(uint8_t time);

// max17263 save/load functions
void max_eepromSaveParameters(Max17263_t* max);
void max_eepromLoadParameters(Max17263_t* max);

#endif /* MAX17263_H_ */