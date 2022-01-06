/*
 * PortPins.h
 *
 *  Created on: Dec 15, 2021
 *      Author: bilge
 *  up to date for revision 3.0 of the printed circuit board, 
 *  adjusted for MSP430fr2477
 */

#ifndef PORTPINS_H_
#define PORTPINS_H_

#include <stdbool.h>
#include <stdint.h>
#include "msp430.h"
#include "./Error.h"

/**
 *  MX enable - P4.7
 *  MY enable - P1.7
 *
 *  recall that enables are to be held high
 *
 */

#define MX_ENABLE_PORT P4
#define MX_ENABLE_PIN BIT7 
#define MX_ENABLE_PORT_SEL0 P4SEL0
#define MX_ENABLE_PORT_SEL1 P4SEL1
#define MX_ENABLE_PORT_DIR P4DIR
#define MX_ENABLE_PORT_OUT P4OUT

#define MY_ENABLE_PORT P1
#define MY_ENABLE_PIN BIT7 
#define MY_ENABLE_PORT_SEL0 P1SEL0
#define MY_ENABLE_PORT_SEL1 P1SEL1
#define MY_ENABLE_PORT_DIR P1DIR
#define MY_ENABLE_PORT_OUT P1OUT

/**
 * MX direction - P5.0
 * MY direction - P4.3
 *
 */

#define MX_DIR_PORT P5
#define MX_DIR_PIN BIT0
#define MX_DIR_PORT_SEL0 P5SEL0
#define MX_DIR_PORT_SEL1 P5SEL1
#define MX_DIR_PORT_DIR P5DIR
#define MX_DIR_PORT_OUT P5OUT


#define MY_DIR_PORT P4
#define MY_DIR_PIN BIT3 
#define MY_DIR_PORT_SEL1 P4SEL1
#define MY_DIR_PORT_SEL0 P4SEL0
#define MY_DIR_PORT_DIR P4DIR
#define MY_DIR_PORT_OUT P4OUT


//NOTE1: TO ALL READERS - THESE PINS AND THEIR DEFINITIONS ARE DEPRECIATED FUNCTIONALITY AS OF 
//REVISION 3.0 - DEFINITIONS ARE PROPERTIES SOLELY OF REV 2.0 AND SHOULD NOT BE RELIED UPON FOR FUNCTIONALITY
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

#define MX_uS1_PORT P3
#define MX_uS1_PIN BIT3
#define MX_uS1_PORT_SEL0 P3SEL0
#define MX_uS1_PORT_SEL1 P3SEL1
#define MX_uS1_PORT_DIR P3DIR
#define MX_uS1_PORT_OUT P3OUT

#define MY_uS0_PORT P4
#define MY_uS0_PIN BIT1
#define MY_uS0_PORT_SEL0 P4SEL0
#define MY_uS0_PORT_SEL1 P4SEL1
#define MY_uS0_PORT_DIR P4DIR
#define MY_uS0_PORT_OUT P4OUT

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

//END NOTE1 


/**
 * MX Step - P5.1
 * MY Step - P4.4
 *
 */

#define MX_STEP_PORT P5
#define MX_STEP_PIN BIT1
#define MX_STEP_PORT_SEL0 P5SEL0
#define MX_STEP_PORT_SEL1 P5SEL1
#define MX_STEP_PORT_DIR P5DIR
#define MX_STEP_PORT_OUT P5OUT


#define MY_STEP_PORT P4
#define MY_STEP_PIN BIT4
#define MY_STEP_PORT_SEL0 P4SEL0
#define MY_STEP_PORT_SEL1 P4SEL1
#define MY_STEP_PORT_DIR P4DIR
#define MY_STEP_PORT_OUT P4OUT

/**
 * Pump PWM output - P1.2
 *
 */

#define PUMP_PORT P1
#define PUMP_PIN BIT2
#define PUMP_PORT_SEL0 P1SEL0
#define PUMP_PORT_SEL1 P1SEL1
#define PUMP_PORT_DIR P1DIR
#define PUMP_PORT_OUT P1OUT

/**
 * Debug outputs
 * LED1 - P1.0
 * S1 - P2.3
 * S2 - P4.0
 * 
 * RGB LED:
 * P4.7 - Blue (overlap with MX direction, Do Not Use)
 * P5.0 - Green (overlap with MX enable, Do Not Use)
 * P5.1 - Red (no overlap)
 *
 */
#define LED1_PORT P1
#define S1_PORT P2
#define S2_PORT P4
#define LED1_PORT_SEL0   P1SEL0
#define LED1_PORT_SEL1   P1SEL1
#define LED1_PORT_DIR    P1DIR
#define LED1_PORT_OUT    P1OUT
#define LED1_PIN         BIT0 //0th pin, 0x01

#define LEDB_PORT P4
#define LEDB_PORT_SEL0   P4SEL0
#define LEDB_PORT_SEL1   P4SEL1
#define LEDB_PORT_DIR    P4DIR
#define LEDB_PORT_OUT    P4OUT
#define LEDB_PIN         BIT7

#define LEDR_PORT P5
#define LEDR_PORT_SEL0   P5SEL0
#define LEDR_PORT_SEL1   P5SEL1
#define LEDR_PORT_DIR    P5DIR
#define LEDR_PORT_OUT    P5OUT
#define LEDR_PIN         BIT1

#define LEDG_PORT P5
#define LEDG_PORT_SEL0   P5SEL0
#define LEDG_PORT_SEL1   P5SEL1
#define LEDG_PORT_DIR    P5DIR
#define LEDG_PORT_OUT    P5OUT
#define LEDG_PIN         BIT0

#define S1_PORT_SEL0 P2SEL0
#define S1_PORT_SEL1 P2SEL1
#define S1_PORT_DIR P2DIR
#define S1_PORT_OUT P2OUT
#define S1_PORT_IFG P2IFG
#define S1_PORT_IE P2IE
#define S1_PORT_IES P2IES
#define S1_PORT_IN P2IN
#define S1_PORT_REN P2REN
#define S1_PIN BIT3 

#define S2_PORT_SEL0 P4SEL0
#define S2_PORT_SEL1 P4SEL1
#define S2_PORT_DIR P4DIR
#define S2_PORT_OUT P4OUT
#define S2_PORT_IFG P4IFG
#define S2_PORT_IE P4IE
#define S2_PORT_IES P4IES
#define S2_PORT_IN P4IN
#define S2_PORT_REN P4REN
#define S2_PIN BIT0 


/**
 * Limit Switches
 * MX_Limit0 - P2.1
 * MX_Limit1 - P2.0
 *
 * MY_Limit0 - P2.7
 * MY_Limit1 - P2.4
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

#define MX1_LIMIT_PORT P2
#define MX1_LIMIT_PORT_SEL0 P2SEL0
#define MX1_LIMIT_PORT_SEL1 P2SEL1
#define MX1_LIMIT_PORT_DIR P2DIR
#define MX1_LIMIT_PORT_OUT P2OUT
#define MX1_LIMIT_PORT_IFG P2IFG
#define MX1_LIMIT_PORT_IE P2IE
#define MX1_LIMIT_PORT_IES P2IES
#define MX1_LIMIT_PORT_IN P2IN
#define MX1_LIMIT_PORT_REN P2REN

#define MX_LIMIT_0_PIN BIT1 
#define MX_LIMIT_1_PIN BIT0 
#define MX_LIM0 (MX0_LIMIT_PORT_IN & MX_LIMIT_0_PIN)
#define MX_LIM1 (MX1_LIMIT_PORT_IN & MX_LIMIT_1_PIN)


#define MY_LIMIT_PORT P2
#define MY_LIMIT_PORT_SEL0 P3SEL0
#define MY_LIMIT_PORT_SEL1 P3SEL1
#define MY_LIMIT_PORT_DIR P3DIR
#define MY_LIMIT_PORT_OUT P3OUT
#define MY_LIMIT_PORT_IFG P3IFG
#define MY_LIMIT_PORT_IE P3IE
#define MY_LIMIT_PORT_IES P3IES
#define MY_LIMIT_PORT_IN P3IN
#define MY_LIMIT_PORT_REN P3REN
#define MY_LIM0 (MY_LIMIT_PORT_IN & MY_LIMIT_1_PIN)
#define MY_LIM1 (MY_LIMIT_PORT_IN & MY_LIMIT_0_PIN)

#define MY_LIMIT_1_PIN BIT4
#define MY_LIMIT_0_PIN BIT7 
#define MY_LIMIT_PINS (MY_LIMIT_0_PIN | MY_LIMIT_1_PIN)

#define MOTOR_X_FORWARD (MX_DIR_PORT_OUT |= MX_DIR_PIN)
#define MOTOR_Y_BACKWARD (MY_DIR_PORT_OUT |= MY_DIR_PIN)
#define MOTOR_X_BACKWARD (MX_DIR_PORT_OUT &= ~MX_DIR_PIN)
#define MOTOR_Y_FORWARD (MY_DIR_PORT_OUT &= ~MY_DIR_PIN)

#define MX_ENABLE  (MX_ENABLE_PORT_OUT &= ~(MX_ENABLE_PIN))
#define MX_DISABLE (MX_ENABLE_PORT_OUT |=  (MX_ENABLE_PIN))
#define MY_ENABLE  (MY_ENABLE_PORT_OUT &= ~(MY_ENABLE_PIN))
#define MY_DISABLE (MY_ENABLE_PORT_OUT |=  (MY_ENABLE_PIN))

#define WAIT_500MS (__delay_cycles(48000000>>2));

#endif /* PORTPINS_H_ */
