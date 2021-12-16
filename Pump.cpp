/*
 * Pump.c
 *  FIX THIS ASAP - 12-15-2021 - 23:30 PM
 *  Created on: Nov 30, 2021
 *      Author: panda
 */

/**
 * @file Pump.c
 * @author bilge batsukh
 * @brief Simple MOSFET driven pump driver for our pancake printer - c file
 * @version 0.2
 * @date 2021-12-15
 *
 * @copyright none, disclaimer: use at your own risk, provided as-is without warranty or assurance of function
 *
 */

#include "./Pump.h"

void PWM_Duty2(float duty2);
void PWM_Init2(uint16_t period, uint16_t duty2);
uint16_t current_period = 0;
/**
 * @brief Initialize pump for operations
 * Pump pwm pin is P2.5, AKA PM_TA0.2
 */
void InitPump(){
//    PUMP_IN1_PORT_SEL0 &= ~PUMP_IN1_PIN;
//    PUMP_IN1_PORT_SEL1 &= ~PUMP_IN1_PIN;
//    PUMP_IN1_PORT_DIR  |= PUMP_IN1_PIN;
//    PUMP_IN1_PORT_OUT  |= PUMP_IN1_PIN; // convention shall dictate that IN1 = 1 & IN2 = 0 will be forward
//
//    PUMP_IN2_PORT_SEL0 &= ~PUMP_IN2_PIN;
//    PUMP_IN2_PORT_SEL1 &= ~PUMP_IN2_PIN;
//    PUMP_IN2_PORT_DIR  |= PUMP_IN2_PIN;
//    PUMP_IN2_PORT_OUT  &= ~PUMP_IN2_PIN; // convention shall dictate that IN1 = 1 & IN2 = 0 will be forward

    PWM_Init2(1500, 0); //0% duty cycle, 150*1.33us period = 7.7kHz
    // // Configure clock
    // P2SEL1 |= BIT6 | BIT7;                  // P2.6~P2.7: crystal pins
    // do
    // {
    //     CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
    //     SFRIFG1 &= ~OFIFG;
    // }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag

    // // Setup Timer1_B
    // TB1CCR0 = 100-1;                                  // PWM Period
    // TB1CCTL1 = OUTMOD_7;                              // CCR1 reset/set
    // TB1CCR1 = 0;                                     // CCR1 PWM duty cycle
    // TB1CTL = TBSSEL_1 | MC_1 | TBCLR;                 // ACLK, up mode, clear TBR
}

/**
 * @brief Begin driving pump pin at 50% duty cycle
 *
 */
void PumpDrive(){
    PWM_Duty2(0.5);

}


void PumpSetSpeed(float Duty){
    PWM_Duty2(Duty);
}

void PumpStop(){
    PumpSetSpeed(0);
}

//change direction - direction 1 will be IN1 = 1 & IN2 = 0
// direction 0 will be IN1 = 0 & IN2 = 1
void PumpDir(uint16_t direction){
    if(direction){
    PUMP_IN1_PORT_OUT |= PUMP_IN1_PIN;
    PUMP_IN2_PORT_OUT &= ~PUMP_IN2_PIN;
    }
    else {
        PUMP_IN1_PORT_OUT &= ~PUMP_IN1_PIN;
        PUMP_IN2_PORT_OUT |= PUMP_IN2_PIN;
    }
}
//***************************PWM_Init12*******************************
// PWM outputs on P2.5
// Inputs:  period (1.333us)
//          duty2
// Outputs: none
// SMCLK = 48MHz/4 = 12 MHz, 83.33ns
// Counter counts up to TA0CCR0 and back down
// Let Timerclock period T = 8/12MHz = 666.7ns
// P2.5=1 when timer equals TA0CCR2 on way down, P2.5=0 when timer equals TA0CCR2 on way up
// Period of P2.5 is period*1.333us, duty cycle is duty2/period
void PWM_Init2(uint16_t period, uint16_t duty2){
  if(duty2 >= period) return; // bad input
  current_period = period;
  P2->DIR |= PUMP_PWM_PIN;          //  P2.5 output
  P2->SEL0 |= PUMP_PWM_PIN;         //  P2.5 Timer0A functions
  P2->SEL1 &= ~PUMP_PWM_PIN;        //  P2.5 Timer0A functions
  TIMER_A0->CCTL[0] = 0x0080;      // CCI0 toggle
  TIMER_A0->CCR[0] = period;       // Period is 2*period*8*83.33ns is 1.333us*period
  TIMER_A0->EX0 = 0x0000;        //    divide by 1
  TIMER_A0->CCTL[2] = 0x0040;      // CCR2 toggle/reset
  TIMER_A0->CCR[2] = duty2;        // CCR2 duty cycle is duty2/period
  TIMER_A0->CTL = 0x02F0;        // SMCLK=12MHz, divide by 8, up-down mode
// bit  mode
// 9-8  10    TASSEL, SMCLK=12MHz
// 7-6  11    ID, divide by 8
// 5-4  11    MC, up-down mode
// 2    0     TACLR, no clear
// 1    0     TAIE, no interrupt
// 0          TAIFG
}


//***************************PWM_Duty2*******************************
// change duty cycle of PWM output on P2.5
// Inputs:  duty2, a float percentage of 1
// Outputs: none// period of P2.5 is 2*period*666.7ns, duty cycle is duty2/period
void PWM_Duty2(float duty2){
  if(duty2 >= 1 || duty2 < 0) return; // bad input
  uint16_t PWM_duty_diff = (uint16_t) (duty2*current_period);
  TIMER_A0->CCR[2] = current_period - PWM_duty_diff - 1;        // CCR2 duty cycle is duty2/period
}
