/*
 * debug.c
 *
 *  Created on: Apr 10, 2022
 *      Author: Eric
 */

#include "usart.h"

void debugPrintln(UART_HandleTypeDef *huart, char _out[]){
	HAL_UART_Transmit(huart, (uint8_t *) _out, strlen(_out), 10);
	char newline[2] = "\r\n";
	HAL_UART_Transmit(huart, (uint8_t *) newline, 2, 10);
}



