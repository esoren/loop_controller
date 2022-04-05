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
#include "tmc2130.h"


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


void StartMotorTask(void const *argument)
{
	uint8_t first = 1;
	for(;;)
	  {
		  if(first) {
			  uint8_t dir = 1;
			  uint32_t wait = 20000;
			  uint8_t move = 1;
			  uint32_t res = 0;
			  uint32_t sg_result = 0;
			  uint32_t min_sg_result = 1024;
			  uint32_t max_sg_result = 0;


			  uint32_t count = 0;
			  uint8_t status = 0;
			  uint8_t homing = 1;
			  set_motor_dir(0);

			  while(homing == 1) {
				  if(move) {
					  send_motor_steps(400,wait);
					  HAL_Delay(25); //wait for motor to settle before checking the stall result

					  res = tmc_readwrite_register(TMC_REG_DRV_STATUS, res, 0);
					  sg_result = res & 0x3ff;


					  if(sg_result > max_sg_result) max_sg_result = sg_result;
					  if(sg_result > 0 && sg_result < min_sg_result ) min_sg_result = sg_result;

					  status = tmc_get_status();
					  if((status >> 2) & 0x01) { //if stall is detected
						  move = 0;
					  }



				  } else {
					  homing = 0;
				  }
			  }

			  set_motor_dir(1);
			  send_motor_steps(19100,wait);
			  first = 0;
		  }





	    osDelay(1);
	  }

}
