/*
 * UART.c
 *
 *  Created on: Dec 4, 2021
 *      Author: yudel
 *  all credit to darren lu at TI
 * todo: make it work for the msp430fr2477
 */

#include "./UART.h"
#include <msp430.h>
#include "DebugIO.h"

void (*task)(uint16_t);

void Software_Trim();                       // Software Trim to get the best DCOFTRIM value
#define MCLK_FREQ_MHZ 8                     // MCLK = 8MHz

void UART_Write(int8_t data){
    while(!(UCA0IFG&UCTXIFG));
    UCA0TXBUF = data;
}

void Init_UART(void(*task_)(uint16_t)){

    UART_CTLW0 |= UCSWRST; //place eUSCI_A in hold reset
    UART_CTLW0 &= ~UCPEN; //disable parity bit
    //since parity bit is disabled, don't care abot UCPAR
    UART_CTLW0 &= ~UCMSB; //LSB first
    UART_CTLW0 &= ~UC7BIT; //8-bit character length
    UART_CTLW0 &= ~UCSPB; // 1 stop bit
    UART_CTLW0 &= ~(UCMODE0 | UCMODE1);
    UART_CTLW0 |= UCSYNC; //synchronous mode IMPORTANT: idk what this does so i'm setting it arbitrarily
    UART_CTLW0 |= UCSSEL_1; // set ACLK as BRCLK

    //configure the UART function on 1.6 and 1.7
    UART_SEL0 |= RX_PIN | TX_PIN;
    UART_SEL1 &= ~(RX_PIN | TX_PIN);
    //configure direction, which does not matter
    UART_PDIR |= RX_PIN | TX_PIN;
    UART_PREN |= RX_PIN | TX_PIN;
    UART_POUT &= ~(RX_PIN | TX_PIN);
    PM5CTL0 &= ~LOCKLPM5;                     // Disable the GPIO power-on default high-impedance mode

    // Baud Rate calculation. Referred to UG 17.3.10
    // (1) N=32768/4800=6.827
    // (2) OS16=0, UCBRx=INT(N)=6
    // (4) Fractional portion = 0.827. Refered to UG Table 17-4, UCBRSx=0xEE.
    UART_BAUDRATE0 = 6;                              // INT(32768/4800) - refer to user guide when necessary for changing baud rate
    UART_BAUDRATE1 = 0x00;
    UART_MCTLW = 0xEE00;

    UART_CTLW0 &= ~UCSWRST; // Initialize eUSCI
    UART_RX_INTERRUPT_ENABLE;// Enable USCI_A0 RX interrupt

    task = task_;
}

void uart_main(){
    __bis_SR_register(SCG0);                 // disable FLL
    CSCTL3 |= SELREF__REFOCLK;               // Set REFO as FLL reference source
    CSCTL1 = DCOFTRIMEN_1 | DCOFTRIM0 | DCOFTRIM1 | DCORSEL_3;// DCOFTRIM=3, DCO Range = 8MHz
    CSCTL2 = FLLD_0 + 243;                  // DCODIV = 8MHz
    __delay_cycles(3);
    __bic_SR_register(SCG0);                // enable FLL
    Software_Trim();                        // Software Trim to get the best DCOFTRIM value

    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK; // set default REFO(~32768Hz) as ACLK source, ACLK = 32768Hz
                                             // default DCODIV as MCLK and SMCLK source

    // Configure UART pins
    P1SEL0 |= BIT6 | BIT7;                    // set 2-UART pin as second function

    // Configure UART
    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;

    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 17-4: UCBRSx = 0x49
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BR0 = 52;                             // 8000000/16/9600
    UCA0BR1 = 0x00;
    UCA0MCTLW = 0x4900 | UCOS16 | UCBRF_1;

    UCA0CTLW0 &= ~UCSWRST;                    // Initialize eUSCI
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

void Software_Trim()
{
    unsigned int oldDcoTap = 0xffff;
    unsigned int newDcoTap = 0xffff;
    unsigned int newDcoDelta = 0xffff;
    unsigned int bestDcoDelta = 0xffff;
    unsigned int csCtl0Copy = 0;
    unsigned int csCtl1Copy = 0;
    unsigned int csCtl0Read = 0;
    unsigned int csCtl1Read = 0;
    unsigned int dcoFreqTrim = 3;
    unsigned char endLoop = 0;

    do
    {
        CSCTL0 = 0x100;                         // DCO Tap = 256
        do
        {
            CSCTL7 &= ~DCOFFG;                  // Clear DCO fault flag
        }while (CSCTL7 & DCOFFG);               // Test DCO fault flag

        __delay_cycles((unsigned int)3000 * MCLK_FREQ_MHZ);// Wait FLL lock status (FLLUNLOCK) to be stable
                                                           // Suggest to wait 24 cycles of divided FLL reference clock
        while((CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)) && ((CSCTL7 & DCOFFG) == 0));

        csCtl0Read = CSCTL0;                   // Read CSCTL0
        csCtl1Read = CSCTL1;                   // Read CSCTL1

        oldDcoTap = newDcoTap;                 // Record DCOTAP value of last time
        newDcoTap = csCtl0Read & 0x01ff;       // Get DCOTAP value of this time
        dcoFreqTrim = (csCtl1Read & 0x0070)>>4;// Get DCOFTRIM value

        if(newDcoTap < 256)                    // DCOTAP < 256
        {
            newDcoDelta = 256 - newDcoTap;     // Delta value between DCPTAP and 256
            if((oldDcoTap != 0xffff) && (oldDcoTap >= 256)) // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim--;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }
        else                                   // DCOTAP >= 256
        {
            newDcoDelta = newDcoTap - 256;     // Delta value between DCPTAP and 256
            if(oldDcoTap < 256)                // DCOTAP cross 256
                endLoop = 1;                   // Stop while loop
            else
            {
                dcoFreqTrim++;
                CSCTL1 = (csCtl1Read & (~DCOFTRIM)) | (dcoFreqTrim<<4);
            }
        }

        if(newDcoDelta < bestDcoDelta)         // Record DCOTAP closest to 256
        {
            csCtl0Copy = csCtl0Read;
            csCtl1Copy = csCtl1Read;
            bestDcoDelta = newDcoDelta;
        }

    }while(endLoop == 0);                      // Poll until endLoop == 1

    CSCTL0 = csCtl0Copy;                       // Reload locked DCOTAP
    CSCTL1 = csCtl1Copy;                       // Reload locked DCOFTRIM
    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1)); // Poll until FLL is locked
}

//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
//#pragma vector=USCI_A0_VECTOR
//__interrupt void USCI_A0_ISR(void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
//#else
//#error Compiler not supported!
//#endif
//{
//  switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
//  {
//    case USCI_NONE: break;
//    case USCI_UART_UCRXIFG:
//      while(!(UCA0IFG&UCTXIFG));
//      UCA0TXBUF = UCA0RXBUF;
//
//      __no_operation();
//      break;
//    case USCI_UART_UCTXIFG: break;
//    case USCI_UART_UCSTTIFG: break;
//    case USCI_UART_UCTXCPTIFG: break;
//    default: break;
//  }
//}
