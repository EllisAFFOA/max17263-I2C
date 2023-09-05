/*
 * max17263.c
 *
 * Created: 8/11/2023 12:43:03 PM
 *  Author: Ellis Hobby
 */ 


#include "max17263.h"

volatile Max17263_t max17263 = {
	.addr = MAX17263_I2C_ADDR,
	.rsense = 10,
	.DesignCap.value = DesignCap_DEFAULT,
	.VEmpty.value = VEmpty_DEFAULT,
	.ModelCfg.value = ModelCfg_DEFAULT,
	.IChgTerm.value = IChgTerm_DEFAULT,
	.LEDCfg1.value = LEDCfg1_DEFAULT,
	.LEDCfg2.value = LEDCfg2_DEFAULT,
	.LEDCfg3.value = LEDCfg3_DEFAULT
};


/***********************************************************
 *
 * Read data from internal register
 * Stored into two byte buffer
 * Shifts LSB and MSB for returning word
 *
 * @param reg : register address to be read
 *
 ***********************************************************/
uint16_t max_readRegister(uint8_t reg) {
	uint8_t rx_buffer[2];
	rx_buffer[0] = reg;
	i2c_controller_transmit(max17263.addr, rx_buffer, 1, I2C_REPEAT);
	i2c_controller_receive(max17263.addr, rx_buffer, 2);
	return ((rx_buffer[1] << 8) | (rx_buffer[0]));
}


/***********************************************************
 *
 * Write data to internal register
 * Loads three bytes into buffer
 * Will transmit in following order:
 * 1 -> register
 * 2 -> data LSB
 * 3 -> data MSB
 *
 * @param reg  : register address to write
 * @param data : data to write
 *
 ***********************************************************/
void max_writeRegister(uint8_t reg, uint16_t data) {
	uint8_t tx_buffer[3];
	tx_buffer[0] = reg;
	tx_buffer[1] = (uint8_t)((data & 0x00FF));
	tx_buffer[2] = (uint8_t)((data >> 8) & 0x00FF);
	i2c_controller_transmit(max17263.addr, tx_buffer, 3, I2C_NO_REPEAT);
}


/***********************************************************
 *
 * Writes data to register and verifies data received
 * performs three attempts 
 *
 * @param reg  : register address to write
 * @param data : data to write
 *
 ***********************************************************/
void max_writeAndVerifyRegister(uint8_t reg, uint16_t data) {
	uint16_t buffer;
	uint8_t attempt = 0;
	do {
		max_writeRegister(reg, data);
		_delay_ms(1);
		buffer = max_readRegister(reg);
		attempt++;
	}while(data != buffer && (attempt < 3));
}


/***********************************************************
 *
 * Check POR bit in STATUS Register
 *
 * @returns : POR bit state
 *
 ***********************************************************/
uint16_t max_checkPOR(void) {
	return (max_readRegister(Status_REG_ADDR) & POR);	// return state of por bit in status reg
}


/***********************************************************
 *
 * Sets the DesignCap required for the cell being used
 *
 * @param mAh : millamp hours of cell
 *
 ***********************************************************/
void max_setCellCap(uint16_t mAh) { 
	float res_mAh = (0.005 / max17263.rsense) * 1000;					// Resolution in mAh (see UG6595 p.4 table 1)
	max17263.DesignCap.value  = (uint16_t)(mAh / res_mAh);				// Calculate design capacity value           
	max_writeRegister(DesignCap_REG_ADDR, max17263.DesignCap.value);	// Write DesignCap
}

/***********************************************************
 *
 * Sets the charge termination for system
 *
 * @param mA : charge termination of cell
 *
 ***********************************************************/
void max_setChargeTerm(uint16_t mA) { 
	float res_mAh = (0.0015625 / max17263.rsense) * 1000;				// Resolution in mAh (see UG6595 p.4 table 1)
	max17263.IChgTerm.value  = (uint16_t)(mA / res_mAh);				// Calculate charge termination value
	max_writeRegister(IChgTerm_REG_ADDR, max17263.IChgTerm.value);		// Write DesignCap
}

/***********************************************************
 *
 * Sets value of sense resistor in mOhm
 *
 * @param mOhm : resistance in milliohms
 *
 ***********************************************************/
void max_setSenseResistor(uint8_t mOhm) {
  max17263.rsense = mOhm;
}

/***********************************************************
 *
 * Ez Config steps described in MAX17263 user guide
 *
 * @param max : Max17263 data struct pointer
 *
 ***********************************************************/
void max_loadConfig(void) {
	
	uint16_t buffer;
	uint8_t check_flag = 0;
	
	#ifdef I2C_DEBUG
		max_debugWrite(DEBUG_ADDR, DEBUG_POR_CODE);
	#endif
	
	// check to see if saved parameters exits
	// (should be value 0xBEEF at address 0x00)
	buffer = eeprom_read_word((uint16_t*)EEPROM_CHECK_ADDR);
	
	// if no saved parameters initialize eeprom and flag
	if (buffer != EEPROM_CHECK_VAL) {
	eeprom_write_word((uint16_t*)EEPROM_CHECK_ADDR, EEPROM_CHECK_VAL);
		check_flag = 1;
	}

	// wait until FSTAT.DNR bit = 0 (warming up)
	while(max_readRegister(FStat_REG_ADDR) & DNR) {				
		_delay_ms(10);
	}

	buffer = max_readRegister(HibCfg_REG_ADDR);					// save original hibernate mode settings
	max_writeRegister(SoftWakeup_REG_ADDR, SoftWakeup_CLEAR);	// exit hibernate mode step 1 
	max_writeRegister(HibCfg_REG_ADDR, 0x0000);					// exit hibernate mode step 2 
	max_writeRegister(SoftWakeup_REG_ADDR, SoftWakeup_SOFT);	// exit hibernate mode step 3 
	
	// load configuration
	max_writeRegister(DesignCap_REG_ADDR, max17263.DesignCap.value);	// write DesignCap
	max_writeRegister(IChgTerm_REG_ADDR, max17263.IChgTerm.value);		// write IChgTerm
	max_writeRegister(VEmpty_REG_ADDR, max17263.VEmpty.value);			// write Vempty
	max_writeRegister(ModelCfg_REG_ADDR, max17263.ModelCfg.value);		// write ModelCfg
	
	// wait until MODELCFG.REFRESH = 0
	while(max_readRegister(ModelCfg_REG_ADDR) & ModelCfg_Refresh) {			
		_delay_ms(10);
	}
	
	// if no learned parameters exist in eeprom we need default
	if (check_flag) {
		max17263.RCOMP = max_readRegister(RCOMP0_REG_ADDR);
		max17263.TempCo = max_readRegister(TempCo_REG_ADDR);
		max17263.FullCapRep = max17263.DesignCap.value;
		max17263.Cycles = 0;
		max17263.FullCapNom = max17263.DesignCap.value;
		max_eepromSaveParameters();
	}
	
	// otherwise we can load the learned parameters from eeprom
	else {
		max_eepromLoadParameters();
	}
	
	// load learned parameters
	max_writeRegister(RCOMP0_REG_ADDR, max17263.RCOMP);
	max_writeRegister(TempCo_REG_ADDR, max17263.TempCo);
	max_writeRegister(FullCapRep_REG_ADDR, max17263.FullCapRep);
	max_writeRegister(Cycles_REG_ADDR, max17263.Cycles);
	max_writeRegister(FullCapNom_REG_ADDR, max17263.FullCapNom);
	
	// restore original hibernate settings
	max_writeRegister(HibCfg_REG_ADDR, buffer);						

	// set LED driver operation
	max_writeRegister(LEDCfg3_REG_ADDR, max17263.LEDCfg3.value);
	max_writeRegister(LEDCfg3_REG_ADDR, max17263.LEDCfg3.value);
	max_writeRegister(LEDCfg3_REG_ADDR, max17263.LEDCfg3.value);
	
	buffer = max_readRegister(Status_REG_ADDR);						// read status
	max_writeAndVerifyRegister(Status_REG_ADDR, (buffer & ~POR));	// clear por bit
}




/***********************************************************
 *
 * 
 *
 ***********************************************************/
void max_setLEDBars(uint8_t bars) {
	max17263.LEDCfg1.bit.Nbars = bars;
	max_writeRegister(LEDCfg1_REG_ADDR, max17263.LEDCfg1.value);
}

void max_enLEDGrayScale(bool en) {
	max17263.LEDCfg1.bit.Nbars = en;
	max_writeRegister(LEDCfg1_REG_ADDR, max17263.LEDCfg1.value);
} 

void max_enLEDChargeIndicator(bool en) {
	max17263.LEDCfg1.bit.LChg = en;
	max_writeRegister(LEDCfg1_REG_ADDR, max17263.LEDCfg1.value);
}

void max_setLEDMode(uint8_t md) {
	max17263.LEDCfg1.bit.LEDMd = md;
	max_writeRegister(LEDCfg1_REG_ADDR, max17263.LEDCfg1.value);
}

void max_setLEDAniMode(uint8_t md) {
	max17263.LEDCfg1.bit.AniMd = md;
	max_writeRegister(LEDCfg1_REG_ADDR, max17263.LEDCfg1.value);
}

void max_setLEDAniStep(uint8_t step) {
	max17263.LEDCfg1.bit.AniStep = step;
	max_writeRegister(LEDCfg1_REG_ADDR, max17263.LEDCfg1.value);
}

void max_setLEDTimer(uint8_t time) {
	max17263.LEDCfg1.bit.LEDTimer = time;
	max_writeRegister(LEDCfg1_REG_ADDR, max17263.LEDCfg1.value);
}


/***********************************************************
 *
 *
 *
 ***********************************************************/
 void max_setLEDBrightness(uint8_t brightness) {
	max17263.LEDCfg2.bit.Brightness = brightness;
	max_writeRegister(LEDCfg2_REG_ADDR, max17263.LEDCfg2.value);
} 

void max_enLEDFullBlink(bool en) {
	max17263.LEDCfg2.bit.FBlink = en;
	max_writeRegister(LEDCfg2_REG_ADDR, max17263.LEDCfg2.value);
}

void max_enLEDEmptyBlink(bool en) {
	max17263.LEDCfg2.bit.EBlink = en;
	max_writeRegister(LEDCfg2_REG_ADDR, max17263.LEDCfg2 .value);
}

void max_enLEDGrayBlink(bool en) {
	max17263.LEDCfg2.bit.GBlink = en;
	max_writeRegister(LEDCfg2_REG_ADDR, max17263.LEDCfg2.value);
}

void max_enLEDAutoCount(bool en) {
	max17263.LEDCfg2.bit.EnAutoLEDCnt = en;
	max_writeRegister(LEDCfg2_REG_ADDR, max17263.LEDCfg2.value);
}

void max_setLEDVoltage(uint8_t voltage) {
	max17263.LEDCfg2.bit.VLED = voltage;
	max_writeRegister(LEDCfg2_REG_ADDR, max17263.LEDCfg2.value);
}

void max_setDLED(uint8_t dled) {
	max17263.LEDCfg2.bit.DLED = dled;
	max_writeRegister(LEDCfg2_REG_ADDR, max17263.LEDCfg2.value);
}

/***********************************************************
 *
 * Read fuel gauge parameters, save into data structure
 *
 * @param max : Max17263 data struct pointer
 *
 ***********************************************************/
void max_readFuelGauge(void) {
	max17263.RepCap	= max_readRegister(RepCap_REG_ADDR);
	max17263.RepSOC	= max_readRegister(RepSOC_REG_ADDR);
	max17263.TTE	= max_readRegister(TTE_REG_ADDR);
	#ifdef I2C_DEBUG
		max_debugFuelGauge();
	#endif
} 


/***********************************************************
 *
 * Check bit 6 of cycles register. Data sheet recommends
 * saving learned params every time Cycles.B6 toggles
 *
 * @param max : Max17263 data struct pointer
 *
 * @returns	  : boolean check for toggle bit
 *
 ***********************************************************/
uint8_t max_checkCycles(void) {
	
	uint16_t buffer = max_readRegister(Cycles_REG_ADDR);	// read cycles register
	
	// compare bit 6 of reading to last saved value 
	if((buffer & Cycles_BIT6) != (max17263.Cycles & Cycles_BIT6)) {
		max17263.Cycles = buffer;
		return 1;	// return 1 when toggled
	}
	return 0;
}


/***********************************************************
 *
 * Save learned parameters into data structure
 *
 * @param max : Max17263 data structured
 *
 ***********************************************************/
void max_saveLearnedParameters(void) {
	max17263.RCOMP	 = max_readRegister(RCOMP0_REG_ADDR);
	max17263.TempCo	 = max_readRegister(TempCo_REG_ADDR);
	max17263.FullCapRep = max_readRegister(FullCapRep_REG_ADDR);
	max17263.Cycles	    = max_readRegister(Cycles_REG_ADDR);
	max17263.FullCapNom = max_readRegister(FullCapNom_REG_ADDR);
	max_eepromSaveParameters();
}


/***********************************************************
 *
 * Save learned parameters to non volatile data
 * eeprom_update_word() only writes if data mismatch
 *
 * @param max : Max17263 data structured
 *
 ***********************************************************/
void max_eepromSaveParameters(void) {
	eeprom_update_word((uint16_t *)EEPROM_RCOMP0_ADDR, max17263.RCOMP);
	eeprom_update_word((uint16_t *)EEPROM_TempCo_ADDR, max17263.TempCo);
	eeprom_update_word((uint16_t *)EEPROM_FullCapRep_ADDR, max17263.FullCapRep);
	eeprom_update_word((uint16_t *)EEPROM_Cycles_ADDR, max17263.Cycles);
	eeprom_update_word((uint16_t *)EEPROM_FullCapNom_ADDR, max17263.FullCapNom);
	#ifdef I2C_DEBUG
		max_debugEEPROM();
	#endif
}


/***********************************************************
 *
 * Load learned parameters from non volatile data
 *
 * @param max : Max17263 data structured
 *
 ***********************************************************/
void max_eepromLoadParameters(void) {
	max17263.RCOMP  = eeprom_read_word((uint16_t *)EEPROM_RCOMP0_ADDR);
	max17263.TempCo = eeprom_read_word((uint16_t *)EEPROM_TempCo_ADDR);
	max17263.FullCapRep = eeprom_read_word((uint16_t *)EEPROM_FullCapRep_ADDR);
	max17263.Cycles     = eeprom_read_word((uint16_t *)EEPROM_Cycles_ADDR);
	max17263.FullCapNom = eeprom_read_word((uint16_t *)EEPROM_FullCapNom_ADDR);
	#ifdef I2C_DEBUG
		max_debugEEPROM();
	#endif
}


/***********************************************************
 *
 * Helps with debugging. Reads data at register address
 * and transmits that data too a desired receiver
 *
 * @ param addr : i2c address for receiver
 * @ param reg  : register address to be read
 *
 ***********************************************************/
void max_debugRead(uint8_t addr, uint8_t reg) {
	uint16_t rx_data;
	uint8_t tx_buffer[2];
	rx_data = max_readRegister(reg);
	tx_buffer[0] = (uint8_t)((rx_data & 0x00FF));
	tx_buffer[1] = (uint8_t)((rx_data >> 8) & 0x00FF);
	i2c_controller_transmit(addr, tx_buffer, 2, I2C_NO_REPEAT);
}


/***********************************************************
 *
 * Transmits data to a desired receiver
 *
 * @ param addr : i2c address for receiver
 * @ param data : data to send
 *
 ***********************************************************/
void max_debugWrite(uint8_t addr, uint16_t data) {
	uint8_t buffer[2];
	buffer[0] = (uint8_t)((data & 0x00FF));
	buffer[1] = (uint8_t)((data >> 8) & 0x00FF);
	i2c_controller_transmit(addr, buffer, 2, I2C_NO_REPEAT);
}


/***********************************************************
 *
 * Transmits data and code to a desired receiver. 
 * Code helps receiver parse and interpret data
 *
 * @ param addr : i2c address for receiver
 * @ param code : code for receiver to parse
 * @ param data : data to send
 *
 ***********************************************************/
void max_debugWriteCode(uint8_t addr, uint16_t code, uint16_t data) {
	uint8_t buffer[4];
	buffer[0] = (uint8_t)((code & 0x00FF));
	buffer[1] = (uint8_t)((code >> 8) & 0x00FF);
	buffer[2] = (uint8_t)((data & 0x00FF));
	buffer[3] = (uint8_t)((data >> 8) & 0x00FF);
	i2c_controller_transmit(addr, buffer, 4, I2C_NO_REPEAT);
}


/***********************************************************
 *
 * Transmits data in Max17263_t data struct to receiver.
 * Saves struct values to 26 byte buffer
 * Sent as single data stream to receiver with parse code
 *
 * @param max  : Max17263 data struct pointer
 *
 ***********************************************************/
void max_debugDataStruct(void) {
	
	uint8_t buffer[26];
	uint16_t data[] = {
		DEBUG_STRUCT_CODE,
		max17263.DesignCap.value, max17263.IChgTerm.value, max17263.VEmpty.value, max17263.ModelCfg.value,
		max17263.RepCap, max17263.RepSOC, max17263.TTE, max17263.RCOMP,
		max17263.TempCo, max17263.FullCapRep, max17263.Cycles, max17263.FullCapNom
	};
  
	for(uint8_t i = 0; i < 22; i+=2) {
		buffer[i] = (uint8_t)(data[i/2] & 0x00FF);
		buffer[i+1] = (uint8_t)((data[i/2] >> 8) & 0x00FF);
	}
  
	i2c_controller_transmit(DEBUG_ADDR, buffer, 26, I2C_NO_REPEAT);
}


/***********************************************************
 *
 * Transmit fuel gauge readings to desired receiver
 * Reads registers directly into 8 byte buffer
 * Sent as single data stream to receiver with parse code
 *
 ***********************************************************/
void max_debugFuelGauge(void) {
	  	
	uint8_t buffer[8];
	uint16_t data[4] = {
		DEBUG_FUEL_GAUGE_CODE, max17263.RepCap, 
		max17263.RepSOC, max17263.TTE
	};
  
	for(uint8_t i = 0; i < 8; i+=2) {
	buffer[i] = (uint8_t)(data[i/2] & 0x00FF);
	buffer[i+1] = (uint8_t)((data[i/2] >> 8) & 0x00FF);
	}
  
	i2c_controller_transmit(DEBUG_ADDR, buffer, 8, I2C_NO_REPEAT);
}


/***********************************************************
 *
 * Transmit saved parameters in EEPROM to receiver
 * EEPROM data stored with addr locations in 22 byte buffer
 * Sent as single data stream to receiver with parse code
 * Address + data byte order:
 * 1 -> addr LSB
 * 2 -> addr MSB
 * 3 -> data LSB
 * 4 -> data MSB
 *
 ***********************************************************/
void max_debugEEPROM(void) {

	uint8_t buffer[22];
	uint16_t data[] = {
		DEBUG_EEPROM_CODE,
		EEPROM_RCOMP0_ADDR, eeprom_read_word((uint16_t *) EEPROM_RCOMP0_ADDR),
		EEPROM_TempCo_ADDR, eeprom_read_word((uint16_t *) EEPROM_TempCo_ADDR),
		EEPROM_FullCapRep_ADDR, eeprom_read_word((uint16_t *) EEPROM_FullCapRep_ADDR),
		EEPROM_Cycles_ADDR, eeprom_read_word((uint16_t *) EEPROM_Cycles_ADDR),
		EEPROM_FullCapNom_ADDR, eeprom_read_word((uint16_t *) EEPROM_FullCapNom_ADDR)
	};
	
	for(uint8_t i = 0; i < 22; i+=2) {
		buffer[i] = (uint8_t)(data[i/2] & 0x00FF);
		buffer[i+1] = (uint8_t)((data[i/2] >> 8) & 0x00FF);
	}
	
	i2c_controller_transmit(DEBUG_ADDR, buffer, 22, I2C_NO_REPEAT);
}


/***********************************************************
 *
 * Helps with debugging. Initiates custom LED control and
 * cycles through LEDs 1-4
 *
 ***********************************************************/
void max_debugLED(void) {
	
	// enable custom led 
	max17263.LEDCfg3.bit.CustLEDCtrl = 1;
	max_writeRegister(LEDCfg3_REG_ADDR, max17263.LEDCfg3.value);
	
	// toggle LEDs 1-4 
	for (uint8_t i = 0; i < 4; i++) {
		max_writeRegister(CustLED_REG_ADDR, (uint16_t)(1 << i));
		_delay_ms(1000);
	}
	
	// disable custom led control
	max17263.LEDCfg3.value = LEDCfg3_DEFAULT;
	max_writeRegister(CustLED_REG_ADDR, CustLED_DEFAULT);
	max_writeRegister(CustLED_REG_ADDR, max17263.LEDCfg3.value);
	
	_delay_ms(1000);
}