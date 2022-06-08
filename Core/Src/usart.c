/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "FreeRTOS.h"
#include "queue.h"
#include "motorcontrol.h"

extern QueueHandle_t xMotorQueue; //todo: find a better place for this

/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, USART_TX_Pin|USART_RX_Pin);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

QueueHandle_t xSerialQueue;


volatile uint8_t uartBuffer;

void debugPrintln(UART_HandleTypeDef *huart, char _out[]){
	HAL_UART_Transmit(huart, (uint8_t *) _out, strlen(_out), 10);
	char newline[2] = "\r\n";
	HAL_UART_Transmit(huart, (uint8_t *) newline, 2, 10);
	return;
}


void send_uart(UART_HandleTypeDef *huart, char _out[]){
	HAL_UART_Transmit(huart, (uint8_t *) _out, strlen(_out), 10);
	return;
}

void setup_uart_receive() {
	HAL_UART_Receive_IT(&huart2, &uartBuffer, 1);
	return;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	BaseType_t xStatus;

	xStatus = xQueueSendToBackFromISR(xSerialQueue, &uartBuffer, 0);
	HAL_UART_Receive_IT(&huart2, &uartBuffer, 1);


    return;
}



void StartSerialTask(void const *argument)
{
	char serialChar = 0;
	char serialBuffer[99];
	char numStr[20];
	int val = 0;

	uint8_t bufPos = 0;

	BaseType_t xStatus;
	uint8_t test = 1;

	motorMessage_t motorMessage;

	for(;;) {



		if (uxQueueMessagesWaiting(xSerialQueue) > 0) {
			xStatus = xQueueReceive(xSerialQueue, &serialChar, 0);
			send_uart(&huart2, &serialChar);
			serialBuffer[bufPos] = serialChar;
			bufPos++;
			//VERY crude message handling. Need to re-write this..

			if(serialChar == '\r') { //new line, process and send command
				send_uart(&huart2, "\n");

				switch(serialBuffer[0]) {
				case('m'):
					for(int j = 0; j < 20; j++) { //empty the buffer
						numStr[j] = ' ';
					}

					for(int i = 1; i < bufPos-1; i++) {
						numStr[i-1] = serialBuffer[i];
					}
					val = atoi(&numStr);

					motorMessage.motorCommand = MOVE_TO_POSITION;
					motorMessage.steps = val;

					//todo: add bounds checking for steps
					xStatus = xQueueSendToBack(xMotorQueue, &motorMessage, 0);

					if (xStatus != pdPASS) {
						//todo: add an assert or wait
					}
				}
				bufPos = 0; //reset the buffer position

			}

		}

	    osDelay(10);



	}
}



/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
