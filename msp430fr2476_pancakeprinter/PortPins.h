/*
 * PortPins.h
 *
 *  Created on: Dec 15, 2021
 *      Author: bilge
 */

#ifndef PORTPINS_H_
#define PORTPINS_H_

#include <stdbool.h>
#include <stdint.h>
#include "msp430.h"
#include "./Error.h"

/**
 *  MX enable - P5.0
 *  MY enable - P3.7
 *
 *  recall that enables are to be held high
 *
 */

#define MX_ENABLE_PORT P5
#define MX_ENABLE_PIN BIT0 
#define MX_ENABLE_PORT_SEL0 P5SEl0
#define MX_ENABLE_PORT_SEL1 P5SEL1
#define MX_ENABLE_PORT_DIR P5DIR
#define MX_ENABLE_PORT_OUT P5OUT

#define MY_ENABLE_PORT P3
#define MY_ENABLE_PIN BIT7 
#define MY_ENABLE_PORT_SEL0 P3SEL0
#define MY_ENABLE_PORT_SEL1 P3SEL1
#define MY_ENABLE_PORT_DIR P3DIR
#define MY_ENABLE_PORT_OUT P3OUT

/**
 * MX direction - P4.7
 * MY direction - P3.1
 *
 */

#define MX_DIR_PORT P4
#define MX_DIR_PIN BIT7 //7th pin, 0x80
#define MX_DIR_PORT_SEL0 P4SEL0
#define MX_DIR_PORT_SEL1 P4SEL1
#define MX_DIR_PORT_DIR P4DIR
#define MX_DIR_PORT_OUT P4OUT


#define MY_DIR_PORT P3
#define MY_DIR_PIN BIT1 
#define MY_DIR_PORT_SEL1 P3SEL1
#define MY_DIR_PORT_SEL0 P3SEL0
#define MY_DIR_PORT_DIR P3DIR
#define MY_DIR_PORT_OUT P3OUT

/**
 * MX uS0 - P6.0
 * MX uS1 - P3.3
 *
 * MY uS0 - P4.1
 * MY uS1 - P4.5
 *
 * hold low unless microstepping is viable and needed
 *
 */
#define MX_uS0_PORT P6
#define MX_uS0_PIN BIT0 
#define MX_uS0_PORT_SEL0 P6SEL0
#define MX_uS0_PORT_SEL1 P6SEL1
#define MX_uS0_PORT_DIR P6DIR
#define MX_uS0_PORT_OUT P6OUT

#define MY_uS0_PORT P3
#define MY_uS0_PIN BIT3 
#define MY_uS0_PORT_SEL0 P3SEL0
#define MY_uS0_PORT_SEL1 P3SEL1
#define MY_uS0_PORT_DIR P3DIR
#define MY_uS0_PORT_OUT P3OUT


#define MX_uS1_PORT P4
#define MX_uS1_PIN BIT1  
#define MX_uS1_PORT_SEL0 P4SEL0
#define MX_uS1_PORT_SEL1 P4SEL1
#define MX_uS1_PORT_DIR P4DIR
#define MX_uS1_PORT_OUT P4OUT

#define MY_uS1_PORT P4
#define MY_uS1_PIN BIT5
#define MY_uS1_PORT_SEL0 P4SEL0
#define MY_uS1_PORT_SEL1 P4SEL1
#define MY_uS1_PORT_DIR P4DIR
#define MY_uS1_PORT_OUT P4OUT

/**
 * MX Reset - P1.2
 * MY Reset - P1.3
 *
 * recall the reset pins are active low, so they are to be held high under normal operation
 *
 */
#define MX_RESET_PORT P1
#define MX_RESET_PIN BIT2 
#define MX_RESET_PORT_SEL0 P1SEL0
#define MX_RESET_PORT_SEL1 P1SEL1
#define MX_RESET_PORT_DIR P1DIR
#define MX_RESET_PORT_OUT P1OUT

#define MY_RESET_PORT P1
#define MY_RESET_PIN BIT3 
#define MY_RESET_PORT_SEL0 P1SEL0
#define MY_RESET_PORT_SEL1 P1SEL0
#define MY_RESET_PORT_DIR P1DIR
#define MY_RESET_PORT_OUT P1OUT


/**
 * MX Step - P4.2
 * MY Step - P5.3
 *
 */

#define MX_STEP_PORT P4
#define MX_STEP_PIN BIT2 
#define MX_STEP_PORT_SEL0 P4SEL0
#define MX_STEP_PORT_SEL1 P4SEL1
#define MX_STEP_PORT_DIR P4DIR
#define MX_STEP_PORT_OUT P4OUT


#define MY_STEP_PORT P5
#define MY_STEP_PIN BIT43
#define MY_STEP_PORT_SEL0 P5SEL0
#define MY_STEP_PORT_SEL1 P5SEL1
#define MY_STEP_PORT_DIR P5DIR
#define MY_STEP_PORT_OUT P5OUT

/**
 * Pump PWM output - P4.6
 *
 */

#define PUMP_PORT P4
#define PUMP_PIN BIT6
#define PUMP_PORT_SEL0 P4SEL0
#define PUMP_PORT_SEL1 P4SEL1
#define PUMP_PORT_DIR P4DIR
#define PUMP_PORT_OUT P4OUT

/**
 * Debug outputs
 * LED1 - P1.0
 * S1 - P4.0
 * S2 - P2.3
 *
 */
#define LED1_PORT P1
#define S1_PORT P4
#define S2_PORT P2
#define LED1_PORT_SEL0   LED1_PORT->SEL0
#define LED1_PORT_SEL1   LED1_PORT->SEL1
#define LED1_PORT_DIR    LED1_PORT->DIR
#define LED1_PORT_OUT    LED1_PORT->OUT
#define LED1_PIN         BIT0 //0th pin, 0x01

#define S1_PORT_SEL0 P4SEL0
#define S1_PORT_SEL1 P4SEL1
#define S1_PORT_DIR P4DIR
#define S1_PORT_OUT P4OUT
#define S1_PORT_IFG P4IFG
#define S1_PORT_IE P4IE
#define S1_PORT_IES P4IES
#define S1_PORT_IN P4IN
#define S1_PORT_REN P4REN
#define S1_PIN BIT0 

#define S2_PORT_SEL0 P2SEL0
#define S2_PORT_SEL1 P2SEL1
#define S2_PORT_DIR P2DIR
#define S2_PORT_OUT P2OUT
#define S2_PORT_IFG P2IFG
#define S2_PORT_IE P2IE
#define S2_PORT_IES P2IES
#define S2_PORT_IN P2IN
#define S2_PORT_REN P2REN
#define S2_PIN BIT3 


/**
 * Limit Switches
 * MX_Limit0 - P2.0
 * MX_Limit1 - P4.2
 *
 * MY_Limit0 - P3.2
 * MY_Limit1 - P3.6
 *
 */

#define MX0_LIMIT_PORT P2
#define MX0_LIMIT_PORT_SEL0 P2SEL0
#define MX0_LIMIT_PORT_SEL1 P2SEL1
#define MX0_LIMIT_PORT_DIR P2DIR
#define MX0_LIMIT_PORT_OUT P2OUT
#define MX0_LIMIT_PORT_IFG P2IFG
#define MX0_LIMIT_PORT_IE P2IE
#define MX0_LIMIT_PORT_IES P2IES
#define MX0_LIMIT_PORT_IN P2IN
#define MX0_LIMIT_PORT_REN P2REN

#define MX1_LIMIT_PORT P4
#define MX1_LIMIT_PORT_SEL0 P3SEL0
#define MX1_LIMIT_PORT_SEL1 P3SEL1
#define MX1_LIMIT_PORT_DIR P3DIR
#define MX1_LIMIT_PORT_OUT P3OUT
#define MX1_LIMIT_PORT_IFG P3IFG
#define MX1_LIMIT_PORT_IE P3IE
#define MX1_LIMIT_PORT_IES P3IES
#define MX1_LIMIT_PORT_IN P3IN
#define MX1_LIMIT_PORT_REN P3REN

#define MX_LIMIT_0_PIN BIT0 
#define MX_LIMIT_1_PIN BIT2 

#define MY_LIMIT_PORT P3
#define MY_LIMIT_PORT_SEL0 P3SEL0
#define MY_LIMIT_PORT_SEL1 P3SEL1
#define MY_LIMIT_PORT_DIR P3DIR
#define MY_LIMIT_PORT_OUT P3OUT
#define MY_LIMIT_PORT_IFG P3IFG
#define MY_LIMIT_PORT_IE P3IE
#define MY_LIMIT_PORT_IES P3IES
#define MY_LIMIT_PORT_IN P3IN
#define MY_LIMIT_PORT_REN P3REN

#define MY_LIMIT_1_PIN BIT2 //0x01, bit 0
#define MY_LIMIT_0_PIN BIT6 //0x02, bit 2

#define MY_LIMIT_PINS (MY_LIMIT_0_PIN | MY_LIMIT_1_PIN)

#define MOTOR_X_FORWARD (MX_DIR_PORT_OUT|=MX_DIR_PIN)
#define MOTOR_Y_BACKWARD (MY_DIR_PORT_OUT|=MY_DIR_PIN)
#define MOTOR_X_BACKWARD (MX_DIR_PORT_OUT&=~MX_DIR_PIN)
#define MOTOR_Y_FORWARD (MY_DIR_PORT_OUT&=~MY_DIR_PIN)

#define MX_ENABLE  (MX_ENABLE_PORT_OUT &= ~(MX_ENABLE_PIN))
#define MX_DISABLE (MX_ENABLE_PORT_OUT |=  (MX_ENABLE_PIN))
#define MY_ENABLE  (MY_ENABLE_PORT_OUT &= ~(MY_ENABLE_PIN))
#define MY_DISABLE (MY_ENABLE_PORT_OUT |=  (MY_ENABLE_PIN))

#define WAIT_500MS (__delay_cycles(48000000>>2));

#endif /* PORTPINS_H_ */
