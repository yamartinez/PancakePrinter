/*
 * DebugIO.c
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#include "./DebugIO.h"

void InitLED1(){
    LED1_PORT_SEL0 &= ~LED1_PIN;
    LED1_PORT_SEL1 &= ~LED1_PIN;
    LED1_PORT_DIR  |= LED1_PIN;
    LED1_PORT_OUT  &= ~LED1_PIN;
}

void InitSwitches(){
    SWITCH_PORT_SEL0 &= ~SWITCH_PINS;
    SWITCH_PORT_SEL1 &= ~SWITCH_PINS;
    SWITCH_PORT_DIR &= ~SWITCH_PINS;
    SWITCH_PORT_OUT |=  SWITCH_PINS;
    SWITCH_PORT_REN |=  SWITCH_PINS;
}

void InitDebugIO(){
    InitLED1();
    InitSwitches();
}

bool DebugReadS1(){
    return (SWITCH_PORT_IN & S1_PIN ? false : true);
}

bool DebugReadS2(){
    return (SWITCH_PORT_IN & S2_PIN ? false : true);
}
void DebugLED1On(){
    LED1_PORT_OUT |= LED1_PIN;
}
void DebugLED1Off(){
    LED1_PORT_OUT &= ~LED1_PIN;
}
void DebugToggleLED1(){
    LED1_PORT_OUT ^= (LED1_PIN);
}
