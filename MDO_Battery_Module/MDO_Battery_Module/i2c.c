/*
 * i2c.c
 *
 * Created: 8/8/2023 1:49:01 PM
 *  Author: Ellis Hobby
 */ 

#include "i2c.h"


/***********************************************************
 *
 * Initiate I2C peripheral
 * fscl = fcpu / (16 + 2(TWBR) * 4^TWPS)
 *
 * @param fcpu : CPU clock speed
 * @param fscl : Desired I2C clock speed
 *
 ***********************************************************/
void i2c_init(uint32_t fcpu, uint32_t fscl) {
	TWSR = 0x00;
	TWBR = (((fcpu/fscl) - 16) / 2);
}


/***********************************************************
 *
 * Disable I2C peripheral
 *
 ***********************************************************/
void i2c_disable(void) {
	TWCR &= ~(1 << TWEN);
}


/***********************************************************
 *
 * Send start condition and address target for read/write
 *
 * @param addr : target device address
 * @param dir  : r/w bit
 *
 ***********************************************************/
uint8_t i2c_start(uint8_t addr, uint8_t dir) {
	
	// send START condition, wait for complete
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
  
	// check status reg
	if ((TW_STATUS != TW_START) && (TW_STATUS != TW_REP_START)) {
		return TW_STATUS;
	}

	// load address + r/w, start transmission
	TWDR = (addr << 1) | dir;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	// check status reg for ACK
	if ((TW_STATUS != TW_MT_SLA_ACK) && (TW_STATUS != TW_MR_SLA_ACK)) {
		return TW_STATUS;
	}
	return 0;
}


/***********************************************************
 *
 * Write data to I2C target device
 *
 * @param data : byte to transmit
 *
 ***********************************************************/
uint8_t i2c_write (uint8_t data) {

	// load data, start transmission
	TWDR = data;
	TWCR = (1 << TWINT)|(1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
		
	// check status reg for ACK
	if (TW_STATUS != TW_MT_DATA_ACK) {
		return TW_STATUS;
	}
	return 0; 	
}


/***********************************************************
 *
 * Read target device with ACK
 *
 ***********************************************************/
uint8_t i2c_readACK(void) {

	// read with ACK
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));
		
	// check if ACK sent
	if (TW_STATUS!= TW_MR_DATA_ACK) {;
		return TW_STATUS;
	}
	// return data on success
	return TWDR;
}


/***********************************************************
 *
 * Read target device with NACK
 *
 ***********************************************************/
uint8_t i2c_readNACK (void) {

	// enable send NACK to slave
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (! (TWCR & (1 << TWINT)));
		
	// check if NACK sent
	if (TW_STATUS != TW_MR_DATA_NACK) {
		return TW_STATUS;
	}
	// return data on success
	return TWDR;
}


/***********************************************************
 *
 * End I2C transaction
 *
 ***********************************************************/
void i2c_stop (void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while(TWCR & (1 << TWSTO));
}


/***********************************************************
 *
 * Transmit data packet to I2C target
 *
 * @param addr	 : target device address
 * @param data	 : packet byte array 
 * @param len	 : packet length 
 * @param repeat : leave connection open = 1 
 *
 ***********************************************************/
uint8_t i2c_controller_transmit(uint8_t addr, uint8_t* data, uint8_t len, uint8_t repeat) {
	
	uint8_t err;
	
	err = i2c_start(addr, TW_WRITE);
	if (err != 0) {
		return err;
	}
	
	if (len > 1) {
		for (uint8_t i = 0; i < len; i++) {
			err = i2c_write(data[i]);
		}
	}
	
	else {
		err = i2c_write(data[0]);
	}
	
	if (err != 0) {
		return err;
	}
	
	if (!repeat) {
		i2c_stop();
		return 0;
	}
	
	return 0;
}


/***********************************************************
 *
 * Read data packet from I2C target
 *
 * @param addr	 : target device address
 * @param data	 : packet buffer 
 * @param len	 : packet length 
 *
 ***********************************************************/
uint32_t i2c_controller_receive(uint8_t addr, uint8_t* data, uint8_t len) {
	
	uint8_t err;

	err = i2c_start(addr, TW_READ);
	if (err != 0) {
		return err;
	}
	
	for (uint8_t i = 0; i < len - 1; i++) {
		data[i] = i2c_readACK();
	}
	
	data[len-1] = i2c_readNACK();
	
	i2c_stop();
	
	return 0;
}