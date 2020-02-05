#include <stdint.h>
#include "msp.h"
#include "../inc/TExaS.h"
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/LaunchPad.h"
// Bilge Batsukh
// 1/27/2020
// main2.c
// activity 2
#define TimerA0_CCR0_VALUE (12000-1)
#define TIMERA0_OUT1_PORT P2
#define TIMERA0_OUT1_BIT BIT4
#define CONFIGURE_TIMERA0_OUT1_AS_AN_OUTPUT TIMERA0_OUT1_PORT->DIR |= TIMERA0_OUT1_BIT
#define SET_TIMERA0_OUT1_SEL0 TIMERA0_OUT1_PORT->SEL0 |= TIMERA0_OUT1_BIT
#define CLEAR_TIMERA0_OUT1_SEL0 TIMERA0_OUT1_PORT->SEL0 &= ~TIMERA0_OUT1_BIT
#define SET_TIMERA0_OUT1_SEL1 TIMERA0_OUT1_PORT->SEL1 |= TIMERA0_OUT1_BIT
#define CLEAR_TIMERA0_OUT1_SEL1 TIMERA0_OUT1_PORT->SEL1 &= ~TIMERA0_OUT1_BIT

#define TIMERA0_OUT2_PORT P2
#define TIMERA0_OUT2_BIT BIT5
#define CONFIGURE_TIMERA0_OUT2_AS_AN_OUTPUT TIMERA0_OUT2_PORT->DIR |= TIMERA0_OUT2_BIT
#define SET_TIMERA0_OUT2_SEL0 TIMERA0_OUT2_PORT->SEL0 |= TIMERA0_OUT2_BIT
#define CLEAR_TIMERA0_OUT2_SEL0 TIMERA0_OUT2_PORT->SEL0 &= ~TIMERA0_OUT2_BIT
#define SET_TIMERA0_OUT2_SEL1 TIMERA0_OUT2_PORT->SEL1 |= TIMERA0_OUT2_BIT
#define CLEAR_TIMERA0_OUT2_SEL1 TIMERA0_OUT2_PORT->SEL1 &= ~TIMERA0_OUT2_BIT

#define TIMERA0_OUT3_PORT P2
#define TIMERA0_OUT3_BIT BIT6
#define CONFIGURE_TIMERA0_OUT3_AS_AN_OUTPUT TIMERA0_OUT3_PORT->DIR |= TIMERA0_OUT3_BIT
#define SET_TIMERA0_OUT3_SEL0 TIMERA0_OUT3_PORT->SEL0 |= TIMERA0_OUT3_BIT
#define CLEAR_TIMERA0_OUT3_SEL0 TIMERA0_OUT3_PORT->SEL0 &= ~TIMERA0_OUT3_BIT
#define SET_TIMERA0_OUT3_SEL1 TIMERA0_OUT3_PORT->SEL1 |= TIMERA0_OUT3_BIT
#define CLEAR_TIMERA0_OUT3_SEL1 TIMERA0_OUT3_PORT->SEL1 &= ~TIMERA0_OUT3_BIT

int main(void)
{
    Clock_Init48MHz();
    //TExaS_Init(SCOPE); // Attached to P4.4

    // Configure GPIO
    LaunchPad_Init(); // built-in switches and LEDs

    // Out 1 for Timer A0 maps to P2.4
    CONFIGURE_TIMERA0_OUT1_AS_AN_OUTPUT;
    SET_TIMERA0_OUT1_SEL0; CLEAR_TIMERA0_OUT1_SEL1; // Select TA0.1

    //Out 2 for Timer A0 maps to P2.5
    CONFIGURE_TIMERA0_OUT2_AS_AN_OUTPUT;
    SET_TIMERA0_OUT2_SEL0; CLEAR_TIMERA0_OUT2_SEL1; //Select TA0.2

    //Out 3 for Timer A0 maps to P2.6
    CONFIGURE_TIMERA0_OUT3_AS_AN_OUTPUT;
    SET_TIMERA0_OUT3_SEL0; CLEAR_TIMERA0_OUT3_SEL1; //Select TA0.3

    TIMER_A0->CTL = (TIMER_A_CTL_MC_0 | TIMER_A_CTL_CLR); // Stop and clear timer
    TIMER_A0->CCR[0] = TimerA0_CCR0_VALUE;
    TIMER_A0->CCR[1] = TimerA0_CCR0_VALUE >> 1; // 50% duty cycle for TA0.1
    TIMER_A0->CCR[2] = TimerA0_CCR0_VALUE >> 2;  // 25% duty cycle for TA0.2
    TIMER_A0->CCR[3] = (TimerA0_CCR0_VALUE >> 1) + (TimerA0_CCR0_VALUE >> 2); //75% duty cycle for TA0.3

    TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_OUTMOD_7; //output mode 7 is reset/set - at CCR[x] = CCR[0] , the assigned pin will be reset to 0
    TIMER_A0->CCTL[2] |= TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CCTL[3] |= TIMER_A_CCTLN_OUTMOD_7;
    TIMER_A0->CTL = (TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP); // SMCLK, up mode

    while (1)
    {
        __no_operation();                   // For debugger
    }
}