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
#include "util/delay.h"
#include <util/delay.h>
#include "serial/i2c.h"
#include "max/max17263.h"


#define BLINK_DELAY 250
#define DEBUG_ADDR	0x69
#define BUFFER_SIZE	4

void fill_buffer(uint8_t *buff, uint8_t len, uint32_t data) {
	if (len > 2) {
		for (int8_t i = len-1; i > -1; i--) {
			uint32_t temp = data;
			buff[len-i-1] = ((temp >> (i*8)) & 0x000000FF);
		}
	}
	else {
		buff[0] = data;
	}
}


int main(void){
	
	uint8_t buffer[BUFFER_SIZE];
	uint8_t err[12];
	
	// init D3 output, high
	// controls LED 5
	PORTD |= (1 << PORTD3);
	DDRD  |= (1 << PORTD3);
	
	// disable pullups SDA, SCL
	DDRD  |= ((1 << PORTD0) | (1 << PORTD1)); 
	PORTD &= ~((1 << PORTD0) | (1 << PORTD1));
	DDRD  &= ~((1 << PORTD0) | (1 << PORTD1));

	i2c_init(F_CPU, I2C_SCL_400KHZ);
	
	while(1){
		
		buffer[0]= REG_LEDCFG3;
		i2c_controller_transmit(MAX17263_ADDR, buffer, 1, I2C_REPEAT);
		i2c_controller_receive(MAX17263_ADDR, buffer, 2);
		i2c_controller_transmit(DEBUG_ADDR, buffer, 2, I2C_NO_REPEAT);	
		
		buffer[0] = REG_LEDCFG3;	
		buffer[1] = 0;
		buffer[2] = 0b01100000;
		i2c_controller_transmit(MAX17263_ADDR, buffer, 3, I2C_NO_REPEAT);
		
		buffer[0]= REG_LEDCFG3;
		i2c_controller_transmit(MAX17263_ADDR, buffer, 1, I2C_REPEAT);
		i2c_controller_receive(MAX17263_ADDR, buffer, 2);
		i2c_controller_transmit(DEBUG_ADDR, buffer, 2, I2C_NO_REPEAT);
		
		
		for (uint8_t i = 0; i < 4; i++) {
			buffer[0] = REG_CUSTLED;
			buffer[1] = (1 << i);
			buffer[2] = 0;
			i2c_controller_transmit(MAX17263_ADDR, buffer, 3, I2C_NO_REPEAT);
			
			buffer[0] = REG_CUSTLED;
			i2c_controller_transmit(MAX17263_ADDR, buffer, 1, I2C_REPEAT);
			i2c_controller_receive(MAX17263_ADDR, buffer, 2);
			i2c_controller_transmit(DEBUG_ADDR, buffer, 2, I2C_NO_REPEAT);
			
			_delay_ms(1000);
		}
		
		fill_buffer(buffer, 4, 0xAABBCCDD);
		i2c_controller_transmit(DEBUG_ADDR, buffer, 4, I2C_NO_REPEAT);
		
		
		_delay_ms(100);
		
		
    }
}