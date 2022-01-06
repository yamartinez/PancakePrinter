/*
 * Error.c
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#include "./Error.h"
#include "DebugIO.h"
#include "Pump.h"
#include "Steppers.h"
#include <msp430.h>

void error(error_code err, char*file, int line){
    //turn on error LED
    MX_DISABLE;
    MY_DISABLE;
    PumpStop();
    for(;;)
    {
        P5OUT |= BIT1;
        __delay_cycles(1000000);
        P5OUT &= ~BIT1;
        __delay_cycles(1000000);
        continue;
        if (err == 0){
            DebugLEDOn();
        } else if (err == 1){
            //DebugLEDOn();
            ToggleLEDR();
            __delay_cycles(1000000);
            ToggleLEDR();
            __delay_cycles(1000000);
            //ONLEDOn();
            ToggleLEDR();
        } else if (err == 2){
            DebugLEDOn();
            //ONLEDOn();
            ToggleLEDR();
            __delay_cycles(1000000);
            //ONLEDOff();
            ToggleLEDR();
            __delay_cycles(1000000);
            ToggleLEDR();

        } else if (err == 3){
            DebugLEDOff();
            //ONLEDOn();
            ToggleLEDR();
            __delay_cycles(1000000);
            //ONLEDOff();
            ToggleLEDR();
            __delay_cycles(1000000);
            ToggleLEDR();
        } else if (err == 4){
            DebugLEDOn();
            //ONLEDOff();
            ToggleLEDR();
        } else if (err == 5){
            DebugLEDOff();
            //ONLEDOn();
            ToggleLEDR();
            __delay_cycles(1000000);
            //ONLEDOff();
            ToggleLEDR();
            DebugLEDOn();
            __delay_cycles(1000000);
            ToggleLEDR();

        } else if (err == 6){
            DebugLEDOn();
            //ONLEDOn();
            ToggleLEDR();
            __delay_cycles(1000000);
            //ONLEDOff();
            ToggleLEDR();
            DebugLEDOff();
            __delay_cycles(1000000);
            ToggleLEDR();

        } else if (err == 7){
            //ONLEDOn();
            DebugLEDOff();
            ToggleLEDR();
        }
    }
}
