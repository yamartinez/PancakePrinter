/**
 * @file Pump.c
 * @author bilge batsukh
 * @brief Simple MOSFET driven pump driver for our pancake printer - cpp file
 * @version 0.1
 * @date 2021-11-30
 *
 * @copyright none, disclaimer: use at your own risk, provided as-is without warranty or assurance of function
 *
 */

#include "./Pump.h"

/**
 * @brief Initialize pump for operations
 *
 */
void InitPump(){
    PUMP_PORT_SEL0 |= PUMP_PIN;  //configure TB1.1
    PUMP_PORT_SEL1 &= ~PUMP_PIN;
    PUMP_PORT_DIR |= PUMP_PIN;   //configure as TB1.1
    //configure pump pin for gpio, turn off pin

    // Configure clock
    P2SEL1 |= BIT6 | BIT7;                  // P2.6~P2.7: crystal pins
    do
    {
        CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
        SFRIFG1 &= ~OFIFG;
    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag

    // Setup Timer1_B
    TB1CCR0 = 100-1;                                  // PWM Period
    TB1CCTL1 = OUTMOD_7;                              // CCR1 reset/set
    TB1CCR1 = 0;                                     // CCR1 PWM duty cycle
    TB1CTL = TBSSEL_1 | MC_1 | TBCLR;                 // ACLK, up mode, clear TBR
}

/**
 * @brief Begin driving pump pin at 50% duty cycle
 *
 */
void PumpDrive(){
    TB1CCTL1 = OUTMOD_7; // CCR1 reset/set
    TB1CCR1 = 50; //set duty cycle to 50/100
    TB1CTL |= TBCLR; // clear TBR

}

void PumpSetSpeed(uint8_t Duty){
    if (Duty > 99){
        Duty = 50; // if out of range, default to 50
    }
    TB1CCTL1 = OUTMOD_7; // CCR1 reset/set
    TB1CCR1 = Duty; //set duty cycle to Duty/100
    TB1CTL |= TBCLR; // clear TBR
}

void PumpStop(){
    PumpSetSpeed(0);
}
