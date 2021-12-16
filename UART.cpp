/*
 * UART.cpp
 *
 *  Created on: Dec 16, 2021
 *      Author: panda
 */
#include "UART.h"
#include "msp.h"
//------------UART2_Init------------
// Initialize the UART for 115,200 baud rate (assuming 12 MHz SMCLK clock),
// 8 bit word length, no parity bits, one stop bit
// Input: none
// Output: none
void UART2_Init(void){
  EUSCI_A2->CTLW0 = 0x0001;                   // hold the USCI module in reset mode
  // bit15=0,      no parity bits
  // bit14=x,      not used when parity is disabled
  // bit13=0,      LSB first
  // bit12=0,      8-bit data length
  // bit11=0,      1 stop bit
  // bits10-8=000, asynchronous UART mode
  // bits7-6=11,   clock source to SMCLK
  // bit5=0,       reject erroneous characters and do not set flag
  // bit4=0,       do not set flag for break characters
  // bit3=0,       not dormant
  // bit2=0,       transmit data, not address (not used here)
  // bit1=0,       do not transmit break (not used here)
  // bit0=1,       hold logic in reset state while configuring
  EUSCI_A2->CTLW0 = 0x00C1;
                                 // set the baud rate
                                 // N = clock/baud rate = 12,000,000/115,200 = 104.1667
  EUSCI_A2->BRW = 104;           // UCBR = baud rate = int(N) = 104
  EUSCI_A2->MCTLW &= ~0xFFF1;    // clear first and second modulation stage bit fields
  P3->SEL0 |= 0x0C;
  P3->SEL1 &= ~0x0C;             // configure P3.3 and P3.2 as primary module function
  EUSCI_A2->CTLW0 &= ~0x0001;    // enable the USCI module
  EUSCI_A2->IE |= 0x0001;       // enable RX interrupt (transmit ready, start received, transmit empty, receive full)
  __enable_interrupt();
  NVIC->ISER[0] = 1 << ((EUSCIA2_IRQn) & 31);
}

//------------UART0_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART0_InChar(void){
  while((EUSCI_A2->IFG&0x01) == 0);
  return((char)(EUSCI_A2->RXBUF));
}

//------------UART0_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(char letter){
  while((EUSCI_A2->IFG&0x02) == 0);
  EUSCI_A2->TXBUF = letter;
}

void uart_mainloop(){
    static uint16_t notified = 0;
    while(1){
        notified = 0;
        while(QueueSize() > 0){
            Command* c = DequeueCommand();
            if  (c != 0)
                ExecuteCommand(c);
            }
        while(QueueSize() == 0){
            UART0_OutChar('0');
            }
        }
}

void EUSCIA2_IRQHandler(){
    if (EUSCI_A2->IFG & EUSCI_A_IFG_RXIFG){
        uint8_t rx_val = EUSCI_A2->RXBUF;
        int8_t stat = BuildCommand(rx_val);
        UART0_OutChar((char)stat);
    }
}
