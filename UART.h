/*
 * UART.h
 *
 *  Created on: Dec 16, 2021
 *      Author: panda
 */

#ifndef UART_H_
#define UART_H_

#include "msp.h"
#include "Carriage.h"
#include "Commands.h"
#include "PortPins.h"
#include "msp432.h"

void uart_mainloop();
void UART0_OutChar(char letter);
char UART0_InChar(void);
void UART0_Init(void);


#endif /* UART_H_ */
