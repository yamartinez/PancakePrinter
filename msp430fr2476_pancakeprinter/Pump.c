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
    PUMP_PORT_SEL0 |= PUMP_PIN;  //configure TA3.1
    PUMP_PORT_SEL1 &= ~PUMP_PIN;
    PUMP_PORT_DIR |= PUMP_PIN;   //configure as TA3.1
    //configure pump pin for gpio, turn off pin

    // Setup Timer3_A
    TA3CCR0 = 100-1;                                  // PWM Period
    TA3CCTL1 = OUTMOD_7;                              // CCR1 reset/set
    TA3CCR1 = 0;                                     // CCR1 PWM duty cycle
    TA3CTL = TBSSEL_1 | MC_1 | TACLR;                 // ACLK, up mode, clear TAR
}

/**
 * @brief Begin driving pump pin at 50% duty cycle
 *
 */
void PumpDrive(){
    TA3CCTL1 = OUTMOD_7; // CCR1 reset/set
    TA3CCR1 = 50; //set duty cycle to 50/100
    TA3CTL |= TBCLR; // clear TBR

}

void PumpSetSpeed(uint8_t Duty){
    if (Duty > 99){
        Duty = 99; // if out of range, default to max/min
    }
    else if (Duty < 0){
        Duty = 0;
    }
    TA3CCTL1 = OUTMOD_7; // CCR1 reset/set
    TA3CCR1 = Duty; //set duty cycle to Duty/100
    TA3CTL |= TACLR; // clear TAR
}

void PumpStop(){
    PumpSetSpeed(0);
}
