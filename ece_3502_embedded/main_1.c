#include <stdint.h>
#include "msp.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"

#define TimerA0_CCR0_VALUE (12000-1) //12000 - 1 means that this clock will trigger every 1 ms

#define LED_PORT P1
#define LED_BIT BIT0
#define LED_OUTPUT LED_PORT->OUT
#define TOGGLE_LED LED_OUTPUT ^= LED_BIT


//rules - no global variables.
//250 ms is a period of 4 hz
//must use the same SMCLK  and the 48 Mhz system clock
//can't change the current, pre-existing capacities of the code.

//Bilge Batsukh
//eb5yf
//1/22/2020
//main.c
//ECE 3502, activity 1, part 1


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

    }
}
void incrementCOUNTS(){

}
// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void)
{
    static uint32_t COUNTS = 0; //static var will preserve value between interrupts
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // Clear interrupt flag
    COUNTS++;
    if(COUNTS == 250) //will only toggle LED once every 250 interrupts (which occurs at 250*12000*
    {
        COUNTS = 0; //resets counter
        TOGGLE_LED;
    }
}
