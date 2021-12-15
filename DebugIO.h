/*
 * DebugIo.h
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#ifndef DEBUGIO_H_
#define DEBUGIO_H_

#include <stdbool.h>
#include <msp430.h>

#define LED_PORT P1
#define LED_PORT_SEL0   P1SEL0
#define LED_PORT_SEL1   P1SEL1
#define LED_PORT_DIR    P1DIR
#define LED_PORT_OUT    P1OUT
#define LED_PIN         0x01

#define ON_LED_PORT P6
#define ON_LED_PORT_SEL0   P6SEL0
#define ON_LED_PORT_SEL1   P6SEL1
#define ON_LED_PORT_DIR    P6DIR
#define ON_LED_PORT_OUT    P6OUT
#define ON_LED_PIN         BIT6

#define BUTTON_PORT P4
#define BUTTON_PORT_SEL0   P4SEL0
#define BUTTON_PORT_SEL1   P4SEL1
#define BUTTON_PORT_DIR    P4DIR
#define BUTTON_PORT_OUT    P4OUT
#define BUTTON_PORT_IFG    P4IFG
#define BUTTON_PORT_IE     P4IE
#define BUTTON_PORT_IES    P4IES
#define BUTTON_PORT_IN     P4IN
#define BUTTON_PORT_REN    P4REN
#define BUTTON_PIN         0x02

void InitLED();
void InitButton();
void InitDebugIO();

bool DebugReadButton();
void DebugLEDOn();
void DebugLEDOff();
void ONLEDOn();
void ONLEDOff();
void DebugToggleLED();

#endif
