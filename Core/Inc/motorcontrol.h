/*
 * motorcontrol.h
 *
 *  Created on: Jan 25, 2019
 *      Author: esoren
 */

#ifndef INC_MOTORCONTROL_H_
#define INC_MOTORCONTROL_H_



typedef enum {
	HOME=1,
	MOVE_TO_POSITION,
	GET_CURRENT_POSITION,
	GET_TARGET_POSITION,
	STOP_MOTOR
} EMotorCommand;

typedef struct {
	EMotorCommand motorCommand;
	uint32_t steps;
} motorMessage_t;

void enable_motor_driver(void);
void disable_motor_driver(void);
void set_motor_dir(uint8_t motor_dir);
void send_motor_steps(uint32_t step_count, uint32_t delay_in_us);
void StartMotorTask(void const * argument);


#endif /* INC_MOTORCONTROL_H_ */
