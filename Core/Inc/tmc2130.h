/*
 * motorcontrol.h
 *
 *  Created on: Jan 25, 2019
 *      Author: esoren
 */

#ifndef INC_TMC2130_H_
#define INC_TMC2130_H_

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

/*general configuration registers*/
#define TMC_REG_GCONF 			0x0
#define TMC_REG_GSTAT 			0x1
#define TMC_REG_IOIN 			0x4

/*velocity dependent driver feature control registers*/
#define TMC_REG_IHOLD_IRUN  	0x10
#define TMC_REG_TPOWERDOWN  	0x11
#define TMC_REG_TSTEP  			0x12
#define TMC_REG_TPWMTHRS  		0x13
#define TMC_REG_TCOOLTHRS  		0x14
#define TMC_REG_THIGH  			0x15

/*spi mode registers*/
#define TMC_REG_XDIRECT 		0x2D

/*dc-step minimum velocity register*/
#define TMC_REG_VDCMIN  		0x33

/*motor driver registers*/
#define TMC_REG_MSLUT0  		0x60
#define TMC_REG_MSLUT1  		0x61
#define TMC_REG_MSLUT2  		0x62
#define TMC_REG_MSLUT3  		0x63
#define TMC_REG_MSLUT4  		0x64
#define TMC_REG_MSLUT5  		0x65
#define TMC_REG_MSLUT6  		0x66
#define TMC_REG_MSLUT7  		0x67
#define TMC_REG_MSLUTSEL  		0x68
#define TMC_REG_MSLUTSTART  	0x69
#define TMC_REG_MSCNT  			0x6A
#define TMC_REG_MSCURACT  		0X6B
#define TMC_REG_CHOPCONF  		0x6C
#define TMC_REG_COOLCONF  		0x6D
#define TMC_REG_DCCTRL 			0x6E
#define TMC_REG_DRV_STATUS  	0x6F
#define TMC_REG_PWM_CONF  		0x70
#define TMC_REG_PWM_SCALE  		0x71
#define TMC_REG_ENCM_CTRL  		0x72
#define TMC_REG_LOST_STEPS  	0x73

/*FUNCTION PROTOTYPES*/
uint32_t tmc_readwrite_register(uint8_t addr, uint32_t data, uint8_t set_write_flag);
void tmc_init(void);


#endif /* INC_TMC2130_H_ */
