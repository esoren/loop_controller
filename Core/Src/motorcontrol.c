/*
 * motorcontrol.c
 *
 *  Created on: Jan 25, 2019
 *      Author: esoren
 */

#include "FreeRTOS.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include "motorcontrol.h"
#include "tim.h"
#include "tmc2130.h"

//extern QueueHandle_t xMotorQueue;

QueueHandle_t xMotorQueue;

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


void stop_motor() {
	  HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
	  HAL_TIM_OC_Stop_IT(&htim1, TIM_CHANNEL_1);
	  return;
}

void StartMotorTask(void const *argument)
{
	motorMessage_t motorMessage;
	BaseType_t xStatus;
	uint8_t homing = 0;
	uint8_t start_movement = 0;
	uint8_t moving = 0;

	uint32_t current_position_in_steps = 0;
	uint32_t target_position_in_steps = 0;
	uint32_t next_target_position_in_steps = 0;
	int32_t step_delta = 0;
	for(;;)
	  {


		if (uxQueueMessagesWaiting(xMotorQueue) > 0) {
			xStatus = xQueueReceive(xMotorQueue, &motorMessage, 0);
			if (xStatus == pdPASS) {

				switch (motorMessage.motorCommand) {
					case (HOME):
						target_position_in_steps = 0;
						homing = 1;
						break;
					case(MOVE_TO_POSITION):
						next_target_position_in_steps = motorMessage.steps;
						start_movement = 1;
						break;
					case(STOP_MOTOR):
						if(!homing) {
							moving = 0;
							current_position_in_steps = target_position_in_steps; //todo: check the status of the counter and adjust, if necessary
						}
						break;
				}

			} else {
				//todo: error handling
			}
		}

		if(homing) { //todo: make this non-blocking
			set_motor_dir(0);
			uint32_t wait = 20000;
			uint32_t at_home_position = 0;
			uint8_t status = 0;

			while(homing) {
			  if(!at_home_position) {
				  send_motor_steps(400,wait);
				  HAL_Delay(40); //wait for motor to settle before checking the stall result

				  status = tmc_get_status();
				  if((status >> 2) & 0x01) { //if stall is detected
					  at_home_position = 1;
					  homing = 0;
					  current_position_in_steps = 0;
					  moving = 0;
					  stop_motor();
				  }
			  }
			}

		} else {
			if(start_movement && !moving) {
				target_position_in_steps = next_target_position_in_steps;
				step_delta = target_position_in_steps - current_position_in_steps;
				if(step_delta > 0) {
						set_motor_dir(1);
				} else {
						set_motor_dir(0);
				}

				send_motor_steps(  abs(step_delta)  , 20000);
				moving = 1;
				start_movement = 0;

			}

		}






	    osDelay(100);
	  }

}
