#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"

#define TimerA0_CCR0_VALUE (12000-1)
#define LED_PORT P1
#define LED_BIT BIT0
#define LED_OUTPUT LED_PORT->OUT
#define TOGGLE_LED LED_OUTPUT ^= LED_BIT

//Bilge Batsukh
//eb5yf
//1/22/2020
//main_1.c

int main(void)
{
    // Configure system clock
    Clock_Init48MHz();

    // Configure GPIO
    LaunchPad_Init(); // built-in switches and LEDs

    // Configure TimerA0
    TIMER_A0->CTL = (TIMER_A_CTL_MC_0 | TIMER_A_CTL_CLR); // Stop and clear timer
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP; // SMCLK, up mode
    TIMER_A0->CCR[0] = TimerA0_CCR0_VALUE;
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled

    // Enable TimerA0 CCR0 interrupt for Nested Vector Interrupt Controller (NVIC).
    // See Section 2.2.2 of Technical Reference Manual and Table 6-39 of MSP432P401R datasheet.
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31); // NVIC->ISER[0] = 0x00000100;

    // Enable global interrupt
    EnableInterrupts();

    while (1)
    {
        __no_operation();                   // For debugger
    }
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // Clear interrupt flag
    TOGGLE_LED;
}
