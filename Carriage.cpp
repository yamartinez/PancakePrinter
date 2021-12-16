/*
 * Carriage.cpp
 *
 *  Created on: Dec 6, 2021
 *      Author: yudel
 */

#include "Carriage.h"
#include "Steppers.h"

void CarriageMove(uint16_t x,uint16_t y,uint8_t p){
    if(p){
        PumpDrive();
    }
    move_image(x, y);
    if(p){
        PumpStop();
    }
}
void CarriageWait(uint16_t t){
    for(uint16_t i = t; i>0; i--){
        // Replace with better delay
        __delay_cycles(100);
    }
}
void CarriageDone(){
    move_absolute(0,0);
}
