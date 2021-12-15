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

/**
 * UART pins should be
 * P1.6 - UART_RX
 * P1.7 - UART_TX
 */
#define RX_PIN 0x40 //P1.6
#define TX_PIN 0x80 //P1.7

#define UART_SEL0 P1SEL0
#define UART_SEL1 P1SEL1

#define UART_PDIR P1DIR
#define UART_PREN P1REN
#define UART_POUT P1OUT

#define UART_CTLW0 UCA0CTLW0
#define UART_BAUDRATE0 UCA0BR0
#define UART_BAUDRATE1 UCA0BR1
#define UART_MCTLW UCA0MCTLW

#define UART_CTLW0_RST ~UCSWRST
#define UART_RX_INTERRUPT_ENABLE UCA0IE|=UCRXIE

void Init_UART(void(*task)(uint16_t));
void UART_Write(int8_t value);
void uart_main();

#endif /* UART_H_ */
