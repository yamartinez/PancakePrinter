/*
 * UART.h
 *
 *  Created on: Dec 4, 2021
 *      Author: yudel
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <msp430.h>
#include "PortPins.h"


void Init_UART(void(*task)(uint16_t));
void UART_Write(int8_t value);
void uart_main();

#endif /* UART_H_ */
