/*
 * i2c.c
 *
 * Created: 8/8/2023 1:49:01 PM
 *  Author: Ellis Hobby
 */ 

#include "i2c.h"

/*
	fscl = fcpu / (16 + 2(TWBR) * 4^TWPS)
*/
void i2c_init(uint32_t fcpu, uint32_t fscl) {
	TWSR = 0x00;
	TWBR = (((fcpu/fscl) - 16) / 2);
}


void i2c_disable(void) {
	TWCR &= ~(1 << TWEN);
}


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


uint8_t i2c_address(uint8_t addr, uint8_t rw) {
	
	// load address + r/w, start transmission
	TWDR = (addr << 1) | rw;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	
	// check status reg for ACK
	if ((TW_STATUS != TW_MT_SLA_ACK) && (TW_STATUS != TW_MR_SLA_ACK)) {
		return TW_STATUS;
	}
	
	return 0;
	
}


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


uint8_t i2c_readACK(void) {
		
	// read with ACK
	TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWEA);
	while (!(TWCR & (1 << TWINT)));
		
	// check if ACK sent
	if (TW_STATUS!= TW_MR_DATA_ACK) {
		return TW_STATUS;
	}
		
	// return data on success
	return TWDR;
}


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


void i2c_stop (void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while(TWCR & (1 << TWSTO));
}


uint8_t i2c_controller_transmit(uint8_t addr, uint8_t* data, uint8_t len, uint8_t repeat) {
	
	uint8_t err;
	
	err = i2c_start(addr, TW_WRITE);
	if (err != 0) {
		return err;
	}
	
	/*
	err = i2c_address(addr, TW_WRITE);
	if (err != 0) {
		return err;
	}
	*/
	
	if (len > 1) {
		for (uint8_t i = 0; i < len; i++) {
			err = i2c_write(data[i]);
			/*if (err != 0) {
				return err;
			}*/
		}
	}
	
	else {
		err = i2c_write(data[0]);
		/*if (err != 0) {
			return err;
		}*/
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


uint32_t i2c_controller_receive(uint8_t addr, uint8_t* data, uint8_t len) {
	
	uint8_t err;

	err = i2c_start(addr, TW_READ);
	if (err != 0) {
		return err;
	}
	
	/*
	err = i2c_address(addr, TW_READ);
	if (err != 0) {
		return err;
	}
	*/
	
	for (uint8_t i = 0; i < len - 1; i++) {
		data[i] = i2c_readACK();
	}
	
	data[len-1] = i2c_readNACK();
	
	i2c_stop();
	
	return 0;
}