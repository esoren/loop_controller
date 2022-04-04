/*
 * motorcontrol.c
 *
 *  Created on: Jan 25, 2019
 *      Author: esoren
 */


#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "motorcontrol.h"
#include "dwt_stm32_delay.h"
#include "tim.h"

void enable_motor_driver(void) {
	HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, GPIO_PIN_SET);
}

void disable_motor_driver(void) {
	HAL_GPIO_WritePin(MOTOR_EN_GPIO_Port, MOTOR_EN_Pin, GPIO_PIN_RESET);
}


void set_motor_dir(uint8_t motor_dir) {
	if(motor_dir == 0) {
		HAL_GPIO_WritePin(MOTOR_DIR_GPIO_Port, MOTOR_DIR_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(MOTOR_DIR_GPIO_Port, MOTOR_DIR_Pin, GPIO_PIN_SET);
	}
}

void send_motor_steps(uint32_t step_count, uint32_t delay_in_us) {

	__HAL_TIM_SET_AUTORELOAD(&htim1, step_count);
	__HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);
	HAL_TIM_OC_Start_IT(&htim1, TIM_CHANNEL_1);

	__HAL_TIM_SET_AUTORELOAD(&htim2,delay_in_us);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 100);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

}
