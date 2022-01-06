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

void InitLEDB(){
//    LEDB_PORT_SEL0 &= ~LEDB_PIN;
//    LEDB_PORT_SEL1 &= ~LEDB_PIN;
//    LEDB_PORT_DIR  |=  LEDB_PIN;
//    LEDB_PORT_OUT  &= ~LEDB_PIN;
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

void LEDBon(){
    //LEDB_PORT_OUT |= LEDB_PIN;
}

void LEDBoff(){
//    LEDB_PORT_OUT |= LEDB_PIN;
}

void ToggleLEDB(){
//    LEDB_PORT_OUT ^= (LEDB_PIN);
}

void LEDRon(){
//    LEDR_PORT_OUT |= LEDR_PIN;
}

void LEDRoff(){
//    LEDR_PORT_OUT &= ~LEDR_PIN;
}

void ToggleLEDR(){
//    LEDR_PORT_OUT ^= (LEDR_PIN);
}

void LEDGon(){
//    LEDG_PORT_OUT |= LEDG_PIN;
}

void LEDGoff(){
//    LEDG_PORT_OUT |= LEDG_PIN;
}

void ToggleLEDG(){
//    LEDG_PORT_OUT ^= (LEDG_PIN);
}
