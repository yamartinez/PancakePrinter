/*
 * DebugIO.c
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#include "./DebugIO.h"

void InitLED(){
    LED_PORT_SEL0 &= ~LED_PIN;
    LED_PORT_SEL1 &= ~LED_PIN;
    LED_PORT_DIR  |=  LED_PIN;
    LED_PORT_OUT  &= ~LED_PIN;
}

void InitONLED(){
    ON_LED_PORT_SEL0 &= ~ON_LED_PIN;
    ON_LED_PORT_SEL1 &= ~ON_LED_PIN;
    ON_LED_PORT_DIR  |=  ON_LED_PIN;
    ON_LED_PORT_OUT  |=  ON_LED_PIN;
}

void InitButton(){
    BUTTON_PORT_SEL0 &= ~BUTTON_PIN;
    BUTTON_PORT_SEL1 &= ~BUTTON_PIN;
    BUTTON_PORT_DIR  &= ~BUTTON_PIN;
    BUTTON_PORT_OUT  |=  BUTTON_PIN;
    BUTTON_PORT_REN  |=  BUTTON_PIN;
}

void InitDebugIO(){
    InitLED();
    InitButton();
    InitONLED();
}

bool DebugReadButton(){
    return (BUTTON_PORT_IN & BUTTON_PIN ? false : true);
}
void DebugLEDOn(){
    LED_PORT_OUT |= LED_PIN;
}
void DebugLEDOff(){
    LED_PORT_OUT &= ~LED_PIN;
}
void ONLEDOn(){
    ON_LED_PORT_OUT |= ON_LED_PIN;
}
void ONLEDOff(){
    ON_LED_PORT_OUT &= ~ON_LED_PIN;
}
void DebugToggleLED(){
    LED_PORT_OUT ^= (LED_PIN);
}
