/*
 * DebugIO.c
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#include "./DebugIO.h"

void InitLED(){
    LED1_PORT_SEL0 &= ~LED1_PIN;
    LED1_PORT_SEL1 &= ~LED1_PIN;
    LED1_PORT_DIR  |=  LED1_PIN;
    LED1_PORT_OUT  &= ~LED1_PIN;
}


void InitButton(){
    S1_PORT_SEL0 &= ~S1_PIN;
    S1_PORT_SEL1 &= ~S1_PIN;
    S1_PORT_DIR  &= ~S1_PIN;
    S1_PORT_OUT  |=  S1_PIN;
    S1_PORT_REN  |=  S1_PIN;
}

void InitDebugIO(){
    InitLED();
    InitButton();

}

bool DebugReadButton(){
    return (S1_PORT_IN & S1_PIN ? false : true);
}
void DebugLEDOn(){
    LED1_PORT_OUT |= LED1_PIN;
}
void DebugLEDOff(){
    LED1_PORT_OUT &= ~LED1_PIN;
}

void DebugToggleLED(){
    LED1_PORT_OUT ^= (LED1_PIN);
}
