/*
 * max17263.c
 *
 * Created: 8/11/2023 12:43:03 PM
 *  Author: Ellis Hobby
 */ 


#include "max17263.h"


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
	i2c_controller_transmit(MAX17263_ADDR, rx_buffer, 1, I2C_REPEAT);
	i2c_controller_receive(MAX17263_ADDR, rx_buffer, 2);
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
	i2c_controller_transmit(MAX17263_ADDR, tx_buffer, 3, I2C_NO_REPEAT);
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
void max_debugDataStruct(Max17263_t* max) {
	
	uint8_t buffer[26];
	
	buffer[0] = (uint8_t)(DEBUG_STRUCT_CODE & 0x00FF);
	buffer[1] = (uint8_t)((DEBUG_STRUCT_CODE >> 8) & 0x00FF);
	
	buffer[2] = (uint8_t)((max->config.designcap) & 0x00FF);
	buffer[3] = (uint8_t)((max->config.designcap >> 8) & 0x00FF);
	
	buffer[4] = (uint8_t)((max->config.ichgterm) & 0x00FF);
	buffer[5] = (uint8_t)((max->config.ichgterm >> 8) & 0x00FF);
	
	buffer[6] = (uint8_t)((max->config.vempty) & 0x00FF);
	buffer[7] = (uint8_t)((max->config.vempty >> 8) & 0x00FF);
	
	buffer[8] = (uint8_t)((max->config.modelcfg) & 0x00FF);
	buffer[9] = (uint8_t)((max->config.modelcfg >> 8) & 0x00FF);
	
	buffer[10] = (uint8_t)((max->gauge.repcap) & 0x00FF);
	buffer[11] = (uint8_t)((max->gauge.repcap >> 8) & 0x00FF);
	
	buffer[12] = (uint8_t)((max->gauge.repsoc) & 0x00FF);
	buffer[13] = (uint8_t)((max->gauge.repsoc >> 8) & 0x00FF);
	
	buffer[14] = (uint8_t)((max->gauge.tte) & 0x00FF);
	buffer[15] = (uint8_t)((max->gauge.tte >> 8) & 0x00FF);
	
	buffer[16] = (uint8_t)((max->learned.rcomp0) & 0x00FF);
	buffer[17] = (uint8_t)((max->learned.rcomp0 >> 8) & 0x00FF);
	
	buffer[18] = (uint8_t)((max->learned.tempco) & 0x00FF);
	buffer[19] = (uint8_t)((max->learned.tempco >> 8) & 0x00FF);
	
	buffer[20] = (uint8_t)((max->learned.fullcaprep) & 0x00FF);
	buffer[21] = (uint8_t)((max->learned.fullcaprep >> 8) & 0x00FF);
	
	buffer[22] = (uint8_t)((max->learned.cycles) & 0x00FF);
	buffer[23] = (uint8_t)((max->learned.cycles >> 8) & 0x00FF);
	
	buffer[24] = (uint8_t)((max->learned.fullcapnom) & 0x00FF);
	buffer[25] = (uint8_t)((max->learned.fullcapnom >> 8) & 0x00FF);
	
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
	
	uint16_t data;
	uint8_t buffer[8];
	
	buffer[0] = (uint8_t)(DEBUG_FUEL_GAUGE_CODE & 0x00FF);
	buffer[1] = (uint8_t)((DEBUG_FUEL_GAUGE_CODE >> 8) & 0x00FF);
	
	data = max_readRegister(RepCap_ADDR);
	buffer[2] = (uint8_t)(data & 0x00FF);
	buffer[3] = (uint8_t)((data >> 8) & 0x00FF);
	
	data = max_readRegister(RepSOC_ADDR);
	buffer[4] = (uint8_t)(data & 0x00FF);
	buffer[5] = (uint8_t)((data >> 8) & 0x00FF);
	
	data = max_readRegister(TTE_ADDR);
	buffer[6] = (uint8_t)(data & 0x00FF);
	buffer[7] = (uint8_t)((data >> 8) & 0x00FF);
	
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
	
	uint16_t data;
	uint8_t buffer[22];
	
	buffer[0] = (uint8_t)(DEBUG_EEPROM_CODE & 0x00FF);
	buffer[1] = (uint8_t)((DEBUG_EEPROM_CODE >> 8) & 0x00FF);
	
	data = eeprom_read_word((uint16_t *) EEPROM_RCOMP0_ADDR);
	buffer[2] = (uint8_t)(EEPROM_RCOMP0_ADDR & 0x00FF);
	buffer[3] = (uint8_t)((EEPROM_RCOMP0_ADDR >> 8) & 0x00FF);
	buffer[4] = (uint8_t)(data & 0x00FF);
	buffer[5] = (uint8_t)((data >> 8) & 0x00FF);

	
	data = eeprom_read_word((uint16_t *) EEPROM_TempCo_ADDR);
	buffer[6] = (uint8_t)(EEPROM_TempCo_ADDR & 0x00FF);
	buffer[7] = (uint8_t)((EEPROM_TempCo_ADDR >> 8) & 0x00FF);
	buffer[8] = (uint8_t)(data & 0x00FF);
	buffer[9] = (uint8_t)((data >> 8) & 0x00FF);

	
	data = eeprom_read_word((uint16_t *) EEPROM_FullCapRep_ADDR);
	buffer[10] = (uint8_t)(EEPROM_FullCapRep_ADDR & 0x00FF);
	buffer[11] = (uint8_t)((EEPROM_FullCapRep_ADDR >> 8) & 0x00FF);
	buffer[12] = (uint8_t)(data & 0x00FF);
	buffer[13] = (uint8_t)((data >> 8) & 0x00FF);
	
	data = eeprom_read_word((uint16_t *) EEPROM_Cycles_ADDR);
	buffer[14] = (uint8_t)(EEPROM_Cycles_ADDR & 0x00FF);
	buffer[15] = (uint8_t)((EEPROM_Cycles_ADDR >> 8) & 0x00FF);
	buffer[16] = (uint8_t)(data & 0x00FF);
	buffer[17] = (uint8_t)((data >> 8) & 0x00FF);
	
	data = eeprom_read_word((uint16_t *) EEPROM_FullCapNom_ADDR);
	buffer[18] = (uint8_t)(EEPROM_FullCapNom_ADDR & 0x00FF);
	buffer[19] = (uint8_t)((EEPROM_FullCapNom_ADDR >> 8) & 0x00FF);
	buffer[20] = (uint8_t)(data & 0x00FF);
	buffer[21] = (uint8_t)((data >> 8) & 0x00FF);
	
	i2c_controller_transmit(DEBUG_ADDR, buffer, 22, I2C_NO_REPEAT);
}


/***********************************************************
 *
 * Helps with debugging. Initiates custom LED control and
 * cycles through LEDs 1-4
 *
 ***********************************************************/
void max_debugLED(void) {
	
	// enable custom led control
	max_writeRegister(LEDCfg3_ADDR, CUSTLEDCTRL);
	
	// toggle LEDs 1-4 
	for (uint8_t i = 0; i < 4; i++) {
		max_writeRegister(CustLED_ADDR, (uint16_t)(1 << i));
		_delay_ms(1000);
	}
	
	// disable custom led control
	max_writeRegister(CustLED_ADDR, CustLED_DEF);
	max_writeRegister(LEDCfg3_ADDR, LEDCfg3_DEF);
	
	_delay_ms(1000);
}


/***********************************************************
 *
 * Check POR bit in STATUS Register
 *
 * @returns : POR bit state
 *
 ***********************************************************/
uint16_t max_checkPOR(void) {
	return (max_readRegister(Status_ADDR) & POR);	// return state of por bit in status reg
}


/***********************************************************
 *
 * Ez Config steps described in MAX17263 user guide
 *
 * @param max : Max17263 data struct pointer
 *
 ***********************************************************/
void max_loadConfig(Max17263_t* max) {
	
	uint16_t buffer;
	uint8_t check_flag = 0;
	
	if (DEBUG)
		max_debugWrite(DEBUG_ADDR, DEBUG_POR_CODE);
	
	// check to see if saved parameters exits
	// (should be value 0xBEEF at address 0x00)
	buffer = eeprom_read_word((uint16_t*)EEPROM_CHECK_ADDR);
	
	// if no saved parameters initialize eeprom and flag
	if (buffer != EEPROM_CHECK_VAL) {
		check_flag = 1;
	}
	
	while(max_readRegister(FStat_ADDR) & DNR) {				// wait until FSTAT.DNR bit = 0 (warming up)
		_delay_ms(10);
	}
	
	buffer = max_readRegister(HibCFG_ADDR);					// save original hibernate mode settings
	max_writeRegister(SoftWakeup_ADDR, SoftWakeup_CLEAR);	// exit hibernate mode step 1 
	max_writeRegister(HibCFG_ADDR, 0x0000);					// exit hibernate mode step 2 
	max_writeRegister(SoftWakeup_ADDR, SoftWakeup_SOFT);	// exit hibernate mode step 3 
	
	// configuration
	max_writeRegister(DesignCap_ADDR, max->config.designcap);	// write DesignCap
	max_writeRegister(IchgTerm_ADDR, max->config.ichgterm);		// write IChgTerm
	max_writeRegister(Vempty_ADDR, max->config.vempty);			// write Vempty
	max_writeRegister(ModelCFG_ADDR, max->config.modelcfg);		// write ModelCfg
	
	while(max_readRegister(ModelCFG_ADDR) & REFRESH) {			// wait until MODELCFG.REFRESH = 0
		_delay_ms(10);
	}
	
	// if no learned parameters in eeprom we need default
	if (check_flag) {
		max->learned.rcomp0 = max_readRegister(RCOMP0_ADDR);
		max->learned.tempco = max_readRegister(TempCo_ADDR);
		max->learned.fullcaprep = max->config.designcap;
		max->learned.cycles = 0;
		max->learned.fullcapnom = max->config.designcap;
		max_eepromSaveParameters(max);
	}
	
	// otherwise we can load the learned parameters from eeprom
	else {
		max_eepromLoadParameters(max);
	}
	
	// load learned parameters
	max_writeRegister(RCOMP0_ADDR, max->learned.rcomp0);
	max_writeRegister(TempCo_ADDR, max->learned.tempco);
	max_writeRegister(FullCapRep_ADDR, max->learned.fullcaprep);
	max_writeRegister(Cycles_ADDR, max->learned.cycles);
	max_writeRegister(FullCapNom_ADDR, max->learned.fullcapnom);
	
	max_writeRegister(HibCFG_ADDR, buffer);						// restore original hibernate settings
	
	max_initLED();
	
	buffer = max_readRegister(Status_ADDR);						// read status
	max_writeAndVerifyRegister(Status_ADDR, (buffer & ~POR));	// clear por bit
}


void max_initLED(void) {
	max_writeRegister(LEDCfg1_ADDR, 0x6074);
	
	max_setLEDBars(4);
	max_setLEDChargeIndicator(1);
	max_setLEDGrayScale(1);
	max_setLEDAniMode(2);
	max_setLEDAniStep(7);
	max_setLEDTimer(7);
	max_setLEDMode(3)
	
	max_writeRegister(LEDCfg2_ADDR, 0x011f);
	max_writeRegister(LEDCfg3_ADDR, LEDCfg3_DEF);
}


void max_setLEDBars(uint8_t bars) {
	uint16_t buffer = max_readRegister(LEDCfg1_ADDR) & NBARS_MASK;
	buffer |= (bars & 0x0F);
	max_writeRegister(LEDCfg1_ADDR, buffer);
}


void max_setLEDGrayScale(uint8_t state) {
	uint16_t buffer = max_readRegister(LEDCfg1_ADDR);
	if (state >= 1) 
		buffer |= GrEn;
	else 
		buffer &= ~GrEn;
	max_writeRegister(LEDCfg1_ADDR, buffer);
}


void max_setLEDChargeIndicator(uint8_t lchg) {
	uint16_t buffer = max_readRegister(LEDCfg1_ADDR);
	if (lchg >= 1) 
		buffer |= LChg;
	else 
		buffer &= ~LChg;
	max_writeRegister(LEDCfg1_ADDR, buffer);
}


void max_setLEDMode(uint8_t md) {
	uint16_t buffer = max_readRegister(LEDCfg1_ADDR);
	if (md > 0x03)
		md = 0x03;
	buffer |= (md << 6);
	max_writeRegister(LEDCfg1_ADDR, buffer);
}


void max_setLEDAniMode(uint8_t md) {
	uint16_t buffer = max_readRegister(LEDCfg1_ADDR);
	if (md > 0x03)
		md = 0x03;
	buffer |= (md << 8);
	max_writeRegister(LEDCfg1_ADDR, buffer);
}


void max_setLEDAniStep(uint8_t step) {
	uint16_t buffer = max_readRegister(LEDCfg1_ADDR);
	if (step > 0x07)
		step = 0x07;
	buffer |= (step << 10);
	max_writeRegister(LEDCfg1_ADDR, buffer);
}


void max_setLEDTimer(uint8_t time) {
	uint16_t buffer = max_readRegister(LEDCfg1_ADDR);
	if (time > 0x07)
		time = 0x07;
	buffer |= (time << 13);
	max_writeRegister(LEDCfg1_ADDR, buffer);
}


/***********************************************************
 *
 * Read fuel gauge parameters, save into data structure
 *
 * @param max : Max17263 data struct pointer
 *
 ***********************************************************/
void max_readFuelGauge(Max17263_t* max) {
	max->gauge.repcap	= max_readRegister(RepCap_ADDR);
	max->gauge.repsoc	= max_readRegister(RepSOC_ADDR);
	max->gauge.tte		= max_readRegister(TTE_ADDR);
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
uint8_t max_checkCycles(Max17263_t* max) {
	
	uint16_t buffer = max_readRegister(Cycles_ADDR);	// read cycles register
	
	if (DEBUG) {
		max_debugRead(DEBUG_ADDR, Cycles_ADDR);
	}
	
	// compare bit 6 of reading to last saved value 
	if((buffer & Cycles_B6) != (max->learned.cycles & Cycles_B6)) {
		max->learned.cycles = buffer;
		return 1;	// return 1 when toggled
	}
	else {
		max->learned.cycles = buffer;
		return 0;
	}
}


/***********************************************************
 *
 * Save learned parameters into data structure
 *
 * @param max : Max17263 data structured
 *
 ***********************************************************/
void max_saveLearnedParameters(Max17263_t* max) {
	max->learned.rcomp0	 = max_readRegister(RCOMP0_ADDR);
	max->learned.tempco	 = max_readRegister(TempCo_ADDR);
	max->learned.fullcaprep = max_readRegister(FullCapRep_ADDR);
	max->learned.cycles	 = max_readRegister(Cycles_ADDR);
	max->learned.fullcapnom = max_readRegister(FullCapNom_ADDR);
	max_eepromSaveParameters(max);
}


/***********************************************************
 *
 * Save learned parameters to non volatile data
 * eeprom_update_word() only writes if data mismatch
 *
 * @param max : Max17263 data structured
 *
 ***********************************************************/
void max_eepromSaveParameters(Max17263_t* max) {
	eeprom_update_word((uint16_t *)EEPROM_RCOMP0_ADDR, max->learned.rcomp0);
	eeprom_update_word((uint16_t *)EEPROM_TempCo_ADDR, max->learned.tempco);
	eeprom_update_word((uint16_t *)EEPROM_FullCapRep_ADDR, max->learned.fullcaprep);
	eeprom_update_word((uint16_t *)EEPROM_Cycles_ADDR, max->learned.cycles);
	eeprom_update_word((uint16_t *)EEPROM_FullCapNom_ADDR, max->learned.fullcapnom);
}


/***********************************************************
 *
 * Load learned parameters from non volatile data
 *
 * @param max : Max17263 data structured
 *
 ***********************************************************/
void max_eepromLoadParameters(Max17263_t* max) {
	max->learned.rcomp0  = eeprom_read_word((uint16_t *)EEPROM_RCOMP0_ADDR);
	max->learned.tempco = eeprom_read_word((uint16_t *)EEPROM_TempCo_ADDR);
	max->learned.fullcaprep  = eeprom_read_word((uint16_t *)EEPROM_FullCapRep_ADDR);
	max->learned.cycles = eeprom_read_word((uint16_t *)EEPROM_Cycles_ADDR);
	max->learned.fullcapnom = eeprom_read_word((uint16_t *)EEPROM_FullCapNom_ADDR);
}
