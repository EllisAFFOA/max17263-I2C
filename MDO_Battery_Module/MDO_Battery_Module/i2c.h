/*
 * i2c.h
 *
 * Created: 8/8/2023 1:49:19 PM
 *  Author: Ellis Hobby
 */ 


#ifndef I2C_H_
#define I2C_H_

#include "stdbool.h"
#include "util/twi.h"

#define I2C_SCL_400KHZ	400000UL
#define I2C_SCL_100KHZ	100000UL

#define I2C_REPEAT		true
#define I2C_NO_REPEAT	false

void i2c_init(uint32_t fcpu, uint32_t fscl);
void i2c_disable(void);
uint8_t i2c_start(uint8_t addr, uint8_t dir);
uint8_t i2c_write (uint8_t data);
uint8_t i2c_readACK(void);
uint8_t i2c_readNACK(void);
void i2c_stop(void);
uint8_t i2c_controller_transmit(uint8_t addr, uint8_t* data, uint8_t len, bool repeat);
uint32_t i2c_controller_receive(uint8_t addr, uint8_t* data, uint8_t len);

#endif /* I2C_H_ */