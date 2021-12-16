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
#include "msp432.h"
#include "./Error.h"


/**
 *  MX enable - P5.5
 *  MY enable - P2.4
 * 
 *  recall that enables are to be held high
 * 
 */
#define MX_ENABLE_PORT P5 
#define MX_ENABLE_PIN BIT5 //5th pin, 0x10
#define MX_ENABLE_PORT_SEL0 MX_ENABLE_PORT->SEL0
#define MX_ENABLE_PORT_SEL1 MX_ENABLE_PORT->SEL1
#define MX_ENABLE_PORT_DIR MX_ENABLE_PORT->DIR
#define MX_ENABLE_PORT_OUT MX_ENABLE_PORT->OUT

#define MY_ENABLE_PORT P2
#define MY_ENABLE_PIN BIT4 //4th pin, 0x08
#define MY_ENABLE_PORT_SEL0 MY_ENABLE_PORT->SEL0
#define MY_ENABLE_PORT_SEL1 MY_ENABLE_PORT->SEL1
#define MY_ENABLE_PORT_DIR MY_ENABLE_PORT->DIR
#define MY_ENABLE_PORT_OUT MY_ENABLE_PORT->OUT
/**
 * MX direction - P3.7
 * MY direction - P5.6
 * 
 */

#define MX_DIR_PORT P3
#define MX_DIR_PIN BIT7 //7th pin, 0x80
#define MX_DIR_PORT_SEL0 MX_DIR_PORT->SEL0
#define MX_DIR_PORT_SEL1 MX_DIR_PORT->SEL1
#define MX_DIR_PORT_DIR MX_DIR_PORT->DIR
#define MX_DIR_PORT_OUT MX_DIR_PORT->OUT


#define MY_DIR_PORT P5
#define MY_DIR_PIN BIT6 //6th pin, 0x40
#define MY_DIR_PORT_SEL0 MY_DIR_PORT->SEL0
#define MY_DIR_PORT_SEL1 MY_DIR_PORT->SEL1
#define MY_DIR_PORT_DIR MY_DIR_PORT->DIR
#define MY_DIR_PORT_OUT MY_DIR_PORT->OUT

/**
 * MX uS0 - P3.5
 * MX uS1 - P4.6
 * 
 * MY uS0 - P6.6
 * MY uS1 - P6.7 
 * 
 * hold low unless microstepping is viable and needed
 * 
 */
#define MX_uS0_PORT P3
#define MX_uS0_PIN BIT5 //5th pin, 0x20
#define MX_uS0_PORT_SEL0 MX_uS0_PORT->SEL0
#define MX_uS0_PORT_SEL1 MX_uS0_PORT->SEL1
#define MX_uS0_PORT_DIR MX_uS0_PORT->DIR
#define MX_uS0_PORT_OUT MX_uS0_PORT->OUT 

#define MY_uS0_PORT P6
#define MY_uS0_PIN BIT6 //6th pin, 0x40
#define MY_uS0_PORT_SEL0 MY_uS0_PORT->SEL0
#define MY_uS0_PORT_SEL1 MY_uS0_PORT->SEL1
#define MY_uS0_PORT_DIR MY_uS0_PORT->DIR
#define MY_uS0_PORT_OUT MY_uS0_PORT->OUT


#define MX_uS1_PORT P4
#define MX_uS1_PIN BIT6  //6th pin, 0x40
#define MX_uS1_PORT_SEL0 MX_uS1_PORT->SEL0
#define MX_uS1_PORT_SEL1 MX_uS1_PORT->SEL1
#define MX_uS1_PORT_DIR MX_uS1_PORT->DIR
#define MX_uS1_PORT_OUT MX_uS1_PORT->OUT

#define MY_uS1_PORT P6
#define MY_uS1_PIN BIT7 // 7th pin, 0x80
#define MY_uS1_PORT_SEL0 MY_uS1_PORT->SEL0
#define MY_uS1_PORT_SEL1 MY_uS1_PORT->SEL1
#define MY_uS1_PORT_DIR MY_uS1_PORT->DIR
#define MY_uS1_PORT_OUT MY_uS1_PORT->OUT

/**
 * MX Reset - P6.4
 * MY Reset - P1.3
 * 
 * recall the reset pins are active low, so they are to be held high under normal operation
 * 
 */
#define MX_RESET_PORT P6
#define MX_RESET_PIN BIT4 //4th pin, 0x08
#define MX_RESET_PORT_SEL0 MX_RESET_PORT->SEL0
#define MX_RESET_PORT_SEL1 MX_RESET_PORT->SEL1
#define MX_RESET_PORT_DIR MX_RESET_PORT->DIR
#define MX_RESET_PORT_OUT MX_RESET_PORT->OUT

#define MY_RESET_PORT P1
#define MY_RESET_PIN BIT3 //3rd pin, 0x04
#define MY_RESET_PORT_SEL0 MY_RESET_PORT->SEL0
#define MY_RESET_PORT_SEL1 MY_RESET_PORT->SEL0
#define MY_RESET_PORT_DIR MY_RESET_PORT->DIR
#define MY_RESET_PORT_OUT MY_RESET_PORT->OUT


/**
 * MX Step - P4.2
 * MY Step - P4.4
 * 
 */

#define MX_STEP_PORT P4
#define MX_STEP_PIN BIT2 //2nd pin, 0x02
#define MX_STEP_PORT_SEL0 MX_STEP_PORT->SEL0
#define MX_STEP_PORT_SEL1 MX_STEP_PORT->SEL1
#define MX_STEP_PORT_DIR MX_STEP_PORT->DIR
#define MX_STEP_PORT_OUT MX_STEP_PORT->OUT


#define MY_STEP_PORT P4
#define MY_STEP_PIN BIT4 //4th pin, 0x08
#define MY_STEP_PORT_SEL0 MY_STEP_PORT->SEL0
#define MY_STEP_PORT_SEL1 MY_STEP_PORT->SEL1
#define MY_STEP_PORT_DIR MY_STEP_PORT->DIR
#define MY_STEP_PORT_OUT MY_STEP_PORT->OUT

/**
 * Pump PWM output - P2.5
 * 
 */

#define PUMP_PORT P2
#define PUMP_PWM_PIN BIT5 //5th pin, 0x20
#define PUMP_PORT_SEL0 PUMP_PORT->SEL0
#define PUMP_PORT_SEL1 PUMP_PORT->SEL1
#define PUMP_PORT_DIR PUMP_PORT->DIR
#define PUMP_PORT_OUT PUMP_PORT->OUT

/**
 * Debug outputs
 * LED1 - P1.0
 * S1 - P1.1
 * S2 - P1.4
 * 
 */
#define LED1_PORT P1
#define SWITCH_PORT P1
#define LED1_PORT_SEL0   LED1_PORT->SEL0
#define LED1_PORT_SEL1   LED1_PORT->SEL1
#define LED1_PORT_DIR    LED1_PORT->DIR
#define LED1_PORT_OUT    LED1_PORT->OUT
#define LED1_PIN         BIT0 //0th pin, 0x01
#define SWITCH_PORT_SEL0 SWITCH_PORT->SEL0
#define SWITCH_PORT_SEL1 SWITCH_PORT->SEL1
#define SWITCH_PORT_DIR SWITCH_PORT->DIR
#define SWITCH_PORT_OUT SWITCH_PORT->OUT
#define SWITCH_PORT_IFG SWITCH_PORT->IFG
#define SWITCH_PORT_IE SWITCH_PORT->IE
#define SWITCH_PORT_IES SWITCH_PORT->IES
#define SWITCH_PORT_IN SWITCH_PORT->IN
#define SWITCH_PORT_REN SWITCH_PORT->REN
#define SWITCH_PINS     (BIT1 | BIT4) //1st and 4th pins, (0x01 | 0x10)
#define S1_PIN  BIT1 //1ST PIN, 0X01
#define S2_PIN  BIT4 //4TH PIN, 0X10

/**
 * Limit Switches
 * M_Limit0 - P1.6
 * M_Limit1 - P1.7
 * 
 */

#define M_LIMIT_PORT P1
#define M_LIMIT_PORT_SEL0 M_LIMIT_PORT->SEL0
#define M_LIMIT_PORT_SEL1 M_LIMIT_PORT->SEL1
#define M_LIMIT_PORT_DIR M_LIMIT_PORT->DIR
#define M_LIMIT_PORT_OUT M_LIMIT_PORT->OUT
#define M_LIMIT_PORT_IFG M_LIMIT_PORT->IFG
#define M_LIMIT_PORT_IE M_LIMIT_PORT->IE
#define M_LIMIT_PORT_IES M_LIMIT_PORT->IES
#define M_LIMIT_PORT_IN M_LIMIT_PORT->IN
#define M_LIMIT_PORT_REN M_LIMIT_PORT->REN

#define M_LIMIT_0_PIN BIT6 //0x40, bit 6
#define M_LIMIT_1_PIN BIT7 //0x80, bit 7

#define M_LIMIT_PINS (M_LIMIT_0_PIN | M_LIMIT_1_PIN)

#define MOTOR_X_FORWARD (MX_DIR_PORT_OUT|=MX_DIR_PIN)
#define MOTOR_Y_FORWARD (MY_DIR_PORT_OUT|=MY_DIR_PIN)
#define MOTOR_X_BACKWARD (MX_DIR_PORT_OUT&=~MX_DIR_PIN)
#define MOTOR_Y_BACKWARD (MY_DIR_PORT_OUT&=~MY_DIR_PIN)

#endif /* PORTPINS_H_ */
