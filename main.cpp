#include <msp.h>
#include <stdint.h>
#include <stdbool.h>

#include "Steppers.h"
#include "Pump.h"
#include "DebugIO.h"

void testPumpLoop();
void mainloop();
/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    SystemInit();
    InitDebugIO();
    InitStepperMotors(1000,1000);
    InitPump();
    while(1){
        testPumpLoop();
    }


    DebugLED1On();
    while(!(DebugReadS1())){
        __delay_cycles(100);
    }
//  TestSteppers();
    PumpStop();
    DebugLED1Off();
    __delay_cycles(25000000);  // .5s
    CalibrateSteppers();

//    mainloop();
//  whack();

//  return 0;
}

void testPumpLoop(){
    PumpDrive();
    __delay_cycles(1000);
    PumpSetSpeed(0.3);
    __delay_cycles(1000);
    PumpSetSpeed(0.1);
    __delay_cycles(1000);
    PumpDrive();
    __delay_cycles(1000);
}
//void mainloop(){
//    static uint8_t notified = 0;
//    while(1){
//        notified = 0;
//        while(QueueSize()>0){
//            // Execute Command
//            Command* c = DequeueCommand();
//            if(c != 0)
//                ExecuteCommand(c);
//        }
//        while(QueueSize()==0){
//            // Twiddle Thumbs
////            if(!notified){
//                UART_Write(0);
////                notified = 1;
////            }
//            __no_operation();
//        }
//    }
//}
