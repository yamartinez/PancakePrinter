/*
 * Steppers.h
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#ifndef STEPPERS_H_
#define STEPPERS_H_

#include <stdbool.h>
#include <stdint.h>
#include <msp430.h>
#include "./Error.h"
#include "./DebugIO.h"

#define MX_PORT P3
#define MX_PORT_SEL0  P3SEL0
#define MX_PORT_SEL1  P3SEL1
#define MX_PORT_DIR   P3DIR
#define MX_PORT_OUT   P3OUT

#define MY_PORT P6
#define MY_PORT_SEL0  P6SEL0
#define MY_PORT_SEL1  P6SEL1
#define MY_PORT_DIR   P6DIR
#define MY_PORT_OUT   P6OUT
/**
 * Px.1: Enable
 * Px.2: Direction
 * Px.3: uStep0
 * Px.4: uStep1
*/
#define M_ENB BIT1
#define M_DIR BIT2
#define M_uS0 BIT3
#define M_uS1 BIT4
#define MCPINS (M_ENB|M_DIR|M_uS0|M_uS1)

#define MX_ENABLE   MX_PORT_OUT &= ~M_ENB;
#define MX_DISABLE  MX_PORT_OUT |=  M_ENB;
#define MY_ENABLE   MY_PORT_OUT &= ~M_ENB;
#define MY_DISABLE  MY_PORT_OUT |=  M_ENB;

/**
 * P1.2 -> Reset Motor X
 * P1.3 -> Reset Motor Y
*/
#define M_RESET_PORT P1
#define M_RESET_PORT_SEL0   P1SEL0
#define M_RESET_PORT_SEL1   P1SEL1
#define M_RESET_PORT_DIR    P1DIR
#define M_RESET_PORT_OUT    P1OUT
#define MX_RESET BIT2
#define MY_RESET BIT3

/**
 * P5.0 -> Step Motor Y
 * P5.3 -> Step Motor X
*/
#define M_STEP_PORT P5
#define M_STEP_PORT_SEL0    P5SEL0
#define M_STEP_PORT_SEL1    P5SEL1
#define M_STEP_PORT_DIR     P5DIR
#define M_STEP_PORT_OUT     P5OUT
#define MY_STEP BIT0
#define MX_STEP BIT3

/**
 * Limit Switches
 * P4.0 X0
 * P4.4 X1
 * P4.6 Y0
 * P4.7 Y1
*/
#define M_LIMIT_PORT P4
#define M_LIMIT_PORT_SEL0   P4SEL0
#define M_LIMIT_PORT_SEL1   P4SEL1
#define M_LIMIT_PORT_DIR    P4DIR
#define M_LIMIT_PORT_OUT    P4OUT
#define M_LIMIT_PORT_IFG    P4IFG
#define M_LIMIT_PORT_IE     P4IE
#define M_LIMIT_PORT_IES    P4IES
#define M_LIMIT_PORT_IN     P4IN
#define M_LIMIT_PORT_REN    P4REN
#define MX_LIM0 BIT0
#define MX_LIM1 BIT4
#define MY_LIM0 BIT6
#define MY_LIM1 BIT7
#define MLIMPINS (MX_LIM0 | MX_LIM1 | MY_LIM0 | MY_LIM1)

#define MOTOR_X_FORWARD  (MX_PORT_OUT &=~M_DIR)
#define MOTOR_Y_FORWARD  (MY_PORT_OUT &=~M_DIR)
#define MOTOR_X_BACKWARD (MX_PORT_OUT |= M_DIR)
#define MOTOR_Y_BACKWARD (MY_PORT_OUT |= M_DIR)

void InitStepperMotors(uint16_t, uint16_t);
void CalibrateSteppers();
void move_absolute(int16_t x, int16_t y);
void move_image(int16_t x, int16_t y);
void fill_image_square();
void TestSteppers();

typedef enum direction{
    none,
    backward,
    forward
} direction;

#endif /* STEPPERS_H_ */
