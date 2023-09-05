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

#include "stdbool.h"
#include "avr/eeprom.h"
#include "util/delay.h"

#include "i2c.h"
#include "max17263_regmap.h"





// Debugging
#define I2C_DEBUG
//#undef  I2C_DEBUG

// Monitor Fuel Gauge
#define MONITOR
#undef  MONITOR

// max17263 i2c address
#define MAX17263_I2C_ADDR	0x36




// Data structure for MAX17263 functionality
typedef struct max17263_t{

	// I2C address
	uint8_t addr;
	
	// Sense resistor value in mOhm
	uint8_t rsense;

	// Configuration registers
	__DesignCap_t DesignCap;
	__VEmpty_t    VEmpty;
	__ModelCfg_t  ModelCfg;
	__IChgTerm_t  IChgTerm;
	__LEDCfg1_t LEDCfg1;
	__LEDCfg2_t LEDCfg2;
	__LEDCfg3_t LEDCfg3;
	
	// Learned Parameters registers
	uint16_t RCOMP;
	uint16_t TempCo;
	uint16_t FullCapRep;
	uint16_t Cycles;
	uint16_t FullCapNom;

	// Fuel Gauge registers
	uint16_t RepCap;
	uint16_t RepSOC;
	uint16_t TTE;
}Max17263_t;

// MAX17263 data struct global instance
extern volatile Max17263_t max17263;




// read/write functions
uint16_t max_readRegister(uint8_t reg);
void max_writeRegister(uint8_t reg, uint16_t data);
void max_writeAndVerifyRegister(uint8_t reg, uint16_t data);




// MAX17263 configuration
void max_loadConfig(void);
void max_setCellCap(uint16_t mAh);
void max_setChargeTerm(uint16_t mA);
void max_setEmptyVoltage(uint8_t voltage);
void max_setSenseResistor(uint8_t mOhm);
void max_setRecoveryVoltage(uint8_t voltage);

// LED settings
#define LED_MAX_BARS  0x0F
#define LED_MIN_BARS  0x00
#define LED_MODE_OFF                0
#define LED_MODE_PUSH_BUTTON        2
#define LED_MODE_PUSH_BUTTON_TIMER  1
#define LED_MODE_ALWAYS_ON          3
#define LED_ANI_MODE_OFF            0
#define LED_ANI_MODE_FILL           1
#define LED_ANI_MODE_BREATHE        2
#define LED_ANI_MODE_FILL_BREATHE   3
#define LED_TIME_100MS              0
#define LED_TIME_300MS              1
#define LED_TIME_600MS              2
#define LED_TIME_1300MS             3
#define LED_TIME_2700MS             4
#define LED_TIME_5500MS             5
#define LED_TIME_8600MS             6
#define LED_TIME_10S                7
#define LED_MAX_BRIGHTNESS          31
#define LED_MIN_BRIGHTNESS          0

// LED driver operation settings
void max_setLEDBars(uint8_t bars);
void max_enLEDGrayScale(bool en);
void max_enLEDChargeIndicator(bool en);
void max_setLEDMode(uint8_t md);
void max_setLEDAniMode(uint8_t md);
void max_setLEDAniStep(uint8_t step);
void max_setLEDTimer(uint8_t time);
void max_setLEDBrightness(uint8_t brightness);
void max_enLEDFullBlink(bool en);
void max_enLEDEmptyBlink(bool en);
void max_enLEDGrayBlink(bool en);
void max_enLEDAutoCount(bool en);
void max_setLEDVoltage(uint8_t voltage);
void max_setDLED(uint8_t dled);




// max17263 functionality
void max_readFuelGauge(void);
void max_saveLearnedParameters(void);
uint16_t max_checkPOR(void);
uint8_t max_checkCycles(void);




// First word of eeprom to validate data
#define EEPROM_CHECK_ADDR			0x0000
#define EEPROM_CHECK_VAL			0xBEEF

// Learned parameters eeprom r/w addresses
#define EEPROM_RCOMP0_ADDR			0x0002
#define EEPROM_TempCo_ADDR			0x0004
#define EEPROM_FullCapRep_ADDR		0x0006
#define EEPROM_Cycles_ADDR			0x0008
#define EEPROM_FullCapNom_ADDR		0x000A

// max17263 save/load functions
void max_eepromSaveParameters(void);
void max_eepromLoadParameters(void);




// Debug i2c address
#define DEBUG_ADDR					0x69

// Debug parsing codes
#define DEBUG_STARTUP_CODE			0xAAAA
#define DEBUG_DONE_STARTUP_CODE		0xBBBB
#define DEBUG_POR_CODE				0xCCCC
#define DEBUG_STRUCT_CODE			0xDDDD
#define DEBUG_EEPROM_CODE			0xEEEE
#define DEBUG_EEPROM_INIT_CODE		0xAABB
#define DEBUG_FUEL_GAUGE_CODE		0xCCDD

// debugging functions
void max_debugRead(uint8_t addr, uint8_t reg);
void max_debugWrite(uint8_t addr, uint16_t data);
void max_debugWriteCode(uint8_t addr, uint16_t code, uint16_t data);
void max_debugDataStruct(void);
void max_debugFuelGauge(void);
void max_debugEEPROM(void);
void max_debugLED(void);



#endif /* MAX17263_H_ */