/*
 * delay.c
 *
 *  Created on: Jan 25, 2019
 *      Author: esoren
 */

#include "cmsis_os.h"


#pragma GCC push_options
#pragma GCC optimize ("O3")
void delayUS_DWT(uint32_t us) {
//	volatile uint32_t cycles = (SystemCoreClock/1000000L)*us;
//	volatile uint32_t start = DWT->CYCCNT;
//	do  {
//	} while(DWT->CYCCNT - start < cycles);
}
#pragma GCC pop_options
