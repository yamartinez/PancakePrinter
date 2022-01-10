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
    PUMP_PORT_SEL0 &= ~PUMP_PIN;  //configure TA0.2
    PUMP_PORT_SEL1 |= PUMP_PIN;
    PUMP_PORT_DIR |= PUMP_PIN;   //configure as TA0.2
    //configure pump pin for gpio, turn off pin

    //section contains configuration instructions for an l293 breakout board
    PUMP_ENABLE_SEL0 &= ~PUMP_ENABLE_PIN;
    PUMP_ENABLE_SEL1 &= ~PUMP_ENABLE_PIN;
    PUMP_ENABLE_DIR |= PUMP_ENABLE_PIN;
    PUMP_DISABLE;

    PUMP_REVERSE_SEL0 &= ~PUMP_REVERSE_PIN;
    PUMP_REVERSE_SEL1 |= PUMP_REVERSE_PIN;
    PUMP_REVERSE_DIR |= PUMP_REVERSE_PIN;
    PUMP_REVERSE_OUT &= ~PUMP_REVERSE_PIN;

    PUMP_DIRECTION_SEL0 &= ~PUMP_DIRECTION_PIN; //THIS STUFF DOESN'T ACTUALLY DO ANYTHING, I THINK??? ONLY 3 PINS ARE NECESSARY FOR THIS MOTOR DRIVER
    PUMP_DIRECTION_SEL1 &= ~PUMP_DIRECTION_PIN;
    PUMP_DIRECTION_DIR |= PUMP_DIRECTION_PIN;
    PUMP_EXTRUDE;


    //end l293 configuration instructions

    // Setup Timer0_A
    TA0CCR0 = 100-1;                                  // PWM Period
    TA0CCTL2 = OUTMOD_7;                              // CCR2 reset/set
    TA0CCR2 = 0;                                     // CCR2 PWM duty cycle
    TA0CTL = TASSEL_1 | MC_1 | TACLR;                 // ACLK, up mode, clear TAR

    // Setup Timer3_A
    TA3CCR0 = 100-1;
    TA3CCTL2 = OUTMOD_7;
    TA3CCR2 = 0;
    TA3CTL = TASSEL_1 |MC_1 | TACLR;
}

/**
 * @brief Begin driving pump pin at 50% duty cycle
 *
 */
void PumpDrive(){

    //stop all reversing
    TA3CCTL2 = OUTMOD_7; // CCR2 reset/set
    TA3CCR2 = 0; //set duty cycle to 0/100
    TA3CTL |= TACLR; // clear TAR

    //enable forwarding
    TA0CCTL2 = OUTMOD_7; // CCR2 reset/set
    TA0CCR2 = 15; //set duty cycle to 15/100
    TA0CTL |= TACLR; // clear TAR

}

void PumpSetSpeed(uint8_t Duty){
    if (Duty > 99){
        Duty = 99; // if out of range, default to max/min
    }
    else if (Duty < 0){
        Duty = 0;
    }
    TA0CCTL2 = OUTMOD_7; // CCR1 reset/set
    TA0CCR2 = Duty; //set duty cycle to Duty/100
    TA0CTL |= TACLR; // clear TAR
}

void PumpStop(){
    PumpSetSpeed(0);
}

void PumpReverseDrive(){
    //stop all forwarding
    TA0CCTL2 = OUTMOD_7; // CCR2 reset/set
    TA0CCR2 = 0; //set duty cycle to 0/100
    TA0CTL |= TACLR; // clear TAR

    //enable reversing
    TA3CCTL2 = OUTMOD_7; // CCR2 reset/set
    TA3CCR2 = 35; //set duty cycle to 35/100
    TA3CTL |= TACLR; // clear TAR
}
