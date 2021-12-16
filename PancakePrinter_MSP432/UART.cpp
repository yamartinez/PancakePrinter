/*
 * UART.cpp
 *
 *  Created on: Dec 16, 2021
 *      Author: panda
 */
#include "UART.h"
#include "msp.h"
//------------UART0_Init------------
// Initialize the UART for 115,200 baud rate (assuming 12 MHz SMCLK clock),
// 8 bit word length, no parity bits, one stop bit
// Input: none
// Output: none
void UART0_Init(void){
  EUSCI_A0->CTLW0 = 0x0001;                   // hold the USCI module in reset mode
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
  EUSCI_A0->CTLW0 = 0x00C1;
                                 // set the baud rate
                                 // N = clock/baud rate = 12,000,000/115,200 = 104.1667
  EUSCI_A0->BRW = 104;           // UCBR = baud rate = int(N) = 104
  EUSCI_A0->MCTLW &= ~0xFFF1;    // clear first and second modulation stage bit fields
  P1->SEL0 |= 0x0C;
  P1->SEL1 &= ~0x0C;             // configure P1.3 and P1.2 as primary module function
  EUSCI_A0->CTLW0 &= ~0x0001;    // enable the USCI module
  EUSCI_A0->IE |= 0x0001;       // enable RX interrupt (transmit ready, start received, transmit empty, receive full)
}

//------------UART0_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
char UART0_InChar(void){
  while((EUSCI_A0->IFG&0x01) == 0);
  return((char)(EUSCI_A0->RXBUF));
}

//------------UART0_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART0_OutChar(char letter){
  while((EUSCI_A0->IFG&0x02) == 0);
  EUSCI_A0->TXBUF = letter;
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

//#pragma vector=USCI_A0_VECTOR
//__interrupt void USCI_A0_ISR(void){
//    switch(__even_in_range(UCA0IV,18))
//    {
//    case 0x00: //no interrupts
//        break;
//    case 0x02: //vector 2: UCRXIFG
//        while(!(EUSCI_A0->IFG && EUSCI_A0->UCTXIFG));
//        int8_t val = BuildCommand(EUSCI_A0->RXBUF);
//        if(val){
//            EUSCI_A0->TXBUF = val;
//        }
//        break;
//    case 0x04: //vector 4, UCTXIFG - not enabled
//        break;
//    case 0x06: //vector 6, UCTSTTIFG - not enabled
//            break;
//    case 0x08: //vector 8, UCTXCPTIFG - not enabled
//            break;
//    default: break;
//    }
//}

