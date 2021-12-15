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
        if (err == 0){
            DebugLEDOn();
            ONLEDOn();
        } else if (err == 1){
            DebugLEDOn();
            __delay_cycles(1000000);
            DebugLEDOff();
            __delay_cycles(1000000);
            ONLEDOn();
        } else if (err == 2){
            DebugLEDOn();
            ONLEDOn();
            __delay_cycles(1000000);
            ONLEDOff();
            __delay_cycles(1000000);

        } else if (err == 3){
            DebugLEDOff();
            ONLEDOn();
            __delay_cycles(1000000);
            ONLEDOff();
            __delay_cycles(1000000);
        } else if (err == 4){
            DebugLEDOn();
            ONLEDOff();
        } else if (err == 5){
            DebugLEDOff();
            ONLEDOn();
            __delay_cycles(1000000);
            ONLEDOff();
            DebugLEDOn();
            __delay_cycles(1000000);

        } else if (err == 6){
            DebugLEDOn();
            ONLEDOn();
            __delay_cycles(1000000);
            ONLEDOff();
            DebugLEDOff();
            __delay_cycles(1000000);

        } else if (err == 7){
            ONLEDOn();
            DebugLEDOff();
        }
    }
}
