/*
 * max17263.h
 *
 * Created: 8/8/2023 3:09:26 PM
 *  Author: Ellis Hobby
 */ 


#ifndef MAX17263_H_
#define MAX17263_H_




#define REG_LEDCFG3		0x37
#define CUSTLEDCTRL		13

#define REG_CUSTLED		0x64
#define LED0	0
#define LED1	1
#define LED2	2
#define LED3	3

const uint8_t MAX17263_ADDR	= 0x36;

#endif /* MAX17263_H_ */