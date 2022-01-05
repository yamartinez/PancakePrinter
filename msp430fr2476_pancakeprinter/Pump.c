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
 * todo: FIX TO USE TA3.1
 */
void InitPump(){
//    PUMP_PORT_SEL0 |= PUMP_PIN;  //configure TA3.1
//    PUMP_PORT_SEL1 &= ~PUMP_PIN;
//    PUMP_PORT_DIR |= PUMP_PIN;   //configure as TA3.1
//    //configure pump pin for gpio, turn off pin
//
//    // Configure clock
//    P2SEL1 |= BIT6 | BIT7;                  // P2.6~P2.7: crystal pins
//    do
//    {
//        CSCTL7 &= ~(XT1OFFG | DCOFFG);      // Clear XT1 and DCO fault flag
//        SFRIFG1 &= ~OFIFG;
//    }while (SFRIFG1 & OFIFG);               // Test oscillator fault flag
//
//    // Setup Timer3_A

}

/**
 * @brief Begin driving pump pin at 50% duty cycle
 *
 */
void PumpDrive(){


}

void PumpSetSpeed(uint8_t Duty){
//    if (Duty > 99){
//        Duty = 50; // if out of range, default to 50
//    }

}

void PumpStop(){
    PumpSetSpeed(0);
}
