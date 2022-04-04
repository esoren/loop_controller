/*
 * tmc2130.c
 *
 *  Created on: Jan 28, 2019
 *      Author: esoren
 */

#include "tmc2130.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "spi.h"
#include "gpio.h"


uint32_t tmc_readwrite_register(uint8_t addr, uint32_t data, uint8_t set_write_flag) {

	uint8_t rx_data[5];
	uint8_t tx_data[5];
	uint32_t payload =0;
	uint8_t send_count = 2;


	if(set_write_flag) {
		addr |= 0x80;
		send_count = 1;
	}

	//send twice according to TMC2130 datasheet
	for (uint8_t i = 0; i < send_count; i++) {

		tx_data[0] = addr;
		tx_data[1] = (data >> 24) & 0x00ff;
		tx_data[2] = (data >> 16) & 0x00ff;
		tx_data[3] = (data >> 8)  & 0x00ff;
		tx_data[4] = (data)	      & 0x00ff;

		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
		HAL_GPIO_WritePin(MOTOR_CS_GPIO_Port, MOTOR_CS_Pin, GPIO_PIN_RESET);

		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
		HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) tx_data, (uint8_t *) rx_data,  sizeof(rx_data), HAL_MAX_DELAY );

		while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
		HAL_GPIO_WritePin(MOTOR_CS_GPIO_Port, MOTOR_CS_Pin, GPIO_PIN_SET);



	}

	payload = (rx_data[1] << 24) + (rx_data[2] << 16)
			+ (rx_data[3] << 8 ) +  rx_data[4];


	return payload;


}

/* ihold: 		hold current. 5 bits.  0 = 1/32, 31=32/32
 * irun:  		run current.  5 bits.  0 = 1/32, 31=32/32
 * iholddelay: 	number of clock cycles to power down. 4 bits. Delay is 2^18*iholddelay clocks periods.
 */
void tmc_set_motor_current(uint8_t ihold, uint8_t irun, uint8_t iholddelay) {
	uint32_t reg_data = 0;

	reg_data = (ihold & 0x1F)
			+  ((irun & 0x1F) << 8)
			+  ((iholddelay & 0x0F) << 16);

	tmc_readwrite_register(TMC_REG_IHOLD_IRUN, reg_data, 1);

	return;
}

/*initializes the TMC2130 driver for StealthChop operation with current appropriate for loop antenna
 *
 */
void tmc_init(void) {

	uint8_t rx_data[2] = {0,0};
	uint8_t tx_data[2] = {0,0};

	HAL_GPIO_WritePin(MOTOR_CS_GPIO_Port, MOTOR_CS_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	//Power cycle the TMC2130
	HAL_GPIO_WritePin(TMC_PWR_GPIO_Port, TMC_PWR_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(TMC_PWR_GPIO_Port, TMC_PWR_Pin, GPIO_PIN_SET);
	HAL_Delay(100);

	//write 16 bits out of SPI interface (without clearing CS pin) to set sck polarity correct.
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	HAL_SPI_TransmitReceive(&hspi1, (uint8_t *) tx_data, (uint8_t *) rx_data,  sizeof(rx_data), HAL_MAX_DELAY );

	//clear the status register by reading it
	tmc_readwrite_register(TMC_REG_GSTAT, 0, 0);

	//TOFF=4, TBL=2, HSTRT=4. HEND=0
	tmc_readwrite_register(TMC_REG_CHOPCONF, 0x5010044, 1);

	//IHOLD_IRUN: IHOLD=10, IRUN=31 (max. current), IHOLDDELAY=6
	tmc_set_motor_current(3, 10, 10);

	//approx 1 second to fully power down the motor after motion stops
	tmc_readwrite_register(TMC_REG_TPOWERDOWN, 0x40, 1);

	//en_pwm_mode = 1, DIAG1 set to indicate stall, push-pull active high
	tmc_readwrite_register(TMC_REG_GCONF, 0x2104, 1);

	//switching velocity = 35000
	tmc_readwrite_register(TMC_REG_TPWMTHRS, 0x1F4, 1);

	//approx pwm_autoscale = 1, PWM_GRAD = 1, PWM_AMPL=255
	tmc_readwrite_register(TMC_REG_PWM_CONF, 0x401C8, 1);

	return;
}



